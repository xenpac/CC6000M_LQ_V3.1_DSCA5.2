#include "config.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include "viewer-render.h"
//#include "viewer.h"
//#include "pango-layout.h"
#include <pango/pangoft2.h>

#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"
#include "common.h"
#include "osd.h"
#include "gpe.h"
#include "gpe_vsb.h"

#include "mdl.h"
#include "mmi.h"
#include "surface.h"
#include "flinger.h"
#include "gdi.h"
#include "gdi_dc.h"
#include "gdi_anim.h"

#include "gui_resource.h"
#include "gui_pango.h"

#include "ctrl_base.h"
#include "ctrl_common.h"

typedef u32 (*draw_unistr_t)(handle_t h_pango, hdc_t hdc, rect_t *p_rc, u32 style,
  u16 x_offset, u16 y_offset, u8 line_space, u16 *p_str, rsc_fstyle_t *p_fstyle, u32 draw_style);

typedef void (*RenderCallback) (PangoLayout *layout,
  int x, int y, gpointer cb_context, gpointer cb_data);
  
typedef void (*TransformCallback) (PangoContext *context,
  PangoMatrix  *transform, gpointer cb_context, gpointer cb_data);				

typedef struct
{
  draw_unistr_t draw_unistr;

  gpointer instance;

  PangoContext *p_context;
  char *p_text;

  double dpix;
  double dpiy;
  double opt_rotate;
  gboolean opt_rtl;
  gboolean opt_justify;
  PangoAlignment opt_align;
  gboolean opt_auto_dir;
  PangoEllipsizeMode opt_ellipsize;
  PangoWrapMode opt_wrap;
  gboolean opt_single_par;
  int opt_width;
  int opt_height;
  int opt_indent;
  int opt_margin_t;
  int opt_margin_r;
  int opt_margin_b;
  int opt_margin_l;  
}gui_pango_t;

static PangoLayout *make_layout(handle_t h_pango,
  PangoContext *context, const char *text, double size)
{
  static PangoFontDescription *font_description;
  PangoAlignment align;
  PangoLayout *layout;
  gui_pango_t *p_info = NULL;
  
  p_info = (gui_pango_t *)h_pango;
  MT_ASSERT(p_info != NULL);
  
  layout = pango_layout_new (context);

  pango_layout_set_text (layout, text, -1);

  pango_layout_set_auto_dir (layout, p_info->opt_auto_dir);
  pango_layout_set_ellipsize (layout, p_info->opt_ellipsize);
  pango_layout_set_justify (layout, p_info->opt_justify);
  pango_layout_set_single_paragraph_mode (layout, p_info->opt_single_par);
  pango_layout_set_wrap (layout, p_info->opt_wrap);

  if(p_info->opt_width > 0)
  {
    pango_layout_set_width (layout, (p_info->opt_width * PANGO_SCALE));
  }
  
  if(p_info->opt_height > 0)
  {
    pango_layout_set_height (layout, (p_info->opt_height * PANGO_SCALE));
  }
  else
  {
    pango_layout_set_height (layout, p_info->opt_height);
  }
  
  if (p_info->opt_indent != 0)
  {
    pango_layout_set_indent (layout, (p_info->opt_indent * PANGO_SCALE));
  }
  
  align = p_info->opt_align;
  
  if(align != PANGO_ALIGN_CENTER && pango_context_get_base_dir (context) != PANGO_DIRECTION_LTR) 
  {
    align = PANGO_ALIGN_LEFT + PANGO_ALIGN_RIGHT - align;
  }
  pango_layout_set_alignment (layout, align);

  pango_layout_set_font_description (layout, font_description);

  pango_font_description_free (font_description);

  return layout;
}

