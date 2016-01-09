/*******************************************************************************
 ** 
 ** Copyright (c) 2004, S.Moben Technology Corp.
 ** All rights reserved.
 ** 
 ** Document Name: SM_StateMachine.c
 ** Abstract     : MMI state machine
 ** 
 ** Current Ver  : V0.01
 ** Author       : yuliangzhong
 ** Date  		 : 2006/01/25
 ** 
 ** Modification Histroy£º
 ** (V0.01) yuliangzhong@2006.01.25: create this file
 ** 
 ******************************************************************************/
#include "ui_common.h"
#include "SM_StateMachine.h"
#include "sys_define.h"

#define SM_TRACE    OS_PRINTF

//#define SM_TRACE(...)    

static u16 SmGetStatesChain(
	STATE_TREE_INFO_T * pStateTreeInfo, 
	STATEID nDstStateID, 
	STATEID * pStatesChain)
{
	s32 i;
	const STATE_NODE_T *pStateTree = pStateTreeInfo->pStateTree;
	u16 depth = 0;
	STATEID parentStateID = SID_NULL;
	STATEID childStateID = SID_NULL;

	MT_ASSERT((nDstStateID>SID_NULL) && (nDstStateID<pStateTreeInfo->maxStateID));

	//Search default children to bottom
	parentStateID = nDstStateID;
	for (i=0; i<STATE_TREE_MAX_DEPTH; i++)
	{
		childStateID = pStateTree[parentStateID].firstChildID;
		if (SID_NULL==childStateID)
		{
			break;
		}
		parentStateID = childStateID;
	}
	//Search from bottom to root
	childStateID = parentStateID;
	for (i=0, depth=0; i<STATE_TREE_MAX_DEPTH; i++)
	{
		pStatesChain[depth++] = childStateID;
		parentStateID = pStateTree[childStateID].parentID;
		if (SID_NULL==parentStateID)
		{
			break;
		}
		childStateID = parentStateID;
	}
	for (++i; i<STATE_TREE_MAX_DEPTH; i++)
	{
		pStatesChain[i] = SID_NULL;
	}

	//Reverse the array
	for (i=0; i<depth/2; i++)
	{
		childStateID = pStatesChain[i];
		pStatesChain[i] = pStatesChain[depth - i - 1];
		pStatesChain[depth - i - 1] = childStateID;
	}

	return depth;
}

static BOOL SmDoTransition(
	STATE_TREE_INFO_T * pStateTreeInfo, 
	STATEID nDstStateID)
{
	s32 i;
	const STATE_NODE_T *pStateTree = pStateTreeInfo->pStateTree, *pStateNode;
	u16 sameParentDepth = 0;
	u16 curDepth, dstDepth;
	STATEID dstStates[STATE_TREE_MAX_DEPTH];

	MT_ASSERT(nDstStateID>SID_NULL);
	MT_ASSERT(nDstStateID<pStateTreeInfo->maxStateID);
	SM_TRACE("SM Transiting...: %s", pStateTreeInfo->pModuleName);

	//Get state chain of destination, and its depth
	dstDepth = SmGetStatesChain(pStateTreeInfo, nDstStateID, dstStates);
	curDepth = pStateTreeInfo->activeDepth;

	//Get the fork of these two state chain
	for (i=0; (i<curDepth) && (i<dstDepth); i++)
	{
		if (pStateTreeInfo->activeStateID[i]!=dstStates[i])
		{ //Their parent is the fork!
			break;
		}
		if (nDstStateID==dstStates[i])
		{ //Treat their parent as the fork!
			break;
		}
		sameParentDepth = (u16)(i + 1);
	}

	//Exit current states
	for (i=curDepth-1; i>=sameParentDepth; i--)
	{
		pStateNode = &pStateTree[pStateTreeInfo->activeStateID[i]];
		SM_TRACE("SM Exiting state: %d", pStateTreeInfo->activeStateID[i]);
		if (NULL!=pStateNode->Exit)
		{
			pStateNode->Exit();
		}
		SM_TRACE("SM Exited state: %d", pStateTreeInfo->activeStateID[i]);
		pStateTreeInfo->activeStateID[i] = SID_NULL;
		pStateTreeInfo->activeDepth--;
	}
	//Enter new states
	for (i=sameParentDepth; i<dstDepth; i++)
	{
		pStateTreeInfo->activeStateID[i] = dstStates[i];
		pStateTreeInfo->activeDepth++;
		pStateNode = &pStateTree[dstStates[i]];
		SM_TRACE("SM Entering state: %d", pStateTreeInfo->activeStateID[i]);
		if (NULL!=pStateNode->Entry)
		{
			pStateNode->Entry();
		}
		SM_TRACE("SM Entered state: %d", pStateTreeInfo->activeStateID[i]);
	}

	SM_TRACE("SM Transited...: %s", pStateTreeInfo->pModuleName);
	return TRUE;
}

