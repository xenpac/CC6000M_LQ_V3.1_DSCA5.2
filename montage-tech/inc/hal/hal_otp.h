/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __HAL_OTP_H__
#define __HAL_OTP_H__

/*!
  comments
  */
enum otp_region_e
{
  /*!
    comments
    */
  E_OTP_REGION_1,
  /*!
    comments
    */
  E_OTP_REGION_2,
  /*!
    comments
    */
  E_OTP_REGION_3,
  /*!
    comments
    */
  E_OTP_REGION_4,
  /*!
    comments
    */
  E_OTP_CTRL_REGION,
  /*!
    comments
    */
  E_OTP_REGION_1_2_3_4
};

/*!
  comments
  */
enum otp_ioctrl_option
{
  /*!
    comments
    */
  E_OTP_IOC_READONLY,
  /*!
    comments
    */
  E_OTP_IOC_WRITEONLY,
  /*!
    comments
    */
  E_OTP_IOC_WR,
  /*!
    comments
    */
  E_OTP_IOC_WR_UNALLOW
};
/*!
  * get property cmd define 
  */
typedef enum
{
  CMD_GET_PROPERTY_HD_EN  =0,
  CMD_GET_DRIVER_VERSION    ,
  CMD_GET_DOLBY_EN          ,
  CMD_GET_CUSTOMER_ID       ,
  /*!
    * *p_result : 
    *   0 - DRA disable
    *   1 - DRA enable
    */
  CMD_GET_DRA_EN            ,
  /*!
    * *p_result : 
    *   0 - DTMB disable
    *   1 - DTMB enable
    */
  CMD_GET_DTMB_EN           ,
  CMD_OTP_MAX               ,
}cmd_otp_property_e_t;

/*!
  * CMD_GET_CUSTOMER_ID
  * customer id   
  */
typedef enum 
{
  E_CUST_ID_THINEWTEC       = 0   ,
  E_CUST_ID_CYCLE                 ,
  E_CUST_ID_RESERVED              ,
}property_cust_id_t;
/*!
  * property of hdenable
  */
typedef enum
{
  /*!
   *  HD DISABLE 
   */
  SD_HD_SOLUTION_HD_DISABLE    = 0  ,   
  /*!
    * HD ENABLE 
    */
  SD_HD_SOLUTION_HD_EN              ,  
  /*! 
    *HD TYPE ERROR , there must something wrong 
    */
  SD_HD_SOLUTION_ERROR              ,  
}property_hd_type_e_t;
/*!
  comments
  */
s32 hal_otp_read(u8 region, u16 *p_data);
/*!
  comments
  */
s32 hal_otp_write(u8 region, u16 data);
/*!
  comments
  */
void hal_otp_init(void);
/*!
  comments
  */
s32 hal_otp_ioctrl(u8 opt, u8 region);
/*!
  comments
  */
void hal_otp_get_chip_id(char * p_chipid);
/*!
  comments
  */
s32 hal_otp_read_random(u32 addr, u8 len, u32 *p_result);
/*!
  comments
  * input:
    * cmd : cmd_otp_property_e_t 
  * output:
    * p_result :  the result output
  */
s32 hal_otp_get_property(u32 cmd, u32 *p_result);
/*!
  comments
  * p_chipid_h : the address of high 32bits for chipid  
  * p_chipid_l : the address of low 32bits for chipid
  */
s32 hal_otp_get_chipid(u32 *p_chipid_h, u32 *p_chipid_l);
/*!
  comments
  * p_pid_h : the address of high 32bits for pid  
  * p_pid_l : the address of low 32bits for pid
  */
s32 hal_otp_get_pid(u32 *p_pid_h, u32 *p_pid_l);
/*!
  comments
  * p_cpd : the address of cpd value
  */
s32 hal_otp_get_cpd(u32 *p_cpd);
#endif //__HAL_OTP_H__
