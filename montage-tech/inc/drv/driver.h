/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __DRIVER_H__
#define __DRIVER_H__

/*!
  running in ASIC
  */
#ifdef WIN32
/*!
  comment
  */
#define ATTACH_DRIVER(module, chip, chip_version, board)  \
    module##_SIM(module, chip, chip_version, board)
#else

#ifdef FPGA
/*!
  comment
  */
#define ATTACH_DRIVER(module, chip, chip_version, board)  \
    module##_FPGA(module, chip, chip_version, board)
#else
/*!
  comment
  */
#define ATTACH_DRIVER(module, chip, chip_version, board)  \
    module(module, chip, chip_version, board)
#endif

#endif
/*!
  usb1 device
  */
#define DEV_NAME_BLOCK_USB1 "usb1"
/*!
  usb0 device
  */
#define DEV_NAME_BLOCK_USB0 "usb0"
/*!
memory card
  */
#define DEV_NAME_BLOCK_MC1 "mc1"
/*!
  rtl8188EUS/ETV type 
  */
#define RTL_REALTEK_WIFI_NAME  "usb_wifi_real_tek"
/*!
 mtk5370 
  */
#define MTK_RALINK_WIFI_NAME  "usb_wifi_ralink"
/*!
 mtk7601 
  */
#define MTK_RALINK7601_WIFI_NAME  "usb_wifi_ralink7601"



/*!
  smart card
  */
#define SMC(module, chip, chip_version, board) \
  smc_attach##_##chip(#module"_"#chip)
  
/*!
  smart card 7816
  */
extern RET_CODE smc_attach_7816(char *name);

/*!
  uio

  \param[out] name
  */
extern RET_CODE uio_attach(char *name);

/*!
  uio

  \param[out] name
  */
extern RET_CODE uio_jazz_attach(char *name);

/*!
  uio

  \param[out] name
  */
extern RET_CODE uio_warriors_attach(char *name);

/*!
  uio

  \param[out] name
  */
RET_CODE uio_concerto_attach(char *p_name);