BOOL SM_OpenStateTree(STATE_TREE_INFO_T * pStateTreeInfo)
{
	s32 i;

	MT_ASSERT(NULL!=pStateTreeInfo);
	MT_ASSERT(NULL!=pStateTreeInfo->pStateTree);
	MT_ASSERT(0==pStateTreeInfo->activeDepth);

	SM_TRACE("SM Opening...: %s", pStateTreeInfo->pModuleName);

	for (i=0; i<STATE_TREE_MAX_DEPTH; i++)
	{
		pStateTreeInfo->activeStateID[i] = SID_NULL;
	}

	return SmDoTransition(pStateTreeInfo, SID_ROOT);
}

BOOL SM_CloseStateTree(STATE_TREE_INFO_T * pStateTreeInfo)
{
	s32 i;
	const STATE_NODE_T *pStateTree, *pStateNode;

	MT_ASSERT(NULL!=pStateTreeInfo);
	MT_ASSERT(NULL!=pStateTreeInfo->pStateTree);
	MT_ASSERT(0!=pStateTreeInfo->activeDepth);

	SM_TRACE("SM Closing...: %s", pStateTreeInfo->pModuleName);
	pStateTree = pStateTreeInfo->pStateTree;

	//Exit current states
	for (i=pStateTreeInfo->activeDepth-1; i>=0; i--)
	{
		pStateNode = &pStateTree[pStateTreeInfo->activeStateID[i]];
		SM_TRACE("SM Exiting state: %d", pStateTreeInfo->activeStateID[i]);
		if (NULL!=pStateNode->Exit)
		{
			pStateNode->Exit();
		}
		SM_TRACE("SM Exited state: %d", pStateTreeInfo->activeStateID[i]);
		pStateTreeInfo->activeStateID[i] = SID_NULL;
		pStateTreeInfo->activeDepth--;
	}
	
	return 1;
}