static void output_body (PangoLayout *layout,
	     RenderCallback  render_cb,
	     gpointer        cb_context,
	     gpointer        cb_data,
	     int            *width,
	     int            *height,
	     int            *actual_w,
	     int            *actual_h,
	     gboolean        supports_matrix)
{
  PangoRectangle logical_rect;
  int x = 0, y = 0;
  PangoFontDescription *desc = NULL;

  if (!supports_matrix)
  {
    const PangoMatrix* matrix;
    const PangoMatrix identity = PANGO_MATRIX_INIT;
    PangoContext *context = pango_layout_get_context (layout);
    matrix = pango_context_get_matrix (context);
    if (matrix)
    {
      x += matrix->x0;
      y += matrix->y0;
    }
    pango_context_set_matrix (context, &identity);
    pango_layout_context_changed (layout);
  }

  *width = 0;
  *height = 0;
//OS_PRINTF("[output_body]~~~~~~~~~~~~~~~~~~~~~\n");
  desc = pango_font_description_copy (pango_layout_get_font_description (layout));
  //OS_PRINTF("[output_body]pango_font_description_copy\n");
  pango_font_description_set_size (desc, PANGO_SCALE);
  //OS_PRINTF("[output_body]pango_font_description_set_size\n");
  pango_layout_set_font_description (layout, desc);
  //OS_PRINTF("[output_body]pango_layout_set_font_description\n");
  pango_font_description_free (desc);
//OS_PRINTF("[output_body]pango_font_description_free\n");
  pango_layout_get_pixel_extents (layout, NULL, &logical_rect);
//OS_PRINTF("[output_body]pango_layout_get_pixel_extents\n");
  if (render_cb)
  {
    (*render_cb) (layout, x, y + *height, cb_context, cb_data);
  }
  
  *width = MAX(*width, MAX(logical_rect.x + logical_rect.width, PANGO_PIXELS(pango_layout_get_width(layout))));
  *height += MAX(logical_rect.y + logical_rect.height, PANGO_PIXELS (pango_layout_get_height (layout)));
  *actual_w = logical_rect.width;
  *actual_h = logical_rect.height;
}

static void set_transform (PangoContext *context,
	       TransformCallback transform_cb,
	       gpointer          cb_context,
	       gpointer          cb_data,
	       PangoMatrix      *matrix)
{
  pango_context_set_matrix (context, matrix);
  if (transform_cb)
  {
    (*transform_cb) (context, matrix, cb_context, cb_data);
  }
}

