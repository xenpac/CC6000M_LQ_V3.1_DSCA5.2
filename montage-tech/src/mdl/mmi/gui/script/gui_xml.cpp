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
#include "lib_util.h"
#include "common.h"
#include "osd.h"
#include "gpe.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "flinger.h"
#include "vfs.h"

#include "data_manager.h"
#include "lzma.h"
#include "lzmaif.h"

#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"

#include "ctrl_container.h"
#include "ctrl_textfield.h"
#include "ctrl_scrollbar.h"
#include "ctrl_progressbar.h"
#include "ctrl_matrixbox.h"
#include "ctrl_list.h"
#include "ctrl_combobox.h"
#include "ctrl_bitmap.h"
#include "ctrl_editbox.h"
#include "ctrl_common.h"
#include "ctrl_string.h"
#include "ctrl_numbox.h"
#include "ctrl_ipbox.h"
#include "ctrl_setbox.h"
#include "ctrl_timebox.h"

#include "framework.h"
#include "gui_xml.h"

#include "tinystr.h"
#include "tinyxml.h"

#define BLOCK_XML_SKIN 	0xAC

#define BLOCK_XML_LOGO	0xAD

#ifdef WIN32
#define STR_CASE_CMP	(stricmp)
#else
#define STR_CASE_CMP	(strcasecmp)
#endif

//#define XML_DUMP_ENABLE


typedef BOOL (*logo_get_t)(u16 logo_id, u32 *p_addr, u32 *p_size);

typedef rsc_fstyle_t * (*fstyle_get_t)(u32 idx);

typedef rsc_rstyle_t * (*rstyle_get_t)(u32 idx);

typedef BOOL (*img_get_t)(u16 bmp_id, rsc_bitmap_t *p_hdr_bmp, u8 *p_data, u32 *p_bmp_hdr_addr);

typedef struct
{
	u32 old_xstyle;
	u32 new_xstyle;
}xml_xstyle_set_t;

typedef struct
{
	u32 xstyle_set_cnt;
}xml_xstyle_t;

typedef struct
{
	u32 id;
  u32 offset;
}xml_bmp_t;

typedef struct
{
  u32 menu_id;
	u32 xml_offset;
	u32 xml_size;
}xml_menu_t;

typedef struct
{
	u32 menu_cnt;
	u32 menu_offset;
	u32 fstyle_cnt;
	u32 fstyle_offset;
	u32 rstyle_cnt;
	u32 rstyle_offset;
	u32 bmap_cnt;
	u32 bmap_offset;
}skin_t;

typedef struct
{
  void *p_data;
  skin_t *p_skin;
  rsc_fstyle_t *p_ftab;
  rsc_rstyle_t *p_rtab;
  u8 *p_bmp_data;
  u8 *p_menu_data;
  fstyle_get_t get_fstyle;
  rstyle_get_t get_rstyle;
  img_get_t get_img;
}xml_skin_t;

typedef struct
{
	u32 indicate;
	u32 logo_cnt;
	u32 logo_offset;
}logo_t;

typedef struct
{
	u32 id;
	u32 offset;
	u32 size;
}logo_head_t;

typedef struct
{
  logo_t logo;
  u32 logo_addr;
  u32 logo_size;
  logo_get_t logo_get;
}xml_logo_t;

static rsc_fstyle_t *_get_fstyle(u32 idx)
{
  u32 index = 0;
	xml_skin_t *p_info = NULL;

  p_info = (xml_skin_t *)class_get_handle_by_id(XML_SKIN_CLASS_ID);

  mmi_assert(p_info != NULL);
  mmi_assert(p_info->p_data != NULL);
  mmi_assert(p_info->p_skin != NULL);
  mmi_assert(idx >= XML_FSTYLE_START);

  index = idx - XML_FSTYLE_START;
  
  mmi_assert(index < p_info->p_skin->fstyle_cnt);

  return (p_info->p_ftab + index);
}

static rsc_rstyle_t *_get_rstyle(u32 idx)
{
  u32 index = 0;
  xml_skin_t *p_info = NULL;

  p_info = (xml_skin_t *)class_get_handle_by_id(XML_SKIN_CLASS_ID);
  
  mmi_assert(p_info != NULL);  
  mmi_assert(p_info->p_data != NULL);
  mmi_assert(p_info->p_skin != NULL);
  mmi_assert(idx >= XML_FSTYLE_START);

  index = idx - XML_RSTYLE_START;
  
  mmi_assert(index < p_info->p_skin->rstyle_cnt);

  return (p_info->p_rtab + index);
}

static BOOL _get_img(u16 bmp_id, rsc_bitmap_t *p_hdr_bmp, u8 *p_data, u32 *p_bmp_hdr_addr)
{
  xml_bmp_t *p_tmp = NULL;
  u8 *p_bmp = NULL;
  u16 bmap_id = 0;
  xml_skin_t *p_info = NULL;

  p_info = (xml_skin_t *)class_get_handle_by_id(XML_SKIN_CLASS_ID);
  mmi_assert(p_info != NULL);  
  mmi_assert(p_info->p_data != NULL);
  mmi_assert(p_info->p_skin != NULL);
  mmi_assert(bmp_id >= XML_BMAP_START);

  bmap_id = bmp_id - XML_BMAP_START;
  
  mmi_assert(bmap_id < p_info->p_skin->bmap_cnt);  

  p_tmp = (xml_bmp_t *)(p_info->p_bmp_data + bmap_id * sizeof(xml_bmp_t));


	mmi_assert(p_tmp->id == bmp_id);
	
  p_bmp = (u8 *)p_info->p_data + p_tmp->offset;

  memcpy(p_hdr_bmp, p_bmp, sizeof(rsc_bitmap_t));

  *p_bmp_hdr_addr = (u32)p_bmp;

  mmi_assert(p_hdr_bmp->head.id == bmp_id);

  memcpy(p_data, p_bmp + sizeof(rsc_bitmap_t), p_hdr_bmp->head.org_size);
  
  return TRUE;
}

