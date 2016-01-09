#ifndef __VF_FT_H__
#define __VF_FT_H__

/*!
  vf ft config.
  */
typedef struct
{
  /*!
    max count
    */
  u32 max_cnt;
  /*!
    max width
    */
  u16 max_width;
  /*!
    max height
    */
  u16 max_height;    
  /*!
    is alpha blending enable(gpe support), if yes->gray mode, else mono mode.
    */
  BOOL is_alpha_spt;    
  /*!
    bits per pixel.
    */
  u8 bpp;
}vf_ft_cfg_t;


/*!
  attach array font handle.
  */
void vf_ft_attach(handle_t rsc_handle, vf_ft_cfg_t *p_cfg);

/*!
  dettach array font handle.
  */
void vf_ft_detach(handle_t rsc_handle);

#endif