void do_output (handle_t h_pango, PangoContext *context,
	   RenderCallback    render_cb,
	   TransformCallback transform_cb,
	   gpointer          cb_context,
	   gpointer          cb_data,
	   int              *width_out,
	   int              *height_out,
	   int              *actual_w,
	   int              *actual_h)
{
  PangoLayout *layout;
  PangoRectangle rect;
  PangoMatrix matrix = PANGO_MATRIX_INIT;
  PangoMatrix *orig_matrix;
  gboolean supports_matrix;
  int rotated_width, rotated_height;
  int x = 0;
  int y = 0;
  int width, height;
  int a_w = 0, a_h = 0;
  gui_pango_t *p_info = NULL;
  
  p_info = (gui_pango_t *)h_pango;
  MT_ASSERT(p_info != NULL);

  x = p_info->opt_margin_l;
  y = p_info->opt_margin_t;
  width = 0;
  height = 0;

  orig_matrix = pango_matrix_copy(pango_context_get_matrix(context));

  supports_matrix = !orig_matrix ||
		    (orig_matrix->xx != 0. || orig_matrix->xy != 0. ||
		     orig_matrix->yx != 0. || orig_matrix->yy != 0. ||
		     orig_matrix->x0 != 0. || orig_matrix->y0 != 0.);

  set_transform(context, transform_cb, cb_context, cb_data, NULL);

  pango_context_set_language(context, pango_language_get_default ());
  pango_context_set_base_dir (context, PANGO_DIRECTION_LTR);

  if(p_info->opt_rotate != 0)
  {
    if (supports_matrix)
    {
      pango_matrix_rotate (&matrix, p_info->opt_rotate);
    }
    else
    {
      g_printerr ("The backend does not support rotated text\n");
    }
  }

  //pango_context_set_base_gravity (context, opt_gravity);
  //pango_context_set_gravity_hint (context, opt_gravity_hint);
//OS_PRINTF("[do_output]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  layout = make_layout(h_pango, context, p_info->p_text, -1);
//OS_PRINTF("[do_output]make_layout\n");
  set_transform (context, transform_cb, cb_context, cb_data, &matrix);

  output_body(layout,
	       NULL, NULL, NULL,
	       &rotated_width, &rotated_height, &a_w, &a_h,
	       supports_matrix);

  rect.x = rect.y = 0;
  rect.width = rotated_width;
  rect.height = rotated_height;
//OS_PRINTF("[do_output]output_body 1\n");
  pango_matrix_transform_pixel_rectangle (&matrix, &rect);

  matrix.x0 = x - rect.x;
  matrix.y0 = y - rect.y;

  set_transform (context, transform_cb, cb_context, cb_data, &matrix);

  if (render_cb)
  {
    output_body (layout, render_cb, cb_context, cb_data,
		 &rotated_width, &rotated_height, &a_w, &a_h,
		 supports_matrix);
  }
//OS_PRINTF("[do_output]output_body 2\n");  
  width = MAX (width, rect.width);
  height += rect.height;

  width += p_info->opt_margin_l + p_info->opt_margin_r;
  height += p_info->opt_margin_t + p_info->opt_margin_b;

  if(width_out)
  {
    *width_out = width;
  }
  
  if(height_out)
  {
    *height_out = height;
  }

  if(actual_w)
  {
    *actual_w = a_w;
  }

  if(actual_h)
  {
    *actual_h = a_h;
  }
  
  pango_context_set_matrix (context, orig_matrix);
  pango_matrix_free (orig_matrix);
  g_object_unref (layout);
}


static void substitute_func (FcPattern *pattern, gpointer data)
{
  //if (opt_hinting != HINT_DEFAULT)
  //  {
  //    FcPatternDel (pattern, FC_HINTING);
  //    FcPatternAddBool (pattern, FC_HINTING, opt_hinting != HINT_NONE);

  //    FcPatternDel (pattern, FC_AUTOHINT);
  //    FcPatternAddBool (pattern, FC_AUTOHINT, opt_hinting == HINT_AUTO);
  //  }
}

static gpointer pangoft2_view_create(handle_t h_pango, void *p_font, u32 size)
{
  PangoFontMap *fontmap = NULL;
  gui_pango_t *p_info = NULL;
  PangoFT2FontFile font = {0};
  gchar name[8] = {"serif"};
  gchar lang[8] = {"EN"};
  gchar path[8] = {"Arial"};
  
  p_info = (gui_pango_t *)h_pango;
  MT_ASSERT(p_info != NULL);

  font.memaddr = p_font;
  font.memsize = size;
  font.name = name;
  font.lang = lang;
  font.filepath = path;

  pango_ft2_font_file_load(&font, 1);
  
  fontmap = pango_ft2_font_map_new();

  pango_ft2_font_map_set_resolution(PANGO_FT2_FONT_MAP(fontmap), p_info->dpix, p_info->dpiy);
  pango_ft2_font_map_set_default_substitute(PANGO_FT2_FONT_MAP (fontmap), substitute_func, NULL, NULL);

  return fontmap;
}

static void pangoft2_view_destroy (gpointer instance)
{
  pango_ft2_font_file_unload();

  g_object_unref (instance);
}

static PangoContext *pangoft2_view_get_context (gpointer instance)
{
  return pango_font_map_create_context(PANGO_FONT_MAP (instance));
}

