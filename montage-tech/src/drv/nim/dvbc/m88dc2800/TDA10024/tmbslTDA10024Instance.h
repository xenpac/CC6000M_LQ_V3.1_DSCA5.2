/**
  Copyright (C) 2006 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA10024Instance.h
 *                %version: CFR_STB#2 %
 *
 * \date          %date_modified%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *
 * Detailed description may be added here.
 *
 * \section info Change Information
 *
 * \verbatim
   Date          Modified by CRPRNr  TASKNr  Maintenance description
   -------------|-----------|-------|-------|-----------------------------------
   26-Nov-2007  | V.VRIGNAUD|       |       | COMPATIBILITY WITH NXPFE
                | C.CAZETTES|       |       | 
   -------------|-----------|-------|-------|-----------------------------------
                |           |       |       |
   -------------|-----------|-------|-------|-----------------------------------
   \endverbatim
 *
*/
#ifndef _TMBSLTDA10024_INSTANCE_H
#define _TMBSLTDA10024_INSTANCE_H

tmErrorCode_t TDA10024AllocInstance (tmUnitSelect_t tUnit, pptmTDA10024Object_t ppDrvObject);
tmErrorCode_t TDA10024DeAllocInstance (tmUnitSelect_t tUnit);
tmErrorCode_t TDA10024GetInstance (tmUnitSelect_t tUnit, pptmTDA10024Object_t ppDrvObject);

#endif /* _TMBSLTDA10024_INSTANCE_H */