BOOL xml_skin_init(u32 mem_addr, u32 mem_size)
{
  u32 addr = 0, zip_size = 0, unzip_size = 0;
  void *p_dm_handle = NULL;
  void *p_zip_buf = NULL, *p_fake_buf = NULL;
  xml_xstyle_t *p_xstyle_head = NULL;
  xml_xstyle_set_t *p_xstyle_set = NULL;
  rsc_fstyle_t *p_new_fstyle = NULL;
  rsc_rstyle_t *p_new_rstyle = NULL;
  u32 i = 0;
  u8 ret = 0;
  xml_skin_t *p_info = NULL;

  p_info = (xml_skin_t *)mmi_alloc_buf(sizeof(xml_skin_t));
  mmi_assert(p_info != NULL);

  memset((void *)p_info, 0, sizeof(xml_skin_t));
  class_register(XML_SKIN_CLASS_ID, (class_handle_t)p_info);

  p_dm_handle = (void *)class_get_handle_by_id(DM_CLASS_ID);
  mmi_assert(p_dm_handle != NULL);
  
#if 1
  addr = dm_get_block_addr(p_dm_handle, BLOCK_XML_SKIN);
  zip_size = dm_get_block_size(p_dm_handle, BLOCK_XML_SKIN);

  if((addr == 0) || (zip_size == 0))
  {
    return FALSE;
  }

  p_zip_buf = (void *)mtos_malloc(zip_size);
  mmi_assert(p_zip_buf != NULL);

  p_fake_buf = (void *)mtos_malloc(100 * KBYTES);
  mmi_assert(p_fake_buf != NULL);

  p_info->p_data = (void *)mem_addr;
  unzip_size = mem_size;
  mmi_assert(p_info->p_data != NULL);

  dm_read(p_dm_handle, BLOCK_XML_SKIN, 0, 0, zip_size, (u8 *)p_zip_buf);

  init_fake_mem_lzma(p_fake_buf, 100 * KBYTES);

  ret = lzma_decompress((void *)p_info->p_data, &unzip_size, p_zip_buf, zip_size);
  mmi_assert(ret == 0);
  
  mtos_free(p_zip_buf);
  mtos_free(p_fake_buf);
#else
  p_info->p_data = (void *)malloc(10 * MBYTES);
  mmi_assert(p_info->p_data != NULL);	

  {
		FILE *fp = NULL;
		u32 length = 0;

		fp = fopen("skin.bin", "rb");

    fseek(fp, 0L, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
		fread(p_info->p_data, 1, length, fp);

		fclose(fp);
  }
#endif
  p_info->p_skin = (skin_t *)p_info->p_data;

	p_info->p_menu_data = (u8 *)p_info->p_data + p_info->p_skin->menu_offset;
	p_info->p_bmp_data = (u8 *)p_info->p_data + p_info->p_skin->bmap_offset;

	//parse fstyle head, try to reset some fstyles in maincode.
	p_xstyle_head = (xml_xstyle_t *)((u8 *)p_info->p_data + p_info->p_skin->fstyle_offset);

	p_xstyle_set = (xml_xstyle_set_t *)((u8 *)p_info->p_data +
		p_info->p_skin->fstyle_offset + sizeof(p_xstyle_head));

	p_info->p_ftab = (rsc_fstyle_t *)((u8 *)p_info->p_data + p_info->p_skin->fstyle_offset +
		sizeof(xml_xstyle_t) + p_xstyle_head->xstyle_set_cnt * sizeof(xml_xstyle_set_t));
		
	for(i = 0; i < p_xstyle_head->xstyle_set_cnt; i++)
	{
		mmi_assert(p_xstyle_set->new_xstyle < p_info->p_skin->fstyle_cnt);

		p_new_fstyle = p_info->p_ftab + p_xstyle_set->new_xstyle;
		
		rsc_set_fstyle(gui_get_rsc_handle(), p_xstyle_set->old_xstyle, p_new_fstyle);

		p_xstyle_set++;
	}

	//parse rstyle head, try to reset some rstyles in maincode.
	p_xstyle_head = (xml_xstyle_t *)((u8 *)p_info->p_data + p_info->p_skin->rstyle_offset);

	p_xstyle_set = (xml_xstyle_set_t *)((u8 *)p_info->p_data +
		p_info->p_skin->rstyle_offset + sizeof(p_xstyle_head));

	p_info->p_rtab = (rsc_rstyle_t *)((u8 *)p_info->p_data + p_info->p_skin->rstyle_offset +
		sizeof(xml_xstyle_t) + p_xstyle_head->xstyle_set_cnt * sizeof(xml_xstyle_set_t));
		
	for(i = 0; i < p_xstyle_head->xstyle_set_cnt; i++)
	{
		mmi_assert(p_xstyle_set->new_xstyle < p_info->p_skin->rstyle_cnt);

		p_new_rstyle = p_info->p_rtab + p_xstyle_set->new_xstyle;
		
		rsc_set_rstyle(gui_get_rsc_handle(), p_xstyle_set->old_xstyle, p_new_rstyle);

		p_xstyle_set++;
	}

	p_info->get_fstyle = _get_fstyle;
	p_info->get_rstyle = _get_rstyle;
	p_info->get_img = _get_img;

  return TRUE;
}

void xml_skin_release(void)
{
  xml_skin_t *p_info = NULL;

  p_info = (xml_skin_t *)class_get_handle_by_id(XML_SKIN_CLASS_ID);
  mmi_assert(p_info != NULL);
  
  if(p_info->p_data != NULL)
  {
    mtos_free(p_info->p_data);

    p_info->p_data = NULL;

    p_info->p_skin = NULL;
  }

  p_info->get_fstyle = NULL;
  p_info->get_rstyle = NULL;
  p_info->get_img = NULL;
}

rsc_fstyle_t *xml_get_fstyle(u32 idx)
{
	xml_skin_t *p_info = NULL;

  p_info = (xml_skin_t *)class_get_handle_by_id(XML_SKIN_CLASS_ID);
  mmi_assert(p_info != NULL);

	if(p_info->get_fstyle != NULL)
	{
		return p_info->get_fstyle(idx);
	}

  return NULL;
}

rsc_rstyle_t *xml_get_rstyle(u32 idx)
{
  xml_skin_t *p_info = NULL;

  p_info = (xml_skin_t *)class_get_handle_by_id(XML_SKIN_CLASS_ID);
  mmi_assert(p_info != NULL);  

  if(p_info->get_rstyle != NULL)
  {
		return p_info->get_rstyle(idx);
  }

  return NULL;
}

BOOL xml_get_bmp(u16 bmp_id, rsc_bitmap_t *p_hdr_bmp, u8 *p_data, u32 *p_bmp_hdr_addr)
{
  xml_skin_t *p_info = NULL;

  p_info = (xml_skin_t *)class_get_handle_by_id(XML_SKIN_CLASS_ID);
  mmi_assert(p_info != NULL);  

	if(p_info->get_img)
	{
		return p_info->get_img(bmp_id, p_hdr_bmp, p_data, p_bmp_hdr_addr);
	}
  
  return FALSE;
}

static control_t *ctrl_base_adjust(TiXmlElement *p_element, control_t *p_parent)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  u16 id = 0;
  rect_t frame = {0};
  obj_attr_t attr = OBJ_ATTR_ACTIVE;
  obj_sts_t sts = OBJ_STS_SHOW;
  u8 style = 0;
	ctrl_rstyle_t rstyle = {0};
  u8 type[CTRL_CLASS_NAME_LEN + 1] = {0};
  control_t *p_ctrl = NULL;
  u16 ctrl_bg = 0;
  u16 mleft = 0, mright = 0, mtop = 0, mbottom = 0;

  //id
  p_tmp = p_element->Attribute("id");
  mmi_assert(p_tmp != NULL);
  id = atoi(p_tmp);

	if(id == 0)
	{
		return NULL;
	}
	
  p_ctrl = ctrl_get_child_by_id(p_parent, id);
  mmi_assert(p_ctrl != NULL);
  
  //type
  p_tmp = p_element->Attribute("type");
  mmi_assert(p_tmp != NULL);
  memcpy(type, p_tmp, CTRL_CLASS_NAME_LEN);

  mmi_assert(STR_CASE_CMP((char *)type, (char *)ctrl_get_type(p_ctrl)) == 0);

	ctrl_get_frame(p_ctrl, &frame);
	attr = ctrl_get_attr(p_ctrl);
	sts = ctrl_get_sts(p_ctrl);
	style = ctrl_get_style(p_ctrl);
	ctrl_get_rstyle(p_ctrl, &rstyle);  
	ctrl_bg = ctrl_get_bg(p_ctrl);

  //bg
  p_tmp = p_element->Attribute("bg");
  if(p_tmp != NULL)
  {
	  ctrl_bg = atoi(p_tmp);
	}
  
  //x
  p_tmp = p_element->Attribute("x");
  if(p_tmp != NULL)
  {
	  frame.left = atoi(p_tmp);
	}
	
  //y
  p_tmp = p_element->Attribute("y");
  if(p_tmp != NULL)
  {
	  frame.top = atoi(p_tmp);
  }
  
  //w
  p_tmp = p_element->Attribute("w");
  if(p_tmp != NULL)
  {
  	frame.right = (frame.left + atoi(p_tmp));
  }
  
  //h
  p_tmp = p_element->Attribute("h");
  if(p_tmp != NULL)
  {
  	frame.bottom = (frame.top + atoi(p_tmp));
	}
	
  //attr
  p_tmp = p_element->Attribute("attr");
  if(p_tmp != NULL)
  {
    attr = (obj_attr_t)atoi(p_tmp);
  }  

  //sts
  p_tmp = p_element->Attribute("sts");
  if(p_tmp != NULL)
  {
    sts = (obj_sts_t)atoi(p_tmp);
  }  

  //style
  p_tmp = p_element->Attribute("style");
  if(p_tmp != NULL)
  {
    style = atoi(p_tmp);
  }  

  //mleft
  p_tmp = p_element->Attribute("mleft");
  if(p_tmp != NULL)
  {
	  mleft = atoi(p_tmp);
	}

  //mtop
  p_tmp = p_element->Attribute("mtop");
  if(p_tmp != NULL)
  {
	  mtop = atoi(p_tmp);
  }
  
  //mright
  p_tmp = p_element->Attribute("mright");
  if(p_tmp != NULL)
  {
  	mright = atoi(p_tmp);
  }
  
  //mbottom
  p_tmp = p_element->Attribute("mbottom");
  if(p_tmp != NULL)
  {
  	mbottom = atoi(p_tmp);
	}

  //rect-style
  p_tmp = p_element->Attribute("r_active");
  if(p_tmp != NULL)
  {
    rstyle.show_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }    
  
  p_tmp = p_element->Attribute("r_hlight");
  if(p_tmp != NULL)
  {
    rstyle.hl_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   
  
  p_tmp = p_element->Attribute("r_inactive");
  if(p_tmp != NULL)
  {
    rstyle.gray_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }    
  
  //try to adjust surface.
  ctrl_set_bg(p_ctrl, ctrl_bg);
  ctrl_set_frame(p_ctrl, &frame);
  ctrl_set_rstyle(p_ctrl, rstyle.show_idx, rstyle.hl_idx, rstyle.gray_idx);
  ctrl_set_attr(p_ctrl, attr);
  ctrl_set_sts_ex(p_ctrl, sts);
  ctrl_set_style(p_ctrl, style);
  ctrl_set_mrect(p_ctrl, mleft, mtop, mright, mbottom);
  ctrl_empty_invrgn(p_ctrl);
  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  return p_ctrl;
}

static void bmap_priv_adjust(TiXmlElement *p_sub, control_t *p_bmap)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  u16 left = 0, top = 0, bmp_id = 0;
  u32 align = 0;

  mmi_assert(p_sub != NULL);
  mmi_assert(p_bmap != NULL);

  align = bmap_get_align_type(p_bmap);
  bmap_get_bmap_offset(p_bmap, &left, &top);
  bmp_id = bmap_get_content(p_bmap);

  //align type.
  p_tmp = p_sub->Attribute("align");
  if(p_tmp != NULL)
  {
    align = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }
  
  //left-top offset.
  p_tmp = p_sub->Attribute("left");
  if(p_tmp != NULL)
  {
    left = atoi(p_tmp); 
  }
  
  p_tmp = p_sub->Attribute("top");
  if(p_tmp != NULL)
  {
    top = atoi(p_tmp); 
  }

  p_tmp = p_sub->Attribute("bmap_id");
  if(p_tmp != NULL)
  {
    bmp_id = atoi(p_tmp); 
  }

  bmap_set_align_type(p_bmap, align);
  bmap_set_bmap_offset(p_bmap, left, top);

  if(!bmap_get_content_type(p_bmap))
  {
		bmap_set_content_by_id(p_bmap, bmp_id);
  }

  return;
}