static gpointer pangoft2_view_create_surface (int width, int height)
{
  FT_Bitmap *bitmap;

  bitmap = g_slice_new (FT_Bitmap);
  bitmap->width = width;
  bitmap->pitch = (bitmap->width + 3) & ~3;
  bitmap->rows = height;
  bitmap->buffer = g_malloc (bitmap->pitch * bitmap->rows);
  bitmap->num_grays = 256;
  bitmap->pixel_mode = ft_pixel_mode_grays;
  memset (bitmap->buffer, 0x00, bitmap->pitch * bitmap->rows);

  return bitmap;
}

static void
pangoft2_view_destroy_surface (gpointer surface)
{
  FT_Bitmap *bitmap = (FT_Bitmap *) surface;

  g_free (bitmap->buffer);
  g_slice_free (FT_Bitmap, bitmap);
}

static void render_callback (PangoLayout *layout, int x, int y, gpointer context, gpointer data)
{
  pango_ft2_render_layout ((FT_Bitmap *)context, layout, x, y);
}

static void pangoft2_view_render (handle_t h_pango, gpointer surface,
  PangoContext *context, int *width, int *height, int *aw, int *ah, gpointer state)
{
  do_output(h_pango, context, render_callback, NULL, surface, state, width, height, aw, ah);
}

static u32 _draw_unistr(handle_t h_pango, hdc_t hdc, rect_t *p_rc, u32 style,
  u16 x_offset, u16 y_offset, u8 line_space, u16 *p_str, rsc_fstyle_t *p_fstyle, u32 draw_style)
{
  int width = 0, height = 0;
  int aw = 0, ah = 0;
  gpointer surface = NULL;
  gui_pango_t *p_info = NULL;
  bitmap_t bmp = {0};
  FT_Bitmap *bitmap = NULL;
  void *p_buf = NULL;
  u8 bpp = 0;
  u32 i = 0;
  
  p_info = (gui_pango_t *)h_pango;
  MT_ASSERT(p_info != NULL);
  MT_ASSERT(p_info->p_text == NULL);
  MT_ASSERT(p_rc != NULL);
  MT_ASSERT(p_str != NULL);
  MT_ASSERT(p_fstyle != NULL);
  //OS_PRINTF("[_draw_unistr]Enter~~~~~~~~~~~~~~~~~\n");
  width = RECTWP(p_rc);
  height = RECTHP(p_rc);

  p_info->p_text = g_utf16_to_utf8(p_str, uni_strlen(p_str), NULL, NULL, NULL);
//OS_PRINTF("[_draw_unistr]text = %s\n", p_info->p_text);
  if(style & STL_LEFT)
  {
    p_info->opt_align = PANGO_ALIGN_LEFT;
  }
  else if(style & STL_RIGHT)
  {
    p_info->opt_align = PANGO_ALIGN_RIGHT;
  }
  else
  {
    p_info->opt_align = PANGO_ALIGN_CENTER;
  }

  p_info->opt_width = width;
  p_info->opt_height = height;
  p_info->dpix = p_fstyle->width * 5;
  p_info->dpiy = p_fstyle->height * 5;

  pango_ft2_font_map_set_resolution(
    PANGO_FT2_FONT_MAP(p_info->instance), p_info->dpix, p_info->dpiy);
  
  surface = pangoft2_view_create_surface(width, height);
//OS_PRINTF("[_draw_unistr]0x%x = pangoft2_view_create_surface\n", surface);
  pangoft2_view_render(h_pango, surface, p_info->p_context, &width, &height, &aw, &ah, NULL);
//OS_PRINTF("[_draw_unistr]width=%d, height=%d\n", width, height);
  bitmap = (FT_Bitmap *)surface;

  bpp = gdi_get_bpp(gdi_get_screen_pixel_type());
  
  p_buf = mtos_align_malloc(bitmap->pitch * bitmap->rows * bpp >> 3, 8);
  MT_ASSERT(p_buf != NULL);

  for(i = 0; i < (bitmap->pitch * bitmap->rows); i++)
  {
    if(bpp == 32)
    {
      *((u32 *)p_buf + i) = p_fstyle->color;
    }
    else if(bpp == 16)
    {
      *((u16 *)p_buf + i) = p_fstyle->color;
    }
    
  }    

  bmp.width = bitmap->width;
  bmp.height = bitmap->rows;
  bmp.is_font = TRUE;  
  bmp.pitch = bitmap->pitch * bpp >> 3;
  bmp.p_bits = p_buf;
  bmp.p_alpha = bitmap->buffer;
  bmp.color = p_fstyle->color;
  bmp.alpha_pitch = bitmap->pitch;
  bmp.p_strok_alpha = NULL;
  bmp.p_strok_char = NULL;
  bmp.bpp = bpp;
  bmp.is_font = TRUE; 
  bmp.format = gdi_get_screen_pixel_type();

  if(style & STL_TOP)
  {
    p_rc->bottom -= (height - ah) / 2;
  }
  else if(style & STL_BOTTOM)
  {
    p_rc->top += (height - ah);
  }
  else
  {
    p_rc->top += (height - ah) / 2;
    p_rc->bottom -= (height - ah) / 2;
  }  
  
  gdi_fill_bitmap(hdc, p_rc, (hbitmap_t)&bmp, 0);   

  pangoft2_view_destroy_surface(surface);

  mtos_align_free(p_buf);

  g_free(p_info->p_text);

  p_info->p_text = NULL;
  
  return 0;
}