BOOL SM_IsStateActive(
	STATE_TREE_INFO_T * pStateTreeInfo, 
	STATEID stateID)
{
	s32 i;

	MT_ASSERT(NULL!=pStateTreeInfo);
	MT_ASSERT(NULL!=pStateTreeInfo->pStateTree);
	MT_ASSERT(0!=pStateTreeInfo->activeDepth);

	for (i=0; i<pStateTreeInfo->activeDepth; i++)
	{
		if (pStateTreeInfo->activeStateID[i]==stateID)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL SM_IsStateInactive(
	STATE_TREE_INFO_T * pStateTreeInfo, 
	STATEID stateID)
{
	s32 i;

	MT_ASSERT(NULL!=pStateTreeInfo);
	MT_ASSERT(NULL!=pStateTreeInfo->pStateTree);
	MT_ASSERT(0!=pStateTreeInfo->activeDepth);

	for (i=0; i<pStateTreeInfo->activeDepth; i++)
	{
		if (pStateTreeInfo->activeStateID[i]==stateID)
		{
			return FALSE;
		}
	}

	return TRUE;
}

s32 SM_DispatchMsg(STATE_TREE_INFO_T * pStateTreeInfo, control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
	s32 i, j;
	s32 result = ERR_NOFEATURE; //The message has NOT been processed
	const STATE_NODE_T *pStateTree, *pStateNode;
	const STATE_TRANS_T *pStateTransTab, *pStateTrans;
	STATEID curStateID = SID_NULL;
	STATEID nextStateID = SID_NULL;
	MSGID nMsgID = 0;

	MT_ASSERT(NULL!=pStateTreeInfo);
	MT_ASSERT(NULL!=pStateTreeInfo->pStateTree);
	MT_ASSERT(0!=pStateTreeInfo->activeDepth);

	SM_TRACE("SM Dispatching MSG %d to MODULE %s", msg, pStateTreeInfo->pModuleName);
	pStateTree = pStateTreeInfo->pStateTree;
	nMsgID = msg;

	//Check each state node from top to bottom
	//for (i=0; i<pStateTreeInfo->activeDepth; i++)
	//Check each state node from bottom to top
	for (i=pStateTreeInfo->activeDepth-1; i>=0; i--)
	{
		SM_TRACE("SM Parsing in state: %d", pStateTreeInfo->activeStateID[i]);
		curStateID = pStateTreeInfo->activeStateID[i];
		if ((SID_NULL==curStateID)
			|| (curStateID>=pStateTreeInfo->maxStateID)
			|| (NULL==(pStateNode=&pStateTreeInfo->pStateTree[curStateID])))
		{ //Error
			break;
		}
		if (NULL==(pStateTransTab=pStateNode->pTransTab))
		{ //No transition table
			continue;
		}

		//Check each transition in the transition table of this node
		for (j=0; j<pStateNode->transCount; j++)
		{
			pStateTrans = &pStateTransTab[j];

			if (pStateTrans->msgID==nMsgID) //Message ID matched, then check transit condition and do transition
			{
				SM_TRACE("SM Msg matched in state %d at pos %d", pStateTreeInfo->activeStateID[i], j);
				if ((NULL!=pStateTrans->TransCondition) 
					&& (FALSE==pStateTrans->TransCondition(ctrl, msg, para1, para2)))
				{ //It has transit condition, but the condition is FALSE
					continue;
				}

				SM_TRACE("SM Msg processing in state %d at pos %d", pStateTreeInfo->activeStateID[i], j);
				result = SUCCESS; //The message has been processed

				if (NULL!=pStateTrans->TransAction)
				{ //Process its transit action
					SM_TRACE("SM Msg action...MODULE: %s", pStateTreeInfo->pModuleName);
					nextStateID = pStateTrans->TransAction(ctrl, msg, para1, para2);
					SM_TRACE("SM Msg action OK...MODULE: %s", pStateTreeInfo->pModuleName);
				}

				SM_TRACE("SM destState: %d, but nextState: %d", pStateTrans->dstStateID, nextStateID);
				if (SID_INTERNAL==pStateTrans->dstStateID)
				{
					if (SID_NULL==nextStateID)
					{
						return result;
					}
					if ((SID_NULL!=nextStateID) && (nextStateID<pStateTreeInfo->maxStateID))
					{ //Transit to the RETURNed state, and return directly
						SM_TRACE("SM Msg INTERNAL jumping...next state: %d", nextStateID);
						SmDoTransition(pStateTreeInfo, nextStateID);
						SM_TRACE("SM Msg INTERNAL jumped...next state: %d", nextStateID);
						return result;
					}
					else
					{
						continue;
					}
				}
				else
				{
					nextStateID = pStateTrans->dstStateID;
					if ((SID_NULL!=nextStateID) && (nextStateID<pStateTreeInfo->maxStateID))
					{ //We will not check any more and return directly
						SM_TRACE("SM Msg JUMPing...next state: %d", nextStateID);
						SmDoTransition(pStateTreeInfo, nextStateID);
						SM_TRACE("SM Msg JUMPed...next state: %d", nextStateID);
					}
					return result;
				}
			}
		}
	}

	return result;
}