static void cbox_priv_adjust(TiXmlElement *p_sub, control_t *p_cbox)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  u32 f_active = 0, f_hlight = 0, f_inactive = 0, align = 0;
  BOOL cycle = FALSE;

  mmi_assert(p_sub != NULL);
  mmi_assert(p_cbox != NULL);
  
  align = cbox_get_align_style(p_cbox);
	cbox_get_fstyle(p_cbox, &f_active, &f_hlight, &f_inactive);
	cycle = cbox_is_cycle_mode(p_cbox);
	
  //align
  p_tmp = p_sub->Attribute("align");
  if(p_tmp != NULL)
  {
    align = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  
  
  //f_active, f_hlight, f_inactive.
  p_tmp = p_sub->Attribute("f_active");
  if(p_tmp != NULL)
  {
    f_active = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }

  p_tmp = p_sub->Attribute("f_hlight");
  if(p_tmp != NULL)
  {
    f_hlight = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("f_inactive");
  if(p_tmp != NULL)
  {
    f_inactive = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  //cycle mode, 0 : disable, 1 : enable.
  p_tmp = p_sub->Attribute("cyc");
  if(p_tmp != NULL)
  {
    cycle = (BOOL)atoi(p_tmp);
  }

  cbox_set_align_style(p_cbox, align);
  cbox_set_font_style(p_cbox, f_active, f_hlight, f_inactive);
  cbox_enable_cycle_mode(p_cbox, cycle);

  return;
}

static void cont_priv_adjust(TiXmlElement *p_sub, control_t *p_cont)
{
  mmi_assert(p_sub != NULL);
  mmi_assert(p_cont != NULL);
  
  return;
}

static void ebox_priv_adjust(TiXmlElement *p_sub, control_t *p_ebox)
{
	const char *p_tmp = NULL;
	char *p_stop = NULL;
  u32 f_active = 0, f_hlight = 0, f_inactive = 0;
  u16 left= 0, top = 0, maxtext = 0;
  u32 align = 0;

  mmi_assert(p_sub != NULL);
  mmi_assert(p_ebox != NULL);

  ebox_get_fstyle(p_ebox, &f_active, &f_hlight, &f_inactive);
  ebox_get_align_type(p_ebox);
  ebox_get_offset(p_ebox, &left, &top);
  maxtext = ebox_get_maxtext(p_ebox);

  //align
  p_tmp = p_sub->Attribute("align");
  if(p_tmp != NULL)
  {
    align = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }    
  
  //f_active, f_hlight, f_inactive.
  p_tmp = p_sub->Attribute("f_active");
  if(p_tmp != NULL)
  {
    f_active = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }

  p_tmp = p_sub->Attribute("f_hlight");
  if(p_tmp != NULL)
  {
    f_hlight = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("f_inactive");
  if(p_tmp != NULL)
  {
    f_inactive = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  //str_left, str_top
  p_tmp = p_sub->Attribute("left");
  if(p_tmp != NULL)
  {
    left = atoi(p_tmp);
  }    

  p_tmp = p_sub->Attribute("top");
  if(p_tmp != NULL)
  {
    top = atoi(p_tmp);
  }  

  //max text.
  p_tmp = p_sub->Attribute("maxtext");
  if(p_tmp != NULL)
  {
    maxtext = atoi(p_tmp);
  }  

  ebox_set_font_style(p_ebox, f_active, f_hlight, f_inactive);
  ebox_set_align_type(p_ebox, align);
  ebox_set_offset(p_ebox, left, top);
  ebox_set_maxtext(p_ebox, maxtext);
}

static void ipbox_priv_adjust(TiXmlElement *p_sub, control_t *p_ipbox)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  u32 align = 0;
  u32 f_active = 0, f_hlight = 0, f_inactive = 0;
  u16 left = 0, top = 0;
  u8 maxwidth = 0;

  mmi_assert(p_sub != NULL);
  mmi_assert(p_ipbox != NULL);

	align = ipbox_get_align_type(p_ipbox);
  ipbox_get_fstyle(p_ipbox, &f_active, &f_hlight, &f_inactive);
  ipbox_get_offset(p_ipbox, &left, &top);
  maxwidth = ipbox_get_max_num_width(p_ipbox);

  p_tmp = p_sub->Attribute("align");
  if(p_tmp != NULL)
  {
    align = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("f_active");
  if(p_tmp != NULL)
  {
    f_active = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }       

  p_tmp = p_sub->Attribute("f_hlight");
  if(p_tmp != NULL)
  {
    f_hlight = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("f_inactive");
  if(p_tmp != NULL)
  {
    f_inactive = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("left");
  if(p_tmp != NULL)
  {
    left = atoi(p_tmp);
  }     

  p_tmp = p_sub->Attribute("top");
  if(p_tmp != NULL)
  {
    top = atoi(p_tmp);
  }     

  p_tmp = p_sub->Attribute("maxwidth");
  if(p_tmp != NULL)
  {
    maxwidth = atoi(p_tmp);
  }  
  
  ipbox_set_align_type(p_ipbox, align);
  ipbox_set_font_style(p_ipbox, f_active, f_hlight, f_inactive);
  ipbox_set_max_num_width(p_ipbox, maxwidth);
  ipbox_set_offset(p_ipbox, left, top);
}

static void list_priv_adjust(TiXmlElement *p_sub, control_t *p_list)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  u16 page = 0;
  BOOL is_cyc = FALSE, enable_page = FALSE, is_hori = FALSE;
  u8 columns = 1, interval = 0;
  list_xstyle_t rstyle = {0};
  TiXmlElement *p_data = NULL;
  u8 idx = 0, field_num = 0;
  u32 field_attr = 0;
  u16 field_left = 0, field_top = 0, field_width = 0, field_height = 0;
  list_xstyle_t xfstyle = {0}, xrstyle = {0};

  mmi_assert(p_sub != NULL);
  mmi_assert(p_list != NULL);

	columns = list_get_columns(p_list);
	is_hori = list_is_hori(p_list);
	interval = list_get_item_interval(p_list);
	is_cyc = list_is_cycle_mode(p_list);
	enable_page = list_is_page_mode(p_list);
  page = list_get_page(p_list);
  list_get_item_rstyle(p_list, &rstyle);
  
  p_tmp = p_sub->Attribute("columns");
  if(p_tmp != NULL)
  {
    columns = atoi(p_tmp);
  }      

  p_tmp = p_sub->Attribute("is_hori");
  if(p_tmp != NULL)
  {
    is_hori = (BOOL)atoi(p_tmp);
  }      

  p_tmp = p_sub->Attribute("interval");
  if(p_tmp != NULL)
  {
    interval = atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("is_cycle");
  if(p_tmp != NULL)
  {
    is_cyc = (BOOL)atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("enable_page");
  if(p_tmp != NULL)
  {
    enable_page = (BOOL)atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("page");
  if(p_tmp != NULL)
  {
    page = atoi(p_tmp);
  }    

  p_tmp = p_sub->Attribute("r_item_inactive");
  if(p_tmp != NULL)
  {
    rstyle.g_xstyle= strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("r_item_active");
  if(p_tmp != NULL)
  {
    rstyle.n_xstyle = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  } 

  p_tmp = p_sub->Attribute("r_item_hlight");
  if(p_tmp != NULL)
  {
    rstyle.f_xstyle = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  } 

  p_tmp = p_sub->Attribute("r_item_select");
  if(p_tmp != NULL)
  {
    rstyle.s_xstyle = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  } 

  p_tmp = p_sub->Attribute("r_item_select_hlight");
  if(p_tmp != NULL)
  {
    rstyle.sf_xstyle = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   

  field_num = list_get_field_num(p_list);

  p_data = (TiXmlElement *)p_sub->FirstChild("field_data");
  while(p_data != NULL)
  {
	  p_tmp = p_data->Attribute("idx");
	  if(p_tmp != NULL)
	  {
	    idx = atoi(p_tmp);

	    if(idx < field_num)
	    {
				list_get_field_attr(p_list, idx, &field_attr,
					&field_left, &field_top, &field_width, &field_height,
					&xfstyle, &xrstyle);
					
				p_tmp = p_data->Attribute("left");
			  if(p_tmp != NULL)
			  {
			    field_left = atoi(p_tmp);
			  }  	

				p_tmp = p_data->Attribute("top");
				if(p_tmp != NULL)
				{
				  field_top = atoi(p_tmp);
				}  	

				p_tmp = p_data->Attribute("width");
				if(p_tmp != NULL)
				{
				  field_width = atoi(p_tmp);
				}  	

				p_tmp = p_data->Attribute("height");
				if(p_tmp != NULL)
				{
				  field_height = atoi(p_tmp);
				} 

				p_tmp = p_data->Attribute("g_fstyle");
				if(p_tmp != NULL)
				{
				  xfstyle.g_xstyle = strtoul(p_tmp, &p_stop, 16);
				}  					

				p_tmp = p_data->Attribute("n_fstyle");
				if(p_tmp != NULL)
				{
				  xfstyle.n_xstyle = strtoul(p_tmp, &p_stop, 16);
				}  	

				p_tmp = p_data->Attribute("f_fstyle");
				if(p_tmp != NULL)
				{
				  xfstyle.f_xstyle = strtoul(p_tmp, &p_stop, 16);
				}  	

				p_tmp = p_data->Attribute("s_fstyle");
				if(p_tmp != NULL)
				{
				  xfstyle.s_xstyle = strtoul(p_tmp, &p_stop, 16);
				}  

				p_tmp = p_data->Attribute("sf_fstyle");
				if(p_tmp != NULL)
				{
				  xfstyle.sf_xstyle = strtoul(p_tmp, &p_stop, 16);
				}  		

				p_tmp = p_data->Attribute("g_rstyle");
				if(p_tmp != NULL)
				{
				  xrstyle.g_xstyle = strtoul(p_tmp, &p_stop, 16);
				}  					

				p_tmp = p_data->Attribute("n_rstyle");
				if(p_tmp != NULL)
				{
				  xrstyle.n_xstyle = strtoul(p_tmp, &p_stop, 16);
				}  	

				p_tmp = p_data->Attribute("f_rstyle");
				if(p_tmp != NULL)
				{
				  xrstyle.f_xstyle = strtoul(p_tmp, &p_stop, 16);
				}  	

				p_tmp = p_data->Attribute("s_rstyle");
				if(p_tmp != NULL)
				{
				  xrstyle.s_xstyle = strtoul(p_tmp, &p_stop, 16);
				}  

				p_tmp = p_data->Attribute("sf_rstyle");
				if(p_tmp != NULL)
				{
				  xrstyle.sf_xstyle = strtoul(p_tmp, &p_stop, 16);
				} 	

				list_set_field_attr2(p_list, idx, field_attr,
					field_width, field_height, field_left, field_top);

				list_set_field_font_style(p_list, idx, &xfstyle);

				list_set_field_rect_style(p_list, idx, &xrstyle);
	    }
	  }   

	  p_data = p_data->NextSiblingElement();
  }  
  
  list_set_columns(p_list, columns, is_hori);
  list_set_item_interval(p_list, interval);
  list_set_item_rstyle(p_list, &rstyle);
  list_set_page(p_list, page);
  list_enable_cycle_mode(p_list, is_cyc);
  list_enable_page_mode(p_list, enable_page);
}

static void mbox_priv_adjust(TiXmlElement *p_sub, control_t *p_mbox)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  u32 str_align = 0, icon_align = 0;
  u32 f_active = 0, f_hlight = 0, f_inactive = 0;
	ctrl_rstyle_t rstyle = {0};
  u8 str_left = 0, str_top = 0, icon_left = 0, icon_top = 0;
  u8 h_interval = 0, v_interval = 0;
  u16 total = 0; 
  u16 col = 0, row = 0, win_col = 0, win_row = 0;
	TiXmlElement *p_data = NULL;
	u16 item_idx = 0, hlight_bmp = 0, normal_bmp = 0;
	
  mmi_assert(p_sub != NULL);
  mmi_assert(p_mbox != NULL);

	str_align = mbox_get_string_align_type(p_mbox);
	mbox_get_string_fstyle(p_mbox, &f_active, &f_hlight, &f_inactive);
	mbox_get_string_offset(p_mbox, &str_left, &str_top);

	icon_align = mbox_get_icon_align_type(p_mbox);
	mbox_get_icon_offset(p_mbox, &icon_left, &icon_top);
	
	mbox_get_item_interval(p_mbox, &h_interval, &v_interval);
	mbox_get_item_rstyle(p_mbox, &rstyle);

	mbox_get_count(p_mbox, &total, &col, &row);

	mbox_get_win(p_mbox, &win_col, &win_row);
	
  p_tmp = p_sub->Attribute("str_left");
  if(p_tmp != NULL)
  {
    str_left = atoi(p_tmp);
  }     

  p_tmp = p_sub->Attribute("str_top");
  if(p_tmp != NULL)
  {
    str_top = atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("icon_left");
  if(p_tmp != NULL)
  {
    icon_left = atoi(p_tmp);
  }     

  p_tmp = p_sub->Attribute("icon_top");
  if(p_tmp != NULL)
  {
    icon_top = atoi(p_tmp);
  }    

  p_tmp = p_sub->Attribute("f_active");
  if(p_tmp != NULL)
  {
    f_active = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }       

  p_tmp = p_sub->Attribute("f_hlight");
  if(p_tmp != NULL)
  {
    f_hlight = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("f_inactive");
  if(p_tmp != NULL)
  {
    f_inactive = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("str_align");
  if(p_tmp != NULL)
  {
    str_align = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("icon_align");
  if(p_tmp != NULL)
  {
    icon_align = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }     

  p_tmp = p_sub->Attribute("r_item_active");
  if(p_tmp != NULL)
  {
    rstyle.show_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }       

  p_tmp = p_sub->Attribute("r_item_hlight");
  if(p_tmp != NULL)
  {
    rstyle.hl_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("r_item_inactive");
  if(p_tmp != NULL)
  {
    rstyle.gray_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("h_interval");
  if(p_tmp != NULL)
  {
    h_interval = atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("v_interval");
  if(p_tmp != NULL)
  {
    v_interval = atoi(p_tmp);
  }   


  p_tmp = p_sub->Attribute("col");
  if(p_tmp != NULL)
  {
    col = atoi(p_tmp);
  }  


  p_tmp = p_sub->Attribute("row");
  if(p_tmp != NULL)
  {
    row = atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("win_col");
  if(p_tmp != NULL)
  {
    win_col = atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("win_row");
  if(p_tmp != NULL)
  {
    win_row = atoi(p_tmp);
  }   

  p_data = (TiXmlElement *)p_sub->FirstChild("mbox_data");
  while(p_data != NULL)
  {
	  p_tmp = p_data->Attribute("item_idx");
	  if(p_tmp != NULL)
	  {
	    item_idx = atoi(p_tmp);

	    if(item_idx < total)
	    {
	    	mbox_get_icon(p_mbox, item_idx, &hlight_bmp, &normal_bmp);
			p_tmp = p_data->Attribute("hlight_bmp");
			if(p_tmp != NULL)
			{
			  if((p_tmp[0] == '0')&&((p_tmp[1] == 'x')||(p_tmp[1] == 'X')))//0x begin
              {   
			    hlight_bmp = strtoul(p_tmp, &p_stop, 16);
              }
              else
              {   
			    hlight_bmp = atoi(p_tmp);
              }          
			}  	

			p_tmp = p_data->Attribute("normal_bmp");
			if(p_tmp != NULL)
			{
			  if((p_tmp[0] == '0')&&((p_tmp[1] == 'x')||(p_tmp[1] == 'X')))//0x begin
              {   
			    normal_bmp = strtoul(p_tmp, &p_stop, 16);
              }
              else
              {   
			    normal_bmp = atoi(p_tmp);
              }          
			}  	

			mbox_set_content_by_icon(p_mbox, item_idx, hlight_bmp, normal_bmp);
	    }
	  }   

	  p_data = p_data->NextSiblingElement();
  }
  

	//to make sure total items not changed.
  mbox_set_size(p_mbox, col, row, win_col, win_row);
  mbox_set_icon_align_type(p_mbox, icon_align);
  mbox_set_icon_offset(p_mbox, icon_left, icon_top);
  mbox_set_item_interval(p_mbox, h_interval, v_interval);
  mbox_set_item_rstyle(p_mbox, rstyle.hl_idx, rstyle.show_idx, rstyle.gray_idx);
  mbox_set_string_align_type(p_mbox, str_align);
  mbox_set_string_offset(p_mbox, str_left, str_top);
  mbox_set_string_fstyle(p_mbox, f_hlight, f_active, f_inactive);
}

static void nbox_priv_adjust(TiXmlElement *p_sub, control_t *p_nbox)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  u32 align = 0;
  u32 f_active = 0, f_hlight = 0, f_inactive = 0;
  u16 left = 0, top = 0;
  BOOL is_underline = FALSE;

  mmi_assert(p_sub != NULL);
  mmi_assert(p_nbox != NULL);

  align = nbox_get_align_type(p_nbox);
  nbox_get_offset(p_nbox, &left, &top);
  nbox_get_fstyle(p_nbox, &f_active, &f_hlight, &f_inactive);
  is_underline = nbox_is_underline(p_nbox);
  
  p_tmp = p_sub->Attribute("align");
  if(p_tmp != NULL)
  {
    align = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }

  p_tmp = p_sub->Attribute("left");
  if(p_tmp != NULL)
  {
    left = atoi(p_tmp);
  }     

  p_tmp = p_sub->Attribute("top");
  if(p_tmp != NULL)
  {
    top = atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("f_active");
  if(p_tmp != NULL)
  {
    f_active = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }       

  p_tmp = p_sub->Attribute("f_hlight");
  if(p_tmp != NULL)
  {
    f_hlight = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("f_inactive");
  if(p_tmp != NULL)
  {
    f_inactive = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   
  
  p_tmp = p_sub->Attribute("is_underline");
  if(p_tmp != NULL)
  {
    is_underline = (BOOL)atoi(p_tmp);
  }   
  
  nbox_set_align_type(p_nbox, align);
  nbox_set_font_style(p_nbox, f_active, f_hlight, f_inactive);
  nbox_set_highlight_type(p_nbox, is_underline);
  nbox_set_offset(p_nbox, left, top);
}

static void pbar_priv_adjust(TiXmlElement *p_sub, control_t *p_pbar)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  u8 interval = 0;
  u32 r_min = 0, r_max = 0, r_curn = 0;
  BOOL is_hori = TRUE, is_continue = FALSE;

  mmi_assert(p_sub != NULL);
  mmi_assert(p_pbar != NULL);

  interval = pbar_get_interval(p_pbar);
	pbar_get_rstyle(p_pbar, &r_min, &r_max, &r_curn);
	is_continue = pbar_is_continue(p_pbar);
  
  p_tmp = p_sub->Attribute("interval");
  if(p_tmp != NULL)
  {
    interval = atoi(p_tmp);
  }    

  p_tmp = p_sub->Attribute("r_min");
  if(p_tmp != NULL)
  {
    r_min = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }    

  p_tmp = p_sub->Attribute("r_max");
  if(p_tmp != NULL)
  {
    r_max = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("r_curn");
  if(p_tmp != NULL)
  {
    r_curn = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  } 

  p_tmp = p_sub->Attribute("is_continue");
  if(p_tmp != NULL)
  {
    is_continue = (BOOL)atoi(p_tmp);
  }   

  //dir, 0 : horizontal, 1 : vertical.
  p_tmp = p_sub->Attribute("is_hori");
  if(p_tmp != NULL)
  {
    is_hori = (BOOL)atoi(p_tmp);
  }   
  
  pbar_set_direction(p_pbar, is_hori);
  pbar_set_rstyle(p_pbar, r_min, r_max, r_curn);
  pbar_set_workmode(p_pbar, is_continue, interval);
}

static void sbar_priv_adjust(TiXmlElement *p_sub, control_t *p_sbar)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  BOOL is_autosize = FALSE, is_hori = TRUE;
	ctrl_rstyle_t rstyle = {0};
	
  mmi_assert(p_sub != NULL);
  mmi_assert(p_sbar != NULL);

  is_hori = sbar_is_hori(p_sbar);
  is_autosize = sbar_is_autosize(p_sbar);
  sbar_get_mid_rstyle(p_sbar, &rstyle);
  
  p_tmp = p_sub->Attribute("is_hori");
  if(p_tmp != NULL)
  {
    is_hori = (BOOL)atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("is_autosize");
  if(p_tmp != NULL)
  {
    is_autosize = (BOOL)atoi(p_tmp);
  }     

  p_tmp = p_sub->Attribute("r_mid_active");
  if(p_tmp != NULL)
  {
    rstyle.show_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  } 

  p_tmp = p_sub->Attribute("r_mid_hlight");
  if(p_tmp != NULL)
  {
    rstyle.hl_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("r_mid_inactive");
  if(p_tmp != NULL)
  {
    rstyle.gray_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }     
  
  sbar_set_autosize_mode(p_sbar, is_autosize);
  sbar_set_direction(p_sbar, is_hori);
  sbar_set_mid_rstyle(p_sbar, rstyle.show_idx, rstyle.hl_idx, rstyle.gray_idx);
}

static void sbox_priv_adjust(TiXmlElement *p_sub, control_t *p_sbox)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  u16 left = 0, top = 0;
  u32 f_active = 0, f_hlight = 0, f_inactive = 0;
  u32 align = 0;
	ctrl_rstyle_t rstyle = {0};
	
  mmi_assert(p_sub != NULL);
  mmi_assert(p_sbox != NULL);

  align = sbox_get_align_type(p_sbox);
  sbox_get_fstyle(p_sbox, &f_active, &f_hlight, &f_inactive);
  sbox_get_string_offset(p_sbox, &left, &top);
  sbox_get_mid_rstyle(p_sbox, &rstyle);
  
  p_tmp = p_sub->Attribute("align");
  if(p_tmp != NULL)
  {
    align = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("f_active");
  if(p_tmp != NULL)
  {
    f_active = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }       

  p_tmp = p_sub->Attribute("f_hlight");
  if(p_tmp != NULL)
  {
    f_hlight = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("f_inactive");
  if(p_tmp != NULL)
  {
    f_inactive = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("left");
  if(p_tmp != NULL)
  {
    left = atoi(p_tmp);
  }     

  p_tmp = p_sub->Attribute("top");
  if(p_tmp != NULL)
  {
    top = atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("r_mid_active");
  if(p_tmp != NULL)
  {
    rstyle.show_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }       

  p_tmp = p_sub->Attribute("r_mid_hlight");
  if(p_tmp != NULL)
  {
    rstyle.hl_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("r_mid_inactive");
  if(p_tmp != NULL)
  {
    rstyle.gray_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }    
  
  sbox_set_align_type(p_sbox, align);
  sbox_set_fstyle(p_sbox, f_hlight, f_active, f_inactive);
  sbox_set_mid_rstyle(p_sbox, rstyle.show_idx, rstyle.hl_idx, rstyle.gray_idx);
  sbox_set_string_offset(p_sbox, left, top);
}

static void text_priv_adjust(TiXmlElement *p_sub, control_t *p_text)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  u32 align = 0;
  BOOL is_breakword = FALSE;
  u32 f_active = 0, f_hlight = 0, f_inactive = 0;
  u8 line_vgap = 0;
  u16 left = 0, top = 0;

  mmi_assert(p_sub != NULL);
  mmi_assert(p_text != NULL);

  align = text_get_align_type(p_text);
  is_breakword = text_is_breakword(p_text);
  text_get_fstyle(p_text, &f_active, &f_hlight, &f_inactive);
  text_get_offset(p_text, &left, &top);
  line_vgap = text_get_line_gap(p_text);
  
  p_tmp = p_sub->Attribute("align");
  if(p_tmp != NULL)
  {
    align = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("breakword");
  if(p_tmp != NULL)
  {
    is_breakword = (BOOL)atoi(p_tmp);
  }       

  p_tmp = p_sub->Attribute("f_active");
  if(p_tmp != NULL)
  {
    f_active = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }       

  p_tmp = p_sub->Attribute("f_hlight");
  if(p_tmp != NULL)
  {
    f_hlight = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("f_inactive");
  if(p_tmp != NULL)
  {
    f_inactive = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("line_gap");
  if(p_tmp != NULL)
  {
    line_vgap = atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("left");
  if(p_tmp != NULL)
  {
    left = atoi(p_tmp);
  }     

  p_tmp = p_sub->Attribute("top");
  if(p_tmp != NULL)
  {
    top = atoi(p_tmp);
  }       
  
  text_set_align_type(p_text, align);
  text_set_breakword(p_text, is_breakword);
  text_set_font_style(p_text, f_active, f_hlight, f_inactive);
  text_set_line_gap(p_text, line_vgap);
  text_set_offset(p_text, left, top);
}

static void tbox_priv_adjust(TiXmlElement *p_sub, control_t *p_tbox)
{
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  u32 align = 0;
  u32 f_active = 0, f_hlight = 0, f_inactive = 0;
  u8 max_width = 0;
  u16 left = 0, top = 0;

  mmi_assert(p_sub != NULL);
  mmi_assert(p_tbox != NULL);

  align = tbox_get_align_type(p_tbox);
  tbox_get_offset(p_tbox, &left, &top);
  tbox_get_fstyle(p_tbox, &f_active, &f_hlight, &f_inactive);
  max_width = tbox_get_max_num_width(p_tbox);
  
  p_tmp = p_sub->Attribute("left");
  if(p_tmp != NULL)
  {
    left = atoi(p_tmp);
  }     

  p_tmp = p_sub->Attribute("top");
  if(p_tmp != NULL)
  {
    top = atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("f_active");
  if(p_tmp != NULL)
  {
    f_active = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }       

  p_tmp = p_sub->Attribute("f_hlight");
  if(p_tmp != NULL)
  {
    f_hlight = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }  

  p_tmp = p_sub->Attribute("f_inactive");
  if(p_tmp != NULL)
  {
    f_inactive = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   
  
  p_tmp = p_sub->Attribute("align");
  if(p_tmp != NULL)
  {
    align = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   

  p_tmp = p_sub->Attribute("maxwidth");
  if(p_tmp != NULL)
  {
    max_width = atoi(p_tmp);
  }   
  
  tbox_set_align_type(p_tbox, align);
  tbox_set_font_style(p_tbox, f_active, f_hlight, f_inactive);
  tbox_set_max_num_width(p_tbox, max_width);
  tbox_set_offset(p_tbox, left, top);
}

static void xml_adjust_children(TiXmlElement *p_element, control_t *p_parent)
{
  TiXmlElement *p_sub = NULL;
  control_t *p_ctrl = NULL;
  ctrl_type_t type = CTRL_BMAP;
  
  mmi_assert(p_element != NULL);
  mmi_assert(p_parent != NULL);

  p_sub = (TiXmlElement *)p_element->FirstChild("control");
  while(p_sub != NULL)
  {
    p_ctrl = ctrl_base_adjust(p_sub, p_parent);

    if(p_ctrl != NULL)
    {
    	type = ctrl_get_type(p_ctrl);

    	switch(type)
    	{
				case CTRL_BMAP:
					bmap_priv_adjust(p_sub, p_ctrl);
					break;

				case CTRL_CBOX:
					cbox_priv_adjust(p_sub, p_ctrl);
					break;

				case CTRL_CONT:
					cont_priv_adjust(p_sub, p_ctrl);
					break;

				case CTRL_EBOX:
					ebox_priv_adjust(p_sub, p_ctrl);
					break;

				case CTRL_IPBOX:
					ipbox_priv_adjust(p_sub, p_ctrl);
					break;

				case CTRL_LIST:
					list_priv_adjust(p_sub, p_ctrl);
					break;

				case CTRL_MBOX:
					mbox_priv_adjust(p_sub, p_ctrl);
					break;

				case CTRL_NBOX:
					nbox_priv_adjust(p_sub, p_ctrl);
					break;

				case CTRL_PBAR:
					pbar_priv_adjust(p_sub, p_ctrl);
					break;					

				case CTRL_SBAR:
					sbar_priv_adjust(p_sub, p_ctrl);
					break;					

				case CTRL_SBOX:
					sbox_priv_adjust(p_sub, p_ctrl);
					break;

				case CTRL_TEXT:
					text_priv_adjust(p_sub, p_ctrl);
					break;					

				case CTRL_TBOX:
					tbox_priv_adjust(p_sub, p_ctrl);
					break;					

				default:
					mmi_assert(0);
					break;

    	}

      xml_adjust_children(p_sub, p_ctrl);
    }
  
    p_sub = p_sub->NextSiblingElement();
  };
}


static void xml_adjust_mainwin(control_t *p_root, void *p_xml_data)
{
  TiXmlDocument *p_xml = NULL;
  TiXmlElement *p_element = NULL;
  rect_t frame = {0};
  const char *p_tmp = NULL;
  char *p_stop = NULL;
  obj_attr_t attr = OBJ_ATTR_ACTIVE;
  u8 style = 0;
  obj_sts_t sts = OBJ_STS_SHOW;
	ctrl_rstyle_t rstyle = {0};
	u16 ctrl_bg = 0, root_id = 0;
	  
  mmi_assert(p_xml_data != NULL);
  mmi_assert(p_root != NULL);
  
  p_xml = new TiXmlDocument();
  p_xml->Parse((char *)p_xml_data, 0, TIXML_ENCODING_UTF8);

  TiXmlHandle hDoc((TiXmlDocument *)p_xml);  

  p_element = hDoc.FirstChild("menu").Element();

  if(p_element == NULL)
  {
    return;
  }

	mmi_assert(TRUE == ctrl_is_root(p_root));
  mmi_assert(STR_CASE_CMP((char *)CTRL_CONT, (char *)ctrl_get_type(p_root)) == 0);

  //adjust mainwin.
	ctrl_get_frame(p_root, &frame);
	attr = ctrl_get_attr(p_root);
	sts = ctrl_get_sts(p_root);
	style = ctrl_get_style(p_root);
	ctrl_get_rstyle(p_root, &rstyle);
	root_id = ctrl_get_ctrl_id(p_root);
	ctrl_bg = ctrl_get_bg(p_root);	

	//bg
	p_tmp = p_element->Attribute("bg");
  if(p_tmp != NULL)
  {
	  ctrl_bg = atoi(p_tmp);
	}
	
  //x
  p_tmp = p_element->Attribute("x");
  if(p_tmp != NULL)
  {
	  frame.left = atoi(p_tmp);
	}
	
  //y
  p_tmp = p_element->Attribute("y");
  if(p_tmp != NULL)
  {
	  frame.top = atoi(p_tmp);
  }
  
  //w
  p_tmp = p_element->Attribute("w");
  if(p_tmp != NULL)
  {
  	frame.right = (frame.left + atoi(p_tmp));
  }
  
  //h
  p_tmp = p_element->Attribute("h");
  if(p_tmp != NULL)
  {
  	frame.bottom = (frame.top + atoi(p_tmp));
	}
	
  //attr
  p_tmp = p_element->Attribute("attr");
  if(p_tmp != NULL)
  {
    attr = (obj_attr_t)atoi(p_tmp);
  }  

  //sts
  p_tmp = p_element->Attribute("sts");
  if(p_tmp != NULL)
  {
    sts = (obj_sts_t)atoi(p_tmp);
  }  

  //style
  p_tmp = p_element->Attribute("style");
  if(p_tmp != NULL)
  {
    style = atoi(p_tmp);
  }  

  //rect-style
  p_tmp = p_element->Attribute("r_active");
  if(p_tmp != NULL)
  {
    rstyle.show_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }    
  
  p_tmp = p_element->Attribute("r_hlight");
  if(p_tmp != NULL)
  {
    rstyle.hl_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }   
  
  p_tmp = p_element->Attribute("r_inactive");
  if(p_tmp != NULL)
  {
    rstyle.gray_idx = strtoul(p_tmp, &p_stop, 16);//atoi(p_tmp);
  }    
  
  //try to adjust surface.
	ctrl_set_bg(p_root, ctrl_bg);
  
  ctrl_set_frame(p_root, &frame);
  ctrl_set_rstyle(p_root, rstyle.show_idx, rstyle.hl_idx, rstyle.gray_idx);
  ctrl_set_attr(p_root, attr);
  ctrl_set_sts_ex(p_root, sts);
  ctrl_set_style(p_root, style);
  ctrl_empty_invrgn(p_root);
  ctrl_add_rect_to_invrgn(p_root, NULL);

  fw_reset_mainwin(ctrl_get_ctrl_id(p_root));
  
  //adjust children.
  xml_adjust_children(p_element, p_root);  

  return;
}

void xml_paint_menu(control_t *p_root, BOOL is_force)
{
  u16 i = 0, root_id = 0;
  xml_menu_t *p_menu = NULL;
  xml_skin_t *p_info = NULL;

  mmi_assert(p_root != NULL);
  mmi_assert(ctrl_is_root(p_root));

#ifdef XML_DUMP_ENABLE
	xml_dump_menu(p_root);
#endif

#if 1  
  p_info = (xml_skin_t *)class_get_handle_by_id(XML_SKIN_CLASS_ID);

  if((p_info != NULL) && (p_info->p_skin != NULL))
  {
    root_id = ctrl_get_ctrl_id(p_root);
    
    //to check if the root id exist in skin block.
    for(i = 0; i < p_info->p_skin->menu_cnt; i++)
    {
      p_menu = (xml_menu_t *)(p_info->p_menu_data + i * sizeof(xml_menu_t));

      if(root_id == p_menu->menu_id)
      {
        //hit
        break;
      }
    }

    if(i != p_info->p_skin->menu_cnt)
    {
      //adjust surface.
      xml_adjust_mainwin(p_root, (u8 *)p_info->p_data + p_menu->xml_offset); 
    }
  }
#endif  
  ctrl_paint_ctrl(p_root, is_force);

  return;
}

void xml_dump_children(hfile_t hfile, control_t *p_parent)
{
	control_t *p_child = NULL;
	u8 asc_str[512] = {0};
	rect_t frame = {0};
	rect_t mrect = {0};
	ctrl_rstyle_t rstyle = {0};
	list_xstyle_t list_rstyle = {0};
	ctrl_type_t type = CTRL_CONT;
	u16 left = 0, top = 0;
	u8 str_left = 0, str_top = 0, icon_left = 0, icon_top = 0;
	u32 f_active = 0, f_inactive = 0, f_hlight = 0;
	u32 r_min = 0, r_max = 0, r_curn = 0, i = 0;
	//u32 j = 0;
	u8 interval = 0, h_interval = 0, v_interval = 0;
	u16 total = 0;
	u16 col = 0, row = 0, win_col = 0, win_row = 0;
	u16 icon_hlight = 0, icon_normal = 0;
	u16 field_left = 0, field_top = 0, field_width = 0, field_height = 0;
	list_xstyle_t xfstyle = {0}, xrstyle = {0};
	u8 field_num = 0;
	u32 field_attr = 0;
	//u32 layer = 0;
	
	mmi_assert(p_parent != NULL);

	p_child = ctrl_get_first_child(p_parent);
	
	while(p_child != NULL)
	{
		//layer++;

		//for(i = 0; i < layer; i++)
		//{
		//	sprintf((char *)asc_str, "	");
		//	vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);
		//}
		
		ctrl_get_frame(p_child, &frame);
		ctrl_get_mrect(p_child, &mrect);
		ctrl_get_rstyle(p_child, &rstyle);
		type = ctrl_get_type(p_child);
		
		sprintf((char *)asc_str, 
			"<control type=\"%d\" id=\"%d\" bg = \"%d\" "
			"x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" "
			"style=\"%d\" attr=\"%d\" sts=\"%d\" "
			"mleft=\"%d\" mtop=\"%d\" mright=\"%d\" mbottom=\"%d\" "
			"r_active=\"0x%x\" r_hlight=\"0x%x\" r_inactive=\"0x%x\"",
			type, ctrl_get_ctrl_id(p_child), ctrl_get_bg(p_child),
			frame.left, frame.top, RECTW(frame), RECTH(frame),
			ctrl_get_style(p_child), ctrl_get_attr(p_child), ctrl_get_sts(p_child),
			mrect.left, mrect.top, mrect.right, mrect.bottom,
			(unsigned int)rstyle.show_idx, (unsigned int)rstyle.hl_idx, (unsigned int)rstyle.gray_idx);

		vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);

  	switch(type)
  	{
			case CTRL_BMAP:
				bmap_get_bmap_offset(p_child, &left, &top);
				
				if(!bmap_get_content_type(p_child))
				{
					sprintf((char *)asc_str, 
						" align=\"0x%x\" "
						"left=\"%d\" top=\"%d\" bmap_id=\"%d\">\n",
						(unsigned int)bmap_get_align_type(p_child),
						left, top, (u16)bmap_get_content(p_child));
				}
				else
				{
					sprintf((char *)asc_str, 
						" align=\"0x%x\" "
						"left=\"%d\" top=\"%d\">\n",
						(unsigned int)bmap_get_align_type(p_child),
						left, top);
				}

				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);
				break;

			case CTRL_CBOX:
				cbox_get_fstyle(p_child, &f_active, &f_hlight, &f_inactive);
				
				sprintf((char *)asc_str, 
					" align=\"0x%x\" "
					"f_active=\"0x%x\" f_hlight=\"0x%x\" f_inactive=\"0x%x\" "
					"cyc=\"%d\">\n",
					(unsigned int)cbox_get_align_style(p_child),
					(unsigned int)f_active, (unsigned int)f_hlight, (unsigned int)f_inactive,
					cbox_is_cycle_mode(p_child));

				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);
				break;

			case CTRL_CONT:
				sprintf((char *)asc_str, ">\n");
					
				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);
				break;

			case CTRL_EBOX:
	    	ebox_get_offset(p_child, &left, &top);
	    	ebox_get_fstyle(p_child, &f_active, &f_hlight, &f_inactive);
	    	
				sprintf((char *)asc_str, 
					" align=\"0x%x\" "
					"f_active=\"0x%x\" f_hlight=\"0x%x\" f_inactive=\"0x%x\" "
					"left=\"%d\" top=\"%d\" maxtext=\"%d\">\n",
					(unsigned int)ebox_get_align_type(p_child),
					(unsigned int)f_active, (unsigned int)f_hlight, (unsigned int)f_inactive,
					left, top, ebox_get_maxtext(p_child));

				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);				
				break;

			case CTRL_IPBOX:
	    	ipbox_get_offset(p_child, &left, &top);
	    	ipbox_get_fstyle(p_child, &f_active, &f_hlight, &f_inactive);
	    	
				sprintf((char *)asc_str, 
					" align=\"0x%x\" "
					"f_active=\"0x%x\" f_hlight=\"0x%x\" f_inactive=\"0x%x\" "
					"left=\"%d\" top=\"%d\" maxwidth=\"%d\">\n",
					(unsigned int)ipbox_get_align_type(p_child),
					(unsigned int)f_active, (unsigned int)f_hlight, (unsigned int)f_inactive,
					left, top, ipbox_get_max_num_width(p_child));

				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);				
				break;

			case CTRL_LIST:
	    	list_get_item_rstyle(p_child, &list_rstyle);
	    	
				sprintf((char *)asc_str,
					" columns=\"%d\" is_hori=\"%d\" interval=\"%d\" is_cycle=\"%d\" "
					"enable_page=\"%d\" page = \"%d\" "
					"r_item_active=\"0x%x\" r_item_inactive=\"0x%x\" "
					"r_item_hlight=\"0x%x\" r_item_select=\"0x%x\" r_item_select_hlight=\"0x%x\">\n",
					list_get_columns(p_child),
					list_is_hori(p_child),
					list_get_item_interval(p_child),
					list_is_cycle_mode(p_child),
					list_is_page_mode(p_child), list_get_page(p_child),
					(unsigned int)list_rstyle.n_xstyle, (unsigned int)list_rstyle.g_xstyle,
					(unsigned int)list_rstyle.f_xstyle, (unsigned int)list_rstyle.s_xstyle,
					(unsigned int)list_rstyle.sf_xstyle);

				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);			

				field_num = list_get_field_num(p_child);
				for(i = 0; i < field_num; i++)
				{
					list_get_field_attr(p_child, i, &field_attr,
						&field_left, &field_top, &field_width, &field_height,
						&xfstyle, &xrstyle);
			
				sprintf((char *)asc_str, 
				"<field_data idx=\"%d\" left =\"%d\" top=\"%d\" width=\"%d\" height=\"%d\" "
				"g_fstyle=\"0x%x\" n_fstyle=\"0x%x\" f_fstyle=\"0x%x\" s_fstyle=\"0x%x\" sf_fstyle=\"0x%x\" "
				"g_rstyle=\"0x%x\" n_rstyle=\"0x%x\" f_rstyle=\"0x%x\" s_rstyle=\"0x%x\" sf_rstyle=\"0x%x\"> "
				"</field_data>\n",
				(int)i, (int)field_left, (int)field_top, (int)field_width, (int)field_height,
				(unsigned int)xfstyle.g_xstyle, (unsigned int)xfstyle.n_xstyle, (unsigned int)xfstyle.f_xstyle, (unsigned int)xfstyle.s_xstyle, (unsigned int)xfstyle.sf_xstyle,
				(unsigned int)xrstyle.g_xstyle, (unsigned int)xrstyle.n_xstyle, (unsigned int)xrstyle.f_xstyle, (unsigned int)xrstyle.s_xstyle, (unsigned int)xrstyle.sf_xstyle);
					
					vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);
	 			}		
				break;

			case CTRL_MBOX:
				mbox_get_string_offset(p_child, &str_left, &str_top);
				mbox_get_icon_offset(p_child, &icon_left, &icon_top);
				mbox_get_string_fstyle(p_child, &f_active, &f_hlight, &f_inactive);
				mbox_get_item_rstyle(p_child, &rstyle);
				mbox_get_count(p_child, &total, &col, &row);
				mbox_get_size(p_child, &col, &row, &win_col, &win_row);
				mbox_get_item_interval(p_child, &h_interval, &v_interval);
					
				sprintf((char *)asc_str, 
					" str_left=\"%d\" str_top=\"%d\" str_align=\"0x%x\" "
					"icon_left=\"%d\" icon_top=\"%d\" icon_align=\"0x%x\" "
					"f_active=\"0x%x\" f_hlight=\"0x%x\" f_inactive=\"0x%x\" "
					"r_item_active=\"0x%x\" r_item_hlight=\"0x%x\" r_item_inactive=\"0x%x\" "
					"h_interval=\"%d\" v_interval=\"%d\" "
					"col=\"%d\" row=\"%d\" win_col=\"%d\" win_row=\"%d\">\n",
					str_left, str_top, (unsigned int)mbox_get_string_align_type(p_child),
					icon_left, icon_top, (unsigned int)mbox_get_icon_align_type(p_child),
					(unsigned int)f_active, (unsigned int)f_hlight, (unsigned int)f_inactive,
					(unsigned int)rstyle.show_idx, (unsigned int)rstyle.hl_idx, (unsigned int)rstyle.gray_idx,
					h_interval, v_interval,
					col, row, win_col, win_row);   

				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);
				
				for(i = 0; i < total; i++)
				{
					mbox_get_icon(p_child, i, &icon_hlight, &icon_normal);

					//for(j = 0; j < (layer + 1); j++)
					//{
					//	sprintf((char *)asc_str, "	");
					//	vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);
					//}	
					
					sprintf((char *)asc_str, 
					"<mbox_data item_idx=\"%d\" hlight_bmp=\"%d\" normal_bmp=\"%d\"></mbox_data>\n",
					(int)i, (int)icon_hlight, (int)icon_normal);
					vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);
	 			}				
				break;

			case CTRL_NBOX:
	    	nbox_get_offset(p_child, &left, &top);
	    	nbox_get_fstyle(p_child, &f_active, &f_hlight, &f_inactive);
	      	
				sprintf((char *)asc_str, 
					" align=\"0x%x\" "
					"f_active=\"0x%x\" f_hlight=\"0x%x\" f_inactive=\"0x%x\" "
					"left=\"%d\" top=\"%d\" is_underline=\"%d\">\n",
					(unsigned int)nbox_get_align_type(p_child),
					(unsigned int)f_active, (unsigned int)f_hlight, (unsigned int)f_inactive,
					left, top, nbox_is_underline(p_child));

				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);
				break;

			case CTRL_PBAR:
	    	pbar_get_rstyle(p_child, &r_min, &r_max, &r_curn);
				sprintf((char *)asc_str, 
					" is_hori=\"%d\" is_continue=\"%d\" "
					"r_min=\"0x%x\" r_max=\"0x%x\" r_curn=\"0x%x\" interval=\"%d\">\n",
					pbar_is_hori(p_child), pbar_is_continue(p_child),
					(unsigned int)r_min, (unsigned int)r_max, (unsigned int)r_curn, interval);  

				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);				
				break;					

			case CTRL_SBAR:
	    	sbar_get_mid_rstyle(p_child, &rstyle);
	    	
				sprintf((char *)asc_str, 
					" is_hori=\"%d\" is_autosize=\"%d\" "
					"r_mid_active=\"0x%x\" r_mid_hlight=\"0x%x\" r_mid_inactive=\"0x%x\">\n",
					sbar_is_hori(p_child), sbar_is_autosize(p_child),
					(unsigned int)rstyle.show_idx, (unsigned int)rstyle.hl_idx, (unsigned int)rstyle.gray_idx);  

				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);				
				break;		

			case CTRL_SBOX:
	    	sbox_get_fstyle(p_child, &f_active, &f_hlight, &f_inactive);
	    	sbox_get_string_offset(p_child, &left, &top);
	    	sbox_get_mid_rstyle(p_child, &rstyle);
	    	
				sprintf((char *)asc_str, 
					" align=\"0x%x\" "
					"f_active=\"0x%x\" f_hlight=\"0x%x\" f_inactive=\"0x%x\" "
					"left=\"%d\" top=\"%d\" "
					"r_mid_active=\"0x%x\" r_mid_hlight=\"0x%x\" r_mid_inactive=\"0x%x\">\n",
					(unsigned int)sbox_get_align_type(p_child),
					(unsigned int)f_active, (unsigned int)f_hlight, (unsigned int)f_inactive,
					left, top, (unsigned int)rstyle.show_idx,
					(unsigned int)rstyle.hl_idx, (unsigned int)rstyle.gray_idx);    
					
				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);				
				break;

			case CTRL_TEXT:
	    	text_get_fstyle(p_child, &f_active, &f_hlight, &f_inactive);
	    	text_get_offset(p_child, &left, &top);
				sprintf((char *)asc_str, 
					" align=\"0x%x\" "
					"f_active=\"0x%x\" f_hlight=\"0x%x\" f_inactive=\"0x%x\" "
					"left=\"%d\" top=\"%d\" breakword=\"%d\" line_gap=\"%d\">\n",
					(unsigned int)text_get_align_type(p_child),
					(unsigned int)f_active, (unsigned int)f_hlight, (unsigned int)f_inactive,
					left, top, text_is_breakword(p_child), text_get_line_gap(p_child));   

				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);				
				break;					

			case CTRL_TBOX:
	    	tbox_get_fstyle(p_child, &f_active, &f_hlight, &f_inactive);
	    	tbox_get_offset(p_child, &left, &top);
				sprintf((char *)asc_str, 
					" align=\"0x%x\" "
					"f_active=\"0x%x\" f_hlight=\"0x%x\" f_inactive=\"0x%x\" "
					"left=\"%d\" top=\"%d\" maxwidth=\"%d\">\n",
					(unsigned int)tbox_get_align_type(p_child),
					(unsigned int)f_active, (unsigned int)f_hlight, (unsigned int)f_inactive,
					left, top, tbox_get_max_num_width(p_child));

				vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);					
				break;					

			default:
				mmi_assert(0);
				break;

  	}  	

		xml_dump_children(hfile, p_child);

		//for(i = 0; i < layer; i++)
		//{
		//	sprintf((char *)asc_str, "	");
		//	vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);
		//}		
		
		sprintf((char *)asc_str, "</control>\n");
		vfs_write((void *)asc_str, strlen((char *)asc_str), 1, hfile);

		p_child = ctrl_get_next_ctrl(p_child);
	}
}

void xml_dump_menu(control_t *p_root)
{
	u8 asc_str[256] = {0};
	u16 uni_str[256] = {0};
	hfile_t hfile = NULL;
	rect_t frame = {0};
	rect_t mrect = {0};
	ctrl_rstyle_t rstyle = {0};
	ctrl_type_t type = CTRL_CONT;
	u8 id = 0;
	
  mmi_assert(p_root != NULL);
  mmi_assert(ctrl_is_root(p_root));
	
	sprintf((char *)asc_str, "C:\\%d.xml", ctrl_get_ctrl_id(p_root));

	str_asc2uni(asc_str, uni_str);
	
	hfile = vfs_open(uni_str, VFS_NEW);
	mmi_assert(hfile != NULL);

	ctrl_get_frame(p_root, &frame);
	ctrl_get_mrect(p_root, &mrect);
	ctrl_get_rstyle(p_root, &rstyle);
	type = ctrl_get_type(p_root);
	id = ctrl_get_ctrl_id(p_root);

	mmi_assert(STR_CASE_CMP((char *)CTRL_CONT, (char *)ctrl_get_type(p_root)) == 0);

	sprintf((char *)asc_str,
		"<menu type=\"%d\" id=\"%d\" bg = \"%d\" "
		"x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" "
		"style=\"%d\" attr=\"%d\" sts=\"%d\" "
		"mleft=\"%d\" mtop=\"%d\" mright=\"%d\" mbottom=\"%d\" "
		"r_active=\"0x%x\" r_hlight=\"0x%x\" r_inactive=\"0x%x\">\n",
		type, id, ctrl_get_bg(p_root),
		frame.left, frame.top, RECTW(frame), RECTH(frame),
		ctrl_get_style(p_root), ctrl_get_attr(p_root), ctrl_get_sts(p_root),
		mrect.left, mrect.top, mrect.right, mrect.bottom,
		(unsigned int)rstyle.show_idx, (unsigned int)rstyle.hl_idx, (unsigned int)rstyle.gray_idx);

	vfs_write(asc_str, strlen((char *)asc_str), 1, hfile);

	xml_dump_children(hfile, p_root);

	sprintf((char *)asc_str, "</menu>\n");
	vfs_write(asc_str, strlen((char *)asc_str), 1, hfile);

	vfs_close(hfile);
}

BOOL _get_logo(u16 logo_id, u32 *p_addr, u32 *p_size)
{
  xml_logo_t *p_info = NULL;
	logo_head_t logo_head = {0};
	void *p_dm_handle = NULL;
	
  p_info = (xml_logo_t *)class_get_handle_by_id(XML_LOGO_CLASS_ID);
  mmi_assert(p_info != NULL);  

  p_dm_handle = (void *)class_get_handle_by_id(DM_CLASS_ID);
  mmi_assert(p_dm_handle != NULL);

  if(p_info->logo_addr == 0 || p_info->logo_size == 0)
  {
		return FALSE;
  }

  mmi_assert(logo_id < p_info->logo.logo_cnt);  

  dm_read(p_dm_handle, BLOCK_XML_LOGO,
  	0, sizeof(logo_t) + logo_id * sizeof(logo_head_t), sizeof(logo_head_t), (u8 *)&logo_head);

	if(logo_head.size == 0)
	{
		return FALSE;
	}

	*p_addr = (p_info->logo_addr + logo_head.offset);
	*p_size = logo_head.size;

	return TRUE;
}

BOOL xml_logo_init(void)
{
  void *p_dm_handle = NULL;
  xml_logo_t *p_info = NULL;
  
  p_info = (xml_logo_t *)mmi_alloc_buf(sizeof(xml_logo_t));
  mmi_assert(p_info != NULL);

  memset((void *)p_info, 0, sizeof(xml_logo_t));
  class_register(XML_LOGO_CLASS_ID, (class_handle_t)p_info);

  p_dm_handle = (void *)class_get_handle_by_id(DM_CLASS_ID);
  mmi_assert(p_dm_handle != NULL);
  
  p_info->logo_addr = dm_get_block_addr(p_dm_handle, BLOCK_XML_LOGO);
  p_info->logo_size = dm_get_block_size(p_dm_handle, BLOCK_XML_LOGO);

  if(p_info->logo_addr == 0 || p_info->logo_size == 0)
  {
		return FALSE;
  }

	dm_read(p_dm_handle, BLOCK_XML_LOGO, 0, 0, sizeof(logo_t), (u8 *)&p_info->logo);

	p_info->logo_get = _get_logo;
		
  return TRUE;
}

void xml_logo_release(void)
{
  xml_logo_t *p_info = NULL;

  p_info = (xml_logo_t *)class_get_handle_by_id(XML_LOGO_CLASS_ID);
  mmi_assert(p_info != NULL);

  p_info->logo_get = NULL;
}

BOOL xml_get_logo(u16 logo_id, u32 *p_addr, u32 *p_size)
{
  xml_logo_t *p_info = NULL;
	
  p_info = (xml_logo_t *)class_get_handle_by_id(XML_LOGO_CLASS_ID);
  mmi_assert(p_info != NULL);  

	if(p_info->logo_get != NULL)
	{
		return p_info->logo_get(logo_id, p_addr, p_size);
	}

	return FALSE;
}