handle_t gui_pango_init(pango_cfg_t *p_cfg)
{
  gui_pango_t *p_info = NULL;

  MT_ASSERT(p_cfg != NULL);
  
  p_info = (gui_pango_t *)mmi_alloc_buf(sizeof(gui_pango_t));
  MT_ASSERT(p_info != NULL);
  memset(p_info, 0, sizeof(gui_pango_t));

  p_info->dpix = 96;
  p_info->dpiy = 96;
  p_info->opt_align = PANGO_ALIGN_LEFT;
  p_info->opt_auto_dir = TRUE;
  p_info->opt_ellipsize = PANGO_ELLIPSIZE_NONE;
  p_info->opt_height = -1;
  p_info->opt_indent = 0;
  p_info->opt_justify = FALSE;
  p_info->opt_rotate = 0;
  p_info->opt_rtl = FALSE;
  p_info->opt_single_par = FALSE;
  p_info->opt_width = -1;
  p_info->opt_wrap = PANGO_WRAP_WORD_CHAR;
  p_info->opt_margin_b = 0;
  p_info->opt_margin_l = 0;
  p_info->opt_margin_r = 0;
  p_info->opt_margin_t = 0;

  p_info->draw_unistr = _draw_unistr;

  glib_init();
  gtype_init();
  //glib_set_log_level(G_LOG_LEVEL_DEBUG);
  //OS_PRINTF("glib_init\n");

  p_info->instance = pangoft2_view_create((handle_t)p_info, p_cfg->p_font, p_cfg->font_size);

  p_info->p_context = pangoft2_view_get_context(p_info->instance);

  
  return (handle_t)p_info; 
}

void gui_pango_release(handle_t h_pango)
{
  gui_pango_t *p_info = NULL;
  
  p_info = (gui_pango_t *)h_pango;
  MT_ASSERT(p_info != NULL);

  pangoft2_view_destroy(p_info->instance);
  
  mmi_free_buf((void *)h_pango);

  return;
}

u32 pango_draw_unistr(handle_t h_pango, hdc_t hdc, rect_t *p_rc, u32 style, u16 x_offset, u16 y_offset,
  u8 line_space, u16 *p_str, rsc_fstyle_t *p_fstyle, u32 draw_style)
{
  gui_pango_t *p_info = NULL;
  
  p_info = (gui_pango_t *)h_pango;
  MT_ASSERT(p_info != NULL);

  if(p_info->draw_unistr != NULL)
  {
    return p_info->draw_unistr(h_pango, hdc, p_rc, style, x_offset, y_offset, line_space, p_str, p_fstyle, draw_style);
  }

  return 0;
}
