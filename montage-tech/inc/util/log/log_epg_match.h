/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MATCH_H_
#define __MATCH_H_
/*!
  compare event
*/
typedef struct evt
{
/*! 
  Event name
*/
  u8 event_name[34];
/*!
  Event id
*/
  u16 event_id;
/*!
  Event start time
*/
  utc_time_t start_time;
/*!
  Event duration
*/
  utc_time_t drt_time;

} comp_evt;
 
comp_evt arr107e[20] = {
            {"Now You're Talki",0x5041,{2004,3,23,9,30,0},{0,0,0,0,30,0}},
            {"Money Spinners",0x5042,{2004,3,23,10,0,0},{0,0,0,1,0,0}},
            {"House Invaders",0x5043,{2004,3,23,11,0,0},{0,0,0,0,30,0}},

            {"Disturbing Behav",0x507a,{2004,3,24,0,5,0},{0,0,0,1,15,0}},
            {"Weather View",0x507b,{2004,3,24,1,20,0},{0,0,0,0,5,0}},
            {"This is BBC ONE",0x507c,{2004,3,24,1,25,0},{0,0,0,0,15,0}},

            {"Brannigan",0x50c8,{2004,3,25,0,5,0},{0,0,0,1,50,0}},
            {"SAS Desert: Are ",0x56ac,{2004,3,25,2,0,0},{0,0,0,1,0,0}},
            {"Breakfast",0x5290,{2004,3,25,6,0,0},{0,0,0,3,30,0}},

            {"Tweenies",0x5508,{2004,3,26,15,25,0},{0,0,0,0,20,0}},
            {"Arthur",0x5509,{2004,3,26,15,45,0},{0,0,0,0,15,0}},
            {"ChuckleVision",0x550a,{2004,3,26,16,0,0},{0,0,0,0,15,0}},

            {"Casualty",0x564f,{2004,3,27,19,0,0},{0,0,0,0,50,0}},
            {"Six Nations Gran",0x5650,{2004,3,27,19,50,0},{0,0,0,2,5,0}},
            {"The National Lot",0x5651,{2004,3,27,21,55,0},{0,0,0,0,30,0}},

            {"Breakfast with F",0x58c8,{2004,3,28,8,0,0},{0,0,0,1,0,0}},
            {"The Heaven and E",0x58c9,{2004,3,28,9,0,0},{0,0,0,1,0,0}},
            {"Countryfile",0x58ca,{2004,3,28,10,0,0},{0,0,0,1,0,0}},

            {"Rugrats",0x5937,{2004,3,29,15,20,0},{0,0,0,0,15,0}},
            {"BAMZOOKi",0x5938,{2004,3,29,15,35,0},{0,0,0,0,10,0}},
};
#endif
