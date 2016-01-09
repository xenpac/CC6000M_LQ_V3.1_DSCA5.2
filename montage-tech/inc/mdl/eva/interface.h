/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __INTERFACE_H_
#define __INTERFACE_H_

/*!
  1. if some member defined as FATHER, means this class is bass class.
  Must be put at head!
  Must be put at head!!
  Must be put at head!!!
  Must be put at head!!!!
  */
#define FATHER

/*!
  2. if some API defined as PRIVATE, means you can't overload it and can't call it
  can't overload it and can't call it!
  can't overload it and can't call it!!
  [incantation] you can't see me.
  [incantation] you can't see me..
  [incantation] you can't see me...
  */
#define PRIVATE

/*!
  3. if some API defined as VIRTUAL, means you can overload by your requirement.
  */
#define VIRTUAL

/*!
  4. if some API defined as PURE VIRTUAL, means you must overload it, 
  the bass class will call it.
  */
#define PURE

/*!
  5. if some API not have any title, means it's normal API.
  you can call it but can't overload it.
  */

/*!
  the interface max name length 
  */
#define I_NAME_LEN (15)

/*!
  class ID
  */
typedef enum
{
  /*!
    error id
    */
  CID_UNKNOWN,
  /*!
    buffer sample calss id
    */
  CID_SAMPLE,
  /*!
    buffer queue calss id
    */
  CID_BUFFER_QUEUE,
  /*!
    buffer pool calss id
    */
  CID_BUFFER_POOL,
  /*!
    filter calss id
    */
  CID_IFILTER,
  /*!
    srource filter calss id
    */
  CID_SRC_FILTER,
  /*!
    transfer filter calss id
    */
  CID_TRS_FILTER,
  /*!
    sink filter calss id
    */
  CID_SINK_FILTER,
  /*!
    ipin calss id
    */
  CID_IPIN,
  /*!
    input ipin calss id
    */
  CID_INPUT_PIN,
  /*!
    input ipin calss id
    */
  CID_SINK_PIN,
  /*!
    transfer input pin calss id
    */
  CID_TRANSF_INPUT_PIN,
  /*!
    output ipin calss id
    */
  CID_OUTPUT_PIN,
  /*!
    source pin calss id
    */
  CID_SOURCE_PIN,
  /*!
    transfer output pin calss id
    */
  CID_TRANSF_OUTPUT_PIN,
  /*!
    chain calss id
    */
  CID_CHAIN,
  /*!
    controller calss id
    */
  CID_CONTROLLER,
  /*!
    max calss id
    */
  CID_END
}CLASS_ID;

/*!
  name define
  */
typedef struct tag_interface_id
{
  /*!
    class id
    */
  CLASS_ID c_id;
  /*!
    instance id
    */
  u32 id;
  /*!
    instance name, the more one char is '0';
    */
  char name[I_NAME_LEN + 1];
}interface_id_t;

/*!
  interface define
  */
typedef struct tag_interface
{
  /*!
    instance id
    */
  interface_id_t iid;
  
  /*!
    register to eva, note can't overload it
    */
  PRIVATE void (*_register)(handle_t _this, CLASS_ID c_id, char *p_name);
  
  /*!
    register to eva, note can't overload it
    */
  PRIVATE void (*_rename)(handle_t _this, char *p_name);

  /*!
    destroy instance, note can't overload it
    */
  PRIVATE RET_CODE (*_destroy)(handle_t _this);

  // **************************************************************************
  // * Below function are pure virtual function
  // **************************************************************************
  
  /*!
    when the instance be destroyed, system will call this function. so if your
    class need to free some resource, you can implement this function.
    */
  VIRTUAL void (*on_destroy)(handle_t _this);

  /*!
    for report info.
    */
  PURE VIRTUAL void (*report)(handle_t _this, void *p_state);

}interface_t;

#endif // End for __INTERFACE_H_