/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE uio_attach_magic(char *name) {return uio_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE uio_attach_wizards(char *name) {return uio_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE uio_attach_jazz(char *name) {return uio_jazz_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE uio_attach_warriors(char *name) {return uio_warriors_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE uio_attach_concerto(char *name) {return uio_concerto_attach(name);}


/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE uio_attach_nxp(char *name) {return uio_attach(name);}

/*!
  comment
  */
#define UIO(module, chip, chip_version, board)  \
    uio_attach##_##chip(#board)

/*!
  comment
  */
#define UIO_FPGA(module, chip, chip_version, board)  \
    uio_attach##_##chip(#board)
    
/*!
  comment
  */
#define UIO_SIM(module, chip, chip_version, board)  \
    uio_attach(#board)


/*!
  hdmi

  \param[]
  */
extern RET_CODE hdmi_anx8560_attach(char *name);

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE hdmi_attach_warriors(char *name) 
{
  return hdmi_anx8560_attach(name);
}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE hdmi_attach_concerto(char *name) 
{
  return hdmi_anx8560_attach(name);
}

/*!
  comments
  */
#define HDMI(module, chip, chip_version, board)  \
    hdmi_attach##_##chip(#module"_"#chip)

/*!
  comments
  */
#define HDMI_FPGA(module, chip, chip_version, board)  \
    hdmi_attach##_##chip(#module"_"#chip)
    
/*!
  pti

  \param[out] name
  */
extern RET_CODE pti_soft_attach(char *name);
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE pti_sim_attach_magic(char *name) {return pti_soft_attach(name);}

/*!
  pti

  \param[out] name
  */
extern RET_CODE wizards_pti_attach(char *name);

/*!
  pti

  \param[out] name
  */
extern RET_CODE magic_pti_attach(char *name);

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE pti_attach_magic(char *name) {return magic_pti_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE pti_attach_wizards(char *name) {return wizards_pti_attach(name);}

/*!
  pti

  \param[out] name
  */
extern RET_CODE nxp_pti_attach(char *p_name);
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE pti_attach_nxp(char *name) {return nxp_pti_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE pti_sim_attach_nxp(char *name) {return pti_soft_attach(name);}

/*!
  comment
  */
#define PTI_FPGA(module, chip, chip_version, board)  \
    pti_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define PTI(module, chip, chip_version, board)  \
    pti_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define PTI_SIM(module, chip, chip_version, board)  \
    pti_sim_attach##_##chip(#module"_"#chip)

/*!
  pti

  \param[out] name
  */
extern RET_CODE dmx_magic_attach(char *name);
/*!
  pti

  \param[out] name
  */
extern RET_CODE dmx_wizards_attach(char *name);
/*!
  pti

  \param[out] name
  */
extern RET_CODE dmx_warriors_attach(char *name);
/*!
  pti

  \param[out] name
  */
extern RET_CODE dmx_concerto_attach(char *name);


/*!
  pti

  \param[out] name
  */
extern RET_CODE dmx_orchid_attach(char *p_name);

/*!
  pti

  \param[out] name
  */
extern RET_CODE dmx_soft_attach(char *name);
/*!
  pti

  \param[out] name
  */
extern RET_CODE dmx_jazz_attach(char *p_name);

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE dmx_attach_magic(char *name) 
{
    return dmx_magic_attach(name);
}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE dmx_attach_wizards(char *name) 
{
    return dmx_wizards_attach(name);
}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE dmx_attach_warriors(char *name) 
{
    return dmx_warriors_attach(name);
}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE dmx_attach_concerto(char *name) 
{
    return dmx_concerto_attach(name);
}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE dmx_attach_orchid(char *name) 
{
    return dmx_orchid_attach(name);
}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE dmx_attach_jazz(char *name) 
{
    return dmx_jazz_attach(name);
}


/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE dmx_sim_attach(char *name) 
{
    return dmx_soft_attach(name);
}

/*!
  comment
  */
#define DMX(module, chip, chip_version, board)  \
    dmx_attach##_##chip(#module"_"#chip)

/*!
  comment
  */
#define DMX_FPGA(module, chip, chip_version, board)  \
    dmx_attach##_##chip(#module"_"#chip)

/*!
  comment
  */
#define DMX_SIM(module, chip, chip_version, board)  \
    dmx_sim_attach(#module"_"#chip)



/*!
   Attach the drivers to pdec device

   \param[in] name
  */
extern RET_CODE pdec_warriors_attach(char *name);

/*!
   Attach the drivers to pdec device

   \param[in] name
  */
extern RET_CODE pdec_sonata_attach(char *name);

/*!
   Attach the drivers to pdec device

   \param[in] name
  */
extern RET_CODE pdec_win_attach(char *name);

/*!
   Attach the drivers to pdec device

   \param[in] name
  */
extern RET_CODE jpeg_orchid_attach(char *p_name);

/*!
   Attach the drivers to pdec device

   \param[in] name
*/
extern RET_CODE pdec_jazz_attach(char *name);

/*!
   Attach the drivers to pdec device

   \param[in] name
  */
extern RET_CODE pdec_concerto_attach(char *name);


/*!
   Attach the drivers to warriors pdec device

   \param[in] name
  */
static inline RET_CODE pdec_attach_warriors(char *name)
    {return pdec_warriors_attach(name);}

/*!
   Attach the drivers to sonata pdec device

   \param[in] name
  */
static inline RET_CODE pdec_attach_sonata(char *name)
    {return pdec_sonata_attach(name);}
    
/*!
   Attach the drivers to win32 pdec device

   \param[in] name
  */
static inline RET_CODE pdec_sim_attach(char *name)
    {return pdec_win_attach(name);}

/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
static inline RET_CODE pdec_attach_orchid(char *name)
    {return jpeg_orchid_attach(name);}

/*!
   Attach the drivers to warriors pdec device
   
   \param[in] name
*/
static inline RET_CODE pdec_attach_jazz(char *name)
   {return pdec_jazz_attach(name);}

/*!
   Attach the drivers to warriors pdec device
   
   \param[in] name
*/
static inline RET_CODE pdec_attach_concerto(char *name)
   {return pdec_concerto_attach(name);}


/*!
  PDEC driver attachment 
  */
#define PDEC(module, chip, chip_version, board)  \
    pdec_attach##_##chip(#module"_"#chip)

/*!
  PDEC driver attachment 
  */
#define PDEC_FPGA(module, chip, chip_version, board)  \
    pdec_attach##_##chip(#module"_"#chip)

/*!
  PDEC driver attachment 
  */
#define PDEC_SIM(module, chip, chip_version, board)  \
    pdec_sim_attach(#module"_"#chip)

/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
extern RET_CODE disp_magic_attach(char *p_name);

/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
extern RET_CODE disp_wizards_attach(char *p_name);
/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
extern RET_CODE disp_jazz_attach(char *p_name);
/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
extern RET_CODE disp_warriors_attach(char *p_name);

/*!
   Attach the drivers to win32 disp device

   \param[in] name
  */
extern RET_CODE disp_win32_attach(char *p_name);

/*!
   Attach the drivers to orchid disp device

   \param[in] name
  */
extern RET_CODE disp_zoran_attach(char *p_name);
/*!
   Attach the drivers to orchid disp device

   \param[in] name
  */
extern RET_CODE disp_concerto_attach(char *p_name);



/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
static inline RET_CODE disp_attach_magic(char *name)
    {return disp_magic_attach(name);}
    
/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
  static inline RET_CODE disp_attach_wizards(char *name)
    {return disp_wizards_attach(name);}
    
/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
static inline RET_CODE disp_attach_warriors(char *name)
    {return disp_warriors_attach(name);}
    
/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
static inline RET_CODE disp_attach_jazz(char *name)
    {return disp_jazz_attach(name);}
/*!
   Attach the drivers to win32 disp device

   \param[in] name
  */
static inline RET_CODE disp_sim_attach(char *name)
    {return disp_win32_attach(name);}


/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
static inline RET_CODE disp_attach_orchid(char *name)
    {return disp_zoran_attach(name);}

/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
static inline RET_CODE disp_attach_concerto(char *name)
    {return disp_concerto_attach(name);}

/*!
  DISP driver attachment 
  */
#define DISP(module, chip, chip_version, board)  \
    disp_attach##_##chip(#module"_"#chip)
/*!
  DISP driver attachment 
  */
#define DISP_FPGA(module, chip, chip_version, board)  \
    disp_attach##_##chip(#module"_"#chip)    
/*!
  DISP driver attachment 
  */
#define DISP_SIM(module, chip, chip_version, board)  \
    disp_sim_attach(#module"_"#chip)


/*!
Register osd deivce
   \param[in] name
  */
extern RET_CODE osd_win32_attach(char * name);
/*! Attach wizards osd deivce
   \param[in] name
  */
extern RET_CODE wizards_osd_attach(char * name);
/*!Register nxp osd deivce
   \param[in] name
  */
extern RET_CODE nxp_osd_attach(char *p_name);
/*!
   Attach the drivers to win32 OSD device

   \param[in] name
  */
static inline RET_CODE osd_sim_attach_magic(char *name)
{
    return osd_win32_attach(name);
}
/*!
   Attach the drivers to Magic OSD device

   \param[in] name
  */
static inline RET_CODE osd_attach_magic(char *name)
    {return wizards_osd_attach(name);}
/*!
   Attach the drivers to Wizards OSD device

   \param[in] name
  */
static inline RET_CODE osd_attach_wizards(char *name)
    {return wizards_osd_attach(name);}
/*!
   Attach the drivers to win32 OSD device

   \param[in] name
  */
static inline RET_CODE osd_sim_attach_nxp(char *name)
{
    return osd_win32_attach(name);
}
/*!
   Attach the drivers to nxp OSD device

   \param[in] name
  */
static inline RET_CODE osd_attach_nxp(char *name)
    {return nxp_osd_attach(name);}
/*!
  OSD driver attachment for FPGA board
  */
#define OSD_FPGA(module, chip, chip_version, board)  \
    osd_attach##_##chip(#module"_"#chip)
/*!
  OSD driver attachment for chip board
  */
#define OSD(module, chip, chip_version, board)  \
    osd_attach##_##chip(#module"_"#chip)
/*!
  OSD driver attachment for simulator
  */
#define OSD_SIM(module, chip, chip_version, board)  \
    osd_sim_attach##_##chip(#module"_"#chip)


/*! Attach wizards osd sub deivce 

   \param[in] name
  */
extern RET_CODE wizards_sub_attach(char * name);
/*! Attach win32 osd sub deivce

   \param[in] name
  */
extern RET_CODE sub_win32_attach(char * name);
/*!
   Attach the drivers to win32 SUB device

   \param[in] name
  */
static inline RET_CODE sub_sim_attach_magic(char *name)
    {return sub_win32_attach(name);}
/*!
   Attach the drivers to Magic SUB device

   \param[in] name
  */
static inline RET_CODE sub_attach_magic(char *name)
    {return wizards_sub_attach(name);}
/*!
   Attach the drivers to Wizards SUB device

   \param[in] name
  */
static inline RET_CODE sub_attach_wizards(char *name)
    {return wizards_sub_attach(name);}    


/*!
  SUB driver attachment for FPGA board
  */
#define SUB_FPGA(module, chip, chip_version, board)  \
    sub_attach##_##chip(#module"_"#chip)
/*!
  SUB driver attachment for chip board
  */
#define SUB(module, chip, chip_version, board)  \
    sub_attach##_##chip(#module"_"#chip)
/*!
  SUB driver attachment for simulator
  */
#define SUB_SIM(module, chip, chip_version, board)  \
    sub_sim_attach##_##chip(#module"_"#chip)


/*!
  gpe

  \param[out] name
  */
extern RET_CODE gpe_4bit_attach(char * name);


/*!
  comment
  */
#define GPE_4BIT_FPGA(module, chip, chip_version, board)  \
    gpe_4bit_attach(#module"_"#chip)
/*!
  comment
  */
#define GPE_4BIT(module, chip, chip_version, board)  \
    gpe_4bit_attach(#module"_"#chip)
/*!
  comment
  */
#define GPE_4BIT_SIM(module, chip, chip_version, board)  \
    gpe_4bit_attach(#module"_"#chip)


/*!
  gpe

  \param[out] name
  */
extern RET_CODE gpe_8bit_attach(char * name);

/*!
  comment
  */
#define GPE_8BIT_FPGA(module, chip, chip_version, board)  \
    gpe_8bit_attach(#module"_"#chip)
/*!
  comment
  */
#define GPE_8BIT(module, chip, chip_version, board)  \
    gpe_8bit_attach(#module"_"#chip)
/*!
  comment
  */
#define GPE_8BIT_SIM(module, chip, chip_version, board)  \
    gpe_8bit_attach(#module"_"#chip)
    
/*!
   Attach the drivers to magic disp device

   \param[in] name
  */
extern RET_CODE gpe_soft_attach(char *p_name);
/*!
   Attach the drivers to warriors disp device

   \param[in] name
  */
extern RET_CODE gpe_warriors_attach(char *p_name);

/*!
   Attach the drivers to orchid disp device

   \param[in] name
  */
extern RET_CODE gpe_zoran_attach(char *p_name);

/*!
   Attach the drivers to concerto disp device

   \param[in] name
  */
extern RET_CODE cct_gpe_attach(char *p_name);

/*!
   Attach the drivers to orchid disp device

   \param[in] name
  */  
static inline RET_CODE gpe_vsb_attach_magic(char *name)
    {return gpe_soft_attach(name);}

/*!
   Attach the drivers to jazz gpe vsb device

   \param[in] name
  */
static inline RET_CODE gpe_vsb_attach_jazz(char *name)
    {return gpe_soft_attach(name);}
/*!
   Attach the drivers to GMI gpe vsb device

   \param[in] name
  */
static inline RET_CODE gpe_vsb_attach_wizards(char *name)
    {return gpe_soft_attach(name);}

/*!
   Attach the drivers to warriors gpe vsb device

   \param[in] name
  */
static inline RET_CODE gpe_vsb_attach_warriors(char *name)
    {return gpe_warriors_attach(name);}
/*!
   Attach the drivers to concerto gpe vsb device

   \param[in] name
  */
static inline RET_CODE gpe_vsb_attach_concerto(char *name)
    {return cct_gpe_attach(name);}


/*!
   Attach the drivers to orchid gpe  vsb device

   \param[in] name
  */
static inline RET_CODE gpe_vsb_attach_orchid(char *name)
    {return gpe_zoran_attach(name);}

/*!
   Attach the drivers to magic gpe vsb device

   \param[in] name
  */
static inline RET_CODE gpe_vsb_sim_attach_magic(char *name)
    {return gpe_soft_attach(name);}

/*!
   Attach the drivers to magic gpe vsb device

   \param[in] name
  */
static inline RET_CODE gpe_vsb_sim_attach_wizards(char *name)
    {return gpe_soft_attach(name);}
    
/*!
  GPE vsb driver attachment 
  */
#define GPE_VSB(module, chip, chip_version, board)  \
    gpe_vsb_attach##_##chip(#module"_"#chip)

/*!
  GPE vsb driver attachment 
  */
#define GPE_VSB_FPGA(module, chip, chip_version, board)  \
    gpe_vsb_attach##_##chip(#module"_"#chip)
    
/*!
  GPE vsb driver attachment 
  */
#define GPE_VSB_SIM(module, chip, chip_version, board)  \
    gpe_vsb_sim_attach##_##chip(#module"_"#chip)

/*!
   Attach wizards audio deivce
   \param[in] name
  */
extern RET_CODE wizards_audio_attach(char * name);

/*!
   Attach magic audio deivce
   \param[in] name
  */
extern RET_CODE magic_audio_attach(char * name);

/*!
  Attach fpga audio deivce
   \param[in] name
  */
extern RET_CODE wizards_m_audio_attach(char *name);

/*!
  Attach fpga audio deivce
   \param[in] name
  */
extern RET_CODE magic_audio_fpga_attach(char * name);

/*!
  Attach win32 audio deivce
   \param[in] name
  */
extern RET_CODE win32_audio_attach(char *name);
/*!
  Attach win32 audio deivce
   \param[in] name
  */
extern RET_CODE win32_aud_attach(char *name);
/*!
  Attach nxp audio deivce
   \param[in] name
  */
extern RET_CODE nxp_audio_attach(char *p_name);

/*!
   Attach orchid audio deivce

   \param[in] name
  */
extern RET_CODE orchid_audio_attach(char *p_name);
/*!
   Attach the drivers to Wizards audio device on chip

   \param[in] name
  */
static inline RET_CODE audio_attach_wizards(char *name)
    {return wizards_audio_attach(name);}
/*!
   Attach the drivers to Magic audio device on chip

   \param[in] name
  */
static inline RET_CODE audio_attach_magic(char *name)
    {return magic_audio_attach(name);}
/*!
   Attach the drivers to Wizards audio device on FPGA board

   \param[in] name
  */
  static inline RET_CODE m_audio_attach_wizards(char *name)
    {return wizards_m_audio_attach(name);}
/*!
   Attach the drivers to Wizards audio device on FPGA board

   \param[in] name
  */
static inline RET_CODE audio_fpga_attach_magic(char *name)
    {return magic_audio_fpga_attach(name);}    
/*!
   Attach the drivers to win32 audio device

   \param[in] name
  */
static inline RET_CODE audio_sim_attach_magic(char *name)
    {return win32_audio_attach(name);}    
/*!
   Attach the drivers to win32 audio device

   \param[in] name
  */
static inline RET_CODE audio_gmi_sim_attach_wizards(char *name)
    {return win32_aud_attach(name);}  
/*!
   Attach the drivers to Magic audio device on chip

   \param[in] name
  */
static inline RET_CODE audio_attach_nxp(char *name)
    {return nxp_audio_attach(name);}
/*!
   Attach the drivers to win32 audio device

   \param[in] name
  */
static inline RET_CODE audio_sim_attach_nxp(char *name)
    {return win32_audio_attach(name);} 
/*!
   Attach the drivers to orchid audio device on chip

   \param[in] name
  */
static inline RET_CODE audio_attach_orchid(char *name)
    {return orchid_audio_attach(name);}
/*!
  Audio driver attachment for FPGA board
  */
#define AUDIO_FPGA(module, chip, chip_version, board)  \
    audio_fpga_attach##_##chip(#module"_"#chip)
/*!
  Audio driver attachment for chip board
  */
#define AUDIO(module, chip, chip_version, board)  \
    audio_attach##_##chip(#module"_"#chip)
/*!
  Audio driver attachment for simulator
  */
#define AUDIO_SIM(module, chip, chip_version, board)  \
    audio_sim_attach##_##chip(#module"_"#chip)
    
/*!
  Audio driver attachment for GMI board
  */
#define AUDIO_GMI(module, chip, chip_version, board)  \
    m_audio_attach##_##chip(#module"_"#chip)
    
/*!
  Audio driver attachment for GMI SIM
  */
#define AUDIO_GMI_SIM(module, chip, chip_version, board)  \
    audio_gmi_sim_attach##_##chip(#module"_"#chip)
/*!
   Attach the drivers to  audio device on chip

   \param[in] name
  */
extern RET_CODE magic_audio_attach_vsb(char *p_name);

/*!
   Attach the drivers to  audio device on chip

   \param[in] name
  */
extern RET_CODE warriors_audio_attach(char *p_name);

/*!
   Attach the drivers to  audio device on chip

   \param[in] name
  */
extern RET_CODE concerto_audio_attach(char *p_name);

/*!
   Attach the drivers to  audio device on chip

   \param[in] name
  */
extern RET_CODE jazz_audio_attach(char *p_name);

/*!
   Attach the drivers to  audio device on win32

   \param[in] name
  */
extern RET_CODE win32_aud_attach(char *p_name);

/*!
   Attach the drivers to  audio device on chip

   \param[in] name
  */
static inline RET_CODE audio_vsb_attach_magic(char *name)
    {return magic_audio_attach_vsb(name);}

/*!
   Attach the drivers to  audio device on chip

   \param[in] name
  */
static inline RET_CODE audio_vsb_attach_warriors(char *name)
    {return warriors_audio_attach(name);}

/*!
   Attach the drivers to  audio device on chip

   \param[in] name
  */
static inline RET_CODE audio_vsb_attach_concerto(char *name)
    {return concerto_audio_attach(name);}

/*!
   Attach the drivers to  audio device on chip

   \param[in] name
  */
static inline RET_CODE audio_vsb_attach_jazz(char *name)
    {return jazz_audio_attach(name);}

/*!
   Attach the drivers to  audio device on chip

   \param[in] name
  */
static inline RET_CODE audio_vsb_sim_attach(char *name)
    {return win32_aud_attach(name);}

/*!
  Audio driver attachment for chip board
  */
#define AUDIO_VSB(module, chip, chip_version, board)  \
    audio_vsb_attach##_##chip(#module"_"#chip)
/*!
  Audio driver attachment for chip board
  */
#define AUDIO_VSB_FPGA(module, chip, chip_version, board)  \
    audio_vsb_attach##_##chip(#module"_"#chip)
/*!
  Audio driver attachment for win32 board
  */
#define AUDIO_VSB_SIM(module, chip, chip_version, board)  \
    audio_vsb_sim_attach(#module"_"#chip)
/*!
   Attach win32 vdec deivce

   \param[in] name
  */
extern RET_CODE win32_vdec_attach(char *name);

/*!
   Attach magic vdec deivce

   \param[in] name
  */
extern RET_CODE magic_vdec_attach(char *name);

/*!
   Attach magic vdec deivce

   \param[in] name
  */
extern RET_CODE warriors_vdec_attach(char *name);
/*!
   Attach magic vdec deivce

   \param[in] name
  */
extern RET_CODE concerto_vdec_attach(char *name);


/*!
   Attach sonata vdec deivce

   \param[in] name
  */
extern RET_CODE sonata_vdec_attach(char *name);

/*!
   Attach magic vdec deivce

   \param[in] name
  */
extern RET_CODE jazz_vdec_attach(char *name);
/*!
   Attach orchid vdec deivce

   \param[in] name
  */
extern RET_CODE orchid_vdec_attach(char *p_name);

/*!
   Attach the drivers to nxp video decoder device on chip

   \param[in] name
  */
static inline RET_CODE vdec_attach_magic(char *name)
    {return magic_vdec_attach(name);}
/*!
   Attach the drivers to nxp video decoder device on chip

   \param[in] name
  */
static inline RET_CODE vdec_attach_wizards(char *name)
    {return magic_vdec_attach(name);}
/*!
   Attach the drivers to nxp video decoder device on chip

   \param[in] name
  */
static inline RET_CODE vdec_attach_warriors(char *name)
    {return warriors_vdec_attach(name);}
/*!
   Attach the drivers to nxp video decoder device on chip

   \param[in] name
  */
static inline RET_CODE vdec_attach_concerto(char *name)
    {return concerto_vdec_attach(name);}


/*!
   Attach the drivers to sonata video decoder device on chip

   \param[in] name
  */
static inline RET_CODE vdec_attach_sonata(char *name)
    {return sonata_vdec_attach(name);}
    
/*!
   Attach the drivers to nxp video decoder device on chip

   \param[in] name
  */
static inline RET_CODE vdec_attach_jazz(char *name)
    {return jazz_vdec_attach(name);}

/*!
   Attach the drivers to orchid video decoder device on chip

   \param[in] name
  */
static inline RET_CODE vdec_attach_orchid(char *name)
    {return orchid_vdec_attach(name);}

/*!
   Attach the drivers to win32 video decoder device

   \param[in] name
  */
static inline RET_CODE vdec_sim_attach(char *name)
    {return win32_vdec_attach(name);}    
/*!
  Video decoder driver attachment for chip board
  */    
#define VDEC(module, chip, chip_version, board)  \
    vdec_attach##_##chip(#module"_"#chip)
/*!
  Video decoder driver attachment for chip board
  */    
#define VDEC_FPGA(module, chip, chip_version, board)  \
    vdec_attach##_##chip(#module"_"#chip)    
/*!
  Video decoder driver attachment for simulator
  */
#define VDEC_SIM(module, chip, chip_version, board)  \
    vdec_sim_attach(#module"_"#chip)    

/*!
   Attach win32 video deivce

   \param[in] name
  */
extern RET_CODE video_win32_attach(char *name);
/*!
   Attach wizards video deivce

   \param[in] name
  */
extern RET_CODE wizards_video_attach(char *name);

/*!
  Attach magic video deivce

   \param[in] name
  */
extern RET_CODE magic_video_attach(char *name);

/*!
  Attach magic video deivce

   \param[in] name
  */
extern RET_CODE magic_2m_video_attach(char *name);

/*!
   Attach nxp video deivce

   \param[in] name
  */
extern RET_CODE nxp_video_attach(char *p_name);
/*!
   Attach the drivers to win32 video device

   \param[in] name
  */
static inline RET_CODE video_sim_attach_magic(char *name)
    {return video_win32_attach(name);}

/*!
   Attach the drivers to win32 video device

   \param[in] name
  */
static inline RET_CODE video_sim_attach_magic_2m(char *name)
    {return video_win32_attach(name);}
/*!
   Attach the drivers to Magic video device on chip

   \param[in] name
  */
static inline RET_CODE video_attach_magic(char *name)
    {return magic_video_attach(name);}

/*!
   Attach the drivers to Magic video device on chip

   \param[in] name
  */
static inline RET_CODE video_attach_magic_2m(char *name)
    {return magic_2m_video_attach(name);}
/*!
   Attach the drivers to Wizards video device on chip

   \param[in] name
  */
static inline RET_CODE video_attach_wizards(char *name)
    {return wizards_video_attach(name);}
/*!
   Attach the drivers to nxp video device on chip

   \param[in] name
  */
static inline RET_CODE video_attach_nxp(char *name)
    {return nxp_video_attach(name);}
/*!
   Attach the drivers to win32 video device

   \param[in] name
  */
static inline RET_CODE video_sim_attach_nxp(char *name)
    {return video_win32_attach(name);}
/*!
  Video driver attachment for FPGA board
  */
#define VIDEO_FPGA(module, chip, chip_version, board)  \
    video_attach##_##chip(#module"_"#chip)
/*!
  Video driver attachment for chip board
  */
#define VIDEO(module, chip, chip_version, board)  \
    video_attach##_##chip(#module"_"#chip)
/*!
  Video driver attachment for simulator
  */
#define VIDEO_SIM(module, chip, chip_version, board)  \
    video_sim_attach##_##chip(#module"_"#chip)

/*!
  Attach wizards avsync deivce

   \param[in] name
  */
extern RET_CODE wizards_avsync_attach(char * name);

/*!
  Attach fpga avsync deivce
   \param[in] name
  */
extern RET_CODE wizards_fpga_avsync_attach(char *p_name);

/*!
   Attach magic avsync deivce

   \param[in] name
  */
extern RET_CODE magic_avsync_attach(char *p_name);

/*!
  Attach magic fpga avsync deivce 

   \param[in] name
  */
extern RET_CODE magic_fpga_avsync_attach(char *p_name);

/*!
  Attach wizards avsync deivce

   \param[in] name
  */
extern RET_CODE wizards_avsync_vsb_attach(char * name);

/*!
  Attach fpga avsync deivce
   \param[in] name
  */
extern RET_CODE wizards_fpga_avsync_vsb_attach(char *p_name);

/*!
   Attach magic avsync deivce

   \param[in] name
  */
extern RET_CODE magic_avsync_vsb_attach(char *p_name);

/*!
  Attach magic fpga avsync deivce 

   \param[in] name
  */
extern RET_CODE magic_fpga_avsync_vsb_attach(char *p_name);

/*!
   Attach win32 avsync deivce

   \param[in] name
  */
extern RET_CODE win32_avsync_attach(char * name);
/*!
   Attach win32 avsync deivce

   \param[in] name
  */
extern RET_CODE jazz_avsync_vsb_attach(char *p_name);
/*!
   Attach the drivers to win32 Av sync device

   \param[in] name
  */
static inline RET_CODE avsync_sim_attach_magic(char *name)
    {return win32_avsync_attach(name);}
/*!
   Attach the drivers to win32 Av sync device

   \param[in] name
  */
static inline RET_CODE avsync_sim_attach_vsb_magic(char *name)
    {return win32_avsync_attach(name);}
/*!
   Attach the drivers to win32 Av sync device

   \param[in] name
  */
static inline RET_CODE avsync_sim_attach_vsb_wizards(char *name)
    {return win32_avsync_attach(name);}
/*!
   Attach the drivers to Wizards Av sync device on chip

   \param[in] name
  */
static inline RET_CODE avsync_attach_wizards(char *name)
    {return wizards_avsync_attach(name);}
/*!
   Attach the drivers to Wizards Av sync device on FPGA board

   \param[in] name
  */  
static inline RET_CODE avsync_fpga_attach_wizards(char *name)
    {return wizards_fpga_avsync_attach(name);}       
/*!
   Attach the drivers to Magic Av sync device on chip

   \param[in] name
  */
static inline RET_CODE avsync_attach_magic(char *name)
    {return magic_avsync_attach(name);}
/*!
   Attach the drivers to Wizards Av sync device on FPGA board

   \param[in] name
  */
static inline RET_CODE avsync_fpga_attach_magic(char *name)
    {return magic_fpga_avsync_attach(name);}    

/*!
   Attach the drivers to Wizards Av sync device on chip

   \param[in] name
  */
static inline RET_CODE avsync_attach_vsb_wizards(char *name)
    {return wizards_avsync_vsb_attach(name);}
/*!
   Attach the drivers to Wizards Av sync device on FPGA board

   \param[in] name
  */  
static inline RET_CODE avsync_fpga_attach_wizards_vsb(char *name)
    {return wizards_fpga_avsync_vsb_attach(name);}       
/*!
   Attach the drivers to Magic Av sync device on chip

   \param[in] name
  */
static inline RET_CODE avsync_attach_vsb_magic(char *name)
    {return magic_avsync_vsb_attach(name);}
/*!
   Attach the drivers to jazz Av sync device on chip

   \param[in] name
  */
static inline RET_CODE avsync_attach_vsb_jazz(char *name)
    {return jazz_avsync_vsb_attach(name);}
/*!
   Attach the drivers to Wizards Av sync device on FPGA board

   \param[in] name
  */
static inline RET_CODE avsync_fpga_attach_magic_vsb(char *name)
    {return magic_fpga_avsync_vsb_attach(name);}    

/*!
  Av sync driver attachment for FPGA board
  */
#define AVSYNC_FPGA(module, chip, chip_version, board)  \
    avsync_fpga_attach##_##chip(#module"_"#chip)
/*!
  Av sync driver attachment for chip board
  */
#define AVSYNC(module, chip, chip_version, board)  \
    avsync_attach##_##chip(#module"_"#chip)
/*!
  Av sync driver attachment for simulator
  */
#define AVSYNC_SIM(module, chip, chip_version, board)  \
    avsync_sim_attach##_##chip(#module"_"#chip)
/*!
  Av sync driver attachment for simulator
  */
#define AVSYNC_VSB_SIM(module, chip, chip_version, board)  \
    avsync_sim_attach_vsb##_##chip(#module"_"#chip)
/*!
  Av sync driver attachment for nxp board
  */
#define AVSYNC_VSB(module, chip, chip_version, board)  \
    avsync_attach_vsb##_##chip(#module"_"#chip)


/*!
  vbi

  \param[out] name
  */
extern RET_CODE win32_vbi_inserter_attach(char * name);
/*!
  vbi

  \param[out] name
  */
extern RET_CODE magic_vbi_inserter_attach(char * name);
/*!
  vbi

  \param[out] name
  */
extern RET_CODE warriors_vbi_inserter_attach(char * name);

/*!
  vbi

  \param[out] name
  */
extern RET_CODE concerto_vbi_inserter_attach(char * name);

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE vbi_inserter_sim_attach_magic(char *name)
    {return win32_vbi_inserter_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE vbi_inserter_sim_attach_wizards(char *name)
    {return win32_vbi_inserter_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE vbi_inserter_sim_attach_warriors(char *name)
    {return win32_vbi_inserter_attach(name);}
    
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE vbi_inserter_attach_magic(char *name)
    {return magic_vbi_inserter_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE vbi_inserter_attach_wizards(char *name)
    {return magic_vbi_inserter_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE vbi_inserter_attach_warriors(char *name)
    {return warriors_vbi_inserter_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE vbi_inserter_attach_concerto(char *name)
    {return concerto_vbi_inserter_attach(name);}
        
/*!
  comment
  */
#define VBI_INSERTER_FPGA(module, chip, chip_version, board)  \
    vbi_inserter_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define VBI_INSERTER(module, chip, chip_version, board)  \
    vbi_inserter_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define VBI_INSERTER_SIM(module, chip, chip_version, board)  \
    vbi_inserter_sim_attach##_##chip(#module"_"#chip)


/*!
  i2c

  \param[out] name
  */
extern RET_CODE i2c_wzctrller_attach(char *name);
/*!
  i2c

  \param[out] name
  */
extern RET_CODE i2c_wactrller_attach(char *name);
/*!
  i2c

  \param[out] name
  */
extern RET_CODE i2c_jazz_attach(char *p_name);
/*!
  i2c

  \param[out] name
  */
extern RET_CODE i2c_concerto_attach(char *p_name);

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE i2c_attach_magic(char *name)
    {return i2c_wzctrller_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE i2c_attach_warriors(char *name)
    {return i2c_wactrller_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE i2c_attach_concerto(char *name)
    {return i2c_concerto_attach(name);}    
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE i2c_attach_jazz(char *name)
    {return i2c_jazz_attach(name);}

#ifdef MDA009A
/*!
  i2c

  \param[out] name
  */
extern RET_CODE i2c_cx2448x_attach(char *p_name);
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE i2c_attach_nxp(char *name)
    {return i2c_cx2448x_attach(name);}
#endif

/*!
  comment
  */
#define I2C_FPGA(module, chip, chip_version, board)  \
    i2c_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define I2C(module, chip, chip_version, board)  \
    i2c_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define I2C_SIM(module, chip, chip_version, board)  \
    i2c_sim_attach##_##chip(#module"_"#chip)

/*!
  char storage

  \param[out] name
  */
extern RET_CODE charsto_win32_attach(char *name);
/*!
  char storage

  \param[out] name
  */
extern RET_CODE spi_wz_attach(char *name);
/*!
  char storage

  \param[out] name
  */
extern RET_CODE spi_warriors_attach(char *name);
/*!
  char storage

  \param[out] name
  */
extern RET_CODE spi_concerto_attach(char *name);

/*!
  low power control driver

  \param[out] name
  */
extern RET_CODE lpower_magic_attach(char *name);
/*!
  low power control driver

  \param[out] name
  */
extern RET_CODE lpower_magic_attach_vsb(char *name);
 /*!
  low power control driver

  \param[out] name
  */
extern RET_CODE lpower_warriors_attach(char *name);
/*!
  char storage

  \param[out] p_name
  */
extern RET_CODE pnf_cx2448x_attach(char *p_name);
/*!
  char storage

  \param[out] name
  */
extern RET_CODE spi_supraxd18x_attach(char *p_name);
/*!
  char storage

  \param[out] name
  */
extern RET_CODE spiflash_jazz_attach(char *p_name);

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE charsto_sim_attach(char *name)
    {return charsto_win32_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE charsto_attach_magic(char *name)
    {return spi_wz_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE charsto_attach_jazz(char *name)
//    {return e2p_jazz_at24cxx_attach(name);}
{return spiflash_jazz_attach(name);}
    
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE charsto_attach_wizards(char *name)
    {return spi_wz_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE charsto_attach_nxp(char *name)
    {return pnf_cx2448x_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE charsto_sim_attach_nxp(char *name)
    {return charsto_win32_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE charsto_attach_warriors(char *name)
    {return spi_warriors_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE charsto_attach_concerto(char *name)
    {return spi_concerto_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE charsto_attach_orchid(char *name)
    {return spi_supraxd18x_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE lpower_attach_magic(char *name)
    {return lpower_magic_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE lpower_vsb_attach_magic(char *name)
    {return lpower_magic_attach_vsb(name);}

 /*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE lpower_attach_warriors(char *name)
    {return lpower_warriors_attach(name);}

/*!
  comment
  */
#define LPOWER(module, chip, chip_version, board)  \
    lpower_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define LPOWER_VSB(module, chip, chip_version, board)  \
    lpower_vsb_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define LPOWER_FPGA(module, chip, chip_version, board)  \
    lpower_attach##_##chip(#module"_"#chip)

/*!
  comment
  */
#define CHARSTO_FPGA(module, chip, chip_version, board)  \
    charsto_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define CHARSTO(module, chip, chip_version, board)  \
    charsto_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define CHARSTO_SIM(module, chip, chip_version, board)  \
    charsto_sim_attach(#module"_"#chip)


/*!
    WIN32 Scart

   \param[in] name
  */
extern RET_CODE win32_scart_attach(char *name);

/*!
    MAGIC Scart

   \param[in] name
  */
extern RET_CODE magic_scart_attach(char *name);
/*!
    FPGA Scart

   \param[in] p_name
  */
extern RET_CODE magic_scart_fpga_attach(char *p_name);

/*!
   Attach the drivers to win32 SCART device

   \param[in] name
  */
static inline RET_CODE scart_sim_attach_magic(char *name)
    {return win32_scart_attach(name);}
/*!
   Attach the drivers to win32 SCART device

   \param[in] name
  */
static inline RET_CODE scart_sim_attach_wizards(char *name)
    {return win32_scart_attach(name);}
/*!
   Attach the drivers to Magic SCART device on chip

   \param[in] name
  */
static inline RET_CODE scart_attach_magic(char *name)
    {return magic_scart_attach(name);}
/*!
   Attach the drivers to GMI wizards SCART device on chip

   \param[in] name
  */
static inline RET_CODE scart_attach_wizards(char *name)
    {return magic_scart_attach(name);}
/*!
   Attach the drivers to Magic SCART device on FPGA board

   \param[in] name
  */
static inline RET_CODE scart_fpga_attach_magic(char *name)
    {return magic_scart_fpga_attach(name);}        

/*!
  SCART driver attachment for FPGA board
  */
#define SCART_FPGA(module, chip, chip_version, board)  \
    scart_fpga_attach##_##chip(#module"_"#chip)
/*!
  SCART driver attachment for chip board
  */
#define SCART(module, chip, chip_version, board)  \
    scart_attach##_##chip(#module"_"#chip)
/*!
  SCART driver attachment for simulator
  */
#define SCART_SIM(module, chip, chip_version, board)  \
    scart_sim_attach##_##chip(#module"_"#chip)


/*!
  RF

  \param[out] name
  */
extern RET_CODE win32_rf_attach(char * name);
/*!
  RF

  \param[out] name
  */
extern RET_CODE magic_rf_attach(char * name);

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE rf_sim_attach_magic(char *name)
    {return win32_rf_attach(name);}
/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE rf_attach_magic(char *name)
    {return magic_rf_attach(name);}

/*!
  comment
  */
#define RF_FPGA(module, chip, chip_version, board)  \
    rf_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define RF(module, chip, chip_version, board)  \
    rf_attach##_##chip(#module"_"#chip)
/*!
  comment
  */
#define RF_SIM(module, chip, chip_version, board)  \
    rf_sim_attach##_##chip(#module"_"#chip)


/*!
  nim

  \param[out] name
  */
extern RET_CODE nim_win32_attach(char *name);
/*!
  nim

  \param[out] name
  */
extern RET_CODE nim_m88cs2200_attach(char *name);
/*!
  nim

  \param[out] name
  */
extern RET_CODE nim_m88cs2000_attach(char *name);
/*!
  nim

  \param[out] name
  */
extern RET_CODE nim_m88rs2k_attach(char *name);
/*!
  nim

  \param[out] name
  */
extern RET_CODE nim_m88ds3000_attach(char *name);
/*!
  nim

  \param[out] name
  */
extern RET_CODE nim_m88ds3000_attach(char *name);
/*!
  nim

  \param[out] name
  */
extern RET_CODE nim_m88dc2800_attach(char *name);

/*!
  nim

  \param[out] name
  */  
#ifdef MDA009A
/*!
   TODO: fix me!

   \param[in] name
  */
extern RET_CODE nim_m88da3100_attach(char *name);
#else
/*!
   TODO: fix me!

   \param[in] name
  */
extern RET_CODE nim_cx24485_attach(char *name);
#endif

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE nim_sim_attach(char *name)
    {return nim_win32_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE nim_attach_m88cs2200(char *name)
    {return nim_m88cs2200_attach(name);}

static inline RET_CODE nim_attach_m88cs2000(char *name)
    {return nim_m88cs2000_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE nim_attach_m88rs2k(char *name)
    {return nim_m88rs2k_attach(name);}


/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE nim_attach_m88dc2800(char *name)
    {return nim_m88dc2800_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE nim_attach_m88ds3000(char *name)
    {return nim_m88ds3000_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE nim_attach_magic(char *name)
    {return nim_m88cs2200_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE nim_attach_wizards(char *name)
    {return nim_m88cs2000_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE nim_attach_sonata(char *name)
    {return nim_m88ds3000_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
static inline RET_CODE nim_attach_concerto(char *name)
    {return nim_m88dc2800_attach(name);}

/*!
   TODO: fix me!

   \param[in] name
  */
#if ARCH == ARM_ADS 
#ifdef MDA009A
static inline RET_CODE nim_attach_nxp(char *name)
    {return nim_m88da3100_attach(name);}
#else
static inline RET_CODE nim_attach_nxp(char *name)
    {return nim_cx24485_attach(name);}
#endif
#endif


/*!
  comment
  */
#define NIM(module, chip, chip_version, board)  \
    nim_attach##_##chip(#module"_"#chip)

/*!
  comment
  */
#define NIM_FPGA(module, chip, chip_version, board)  \
    nim_attach##_##chip(#module"_"#chip)
    
/*!
  comment
  */
#define NIM_SIM(module, chip, chip_version, board)  \
    nim_sim_attach(#module"_"#chip)


#endif //__DRIVER_H__


