/*******************************************************************************
 ** 
 ** Copyright (c) 2004, S.Moben Technology Corp.
 ** All rights reserved.
 ** 
 ** Document Name: SM_StateMachine.h
 ** Abstract     : 
 ** 
 ** Current Ver  : V0.01
 ** Author       : yuliangzhong
 ** Date  		 : 2006/08/04
 ** 
 ** Modification Histroy£º
 ** (V0.01) yuliangzhong@2006.08.04: create this file
 ** 
 ******************************************************************************/
#ifndef _SM_STATEMACHINE_H_
#define _SM_STATEMACHINE_H_

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 

#define STATE_TREE_MAX_DEPTH 10
#define SID_NULL (0)
#define SID_ROOT (1)
#define SID_INTERNAL (0xFFFF)
#define SM_MODULE_NAME_MAX_LEN 20

typedef u16 STATEID; //State ID
typedef u16 MSGID; //Message ID
typedef void* PVOID;

typedef struct tagSTATE_TRANS
{
	STATEID dstStateID;
	MSGID   msgID;
	BOOL    (*TransCondition)(PVOID, MSGID, u32, u32); //NULL: will process this message. Return: FALSE-will not process, TRUE-will process
	STATEID (*TransAction)(PVOID, MSGID, u32, u32); //Return: if NOT 0 and dstStateID is SID_INTERNAL, will transit to this state node after returned.
} STATE_TRANS_T;

typedef struct tagSTATE_NODE
{
	STATEID               parentID;
	STATEID               firstChildID;
	void                  (*Entry)(void);
	void                  (*Exit)(void);
	const STATE_TRANS_T * pTransTab;
	u8                 transCount;
} STATE_NODE_T;

typedef struct tagSTATE_TREE_INFO
{
	const STATE_NODE_T * pStateTree;
	u8                 pModuleName[SM_MODULE_NAME_MAX_LEN];
	STATEID              maxStateID;
	u16               activeDepth;
	STATEID              activeStateID[STATE_TREE_MAX_DEPTH];
} STATE_TREE_INFO_T;

extern BOOL SM_OpenStateTree(STATE_TREE_INFO_T * pStateTreeInfo);
extern BOOL SM_CloseStateTree(STATE_TREE_INFO_T * pStateTreeInfo);
extern BOOL SM_IsStateActive(STATE_TREE_INFO_T * pStateTreeInfo, STATEID stateID);
extern BOOL SM_IsStateInactive(STATE_TREE_INFO_T * pStateTreeInfo, STATEID stateID);
extern s32 SM_DispatchMsg(STATE_TREE_INFO_T * pStateTreeInfo, control_t *ctrl, u16 msg, u32 para1, u32 para2);

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif /*_SM_STATEMACHINE_H_*/
