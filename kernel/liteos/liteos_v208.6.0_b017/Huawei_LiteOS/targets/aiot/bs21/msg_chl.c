/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: msg adapter \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-10, Create file. \n
 */
#include "msg_chl.h"
#include "string.h"
#include "los_queue_pri.h"
#include "los_memory.h"
#include "los_hwi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MSG_CHL_NODE_MAX_NUM   40
LOS_MSG_CHL_CTL_T g_stMsgChlList;

/*****************************************************************************
 Function    : LOS_MsgChlInit
 Description : inilization the Message channel
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
VOID LOS_MsgChl_Init(VOID)
{
    static BOOL bInited = FALSE;

    if (bInited) {
        return;
    }

    LOS_ListInit(&(g_stMsgChlList.stList));
    g_stMsgChlList.pcChlName = NULL;
    g_stMsgChlList.MsgChlID = NULL;
    bInited = TRUE;

    return;
}

/*****************************************************************************
 Function    : LOS_MsgChl_Open
 Description : Open a message channel by name
 Input       : const CHAR *pcMsgChlName
 Output      : None
 Return      : osMsgChlId_t or NULL
 *****************************************************************************/
osMsgChlId_t LOS_MsgChl_Open(const CHAR *pcMsgChlName)
{
    osMsgChlId_t MsgChlID;
    UINT32 queueId = 0;
    LOS_MSG_CHL_CTL_T *pstChlItem = NULL;

    MsgChlID = LOS_MsgChl_GetChlbyName(pcMsgChlName);
    if (MsgChlID != NULL) {
        return MsgChlID;
    }

    if (LOS_QueueCreate(NULL, MSG_CHL_NODE_MAX_NUM, &queueId, 0, sizeof(UINT32)) != LOS_OK) {
        /* queue support 40 msgs and each msg size is 4 bytes */
        PRINT_ERR("LOS_MsgChl_Open: LOS_QueueCreate failed!\r\n");
        return NULL;
    }

    PRINT_ERR("LOS_QueueCreate queueId %d\n", queueId);

    MsgChlID = (osMsgChlId_t)GET_QUEUE_HANDLE(queueId);
    pstChlItem = (LOS_MSG_CHL_CTL_T *)LOS_MemAlloc(OS_SYS_MEM_ADDR, sizeof(LOS_MSG_CHL_CTL_T));
    if (pstChlItem == NULL) {
        LOS_QueueDelete(queueId);
        PRINT_ERR("LOS_MsgChl_Open: malloc size %ld failed!\r\n", sizeof(LOS_MSG_CHL_CTL_T));
        return NULL;
    }
    pstChlItem->MsgChlID = MsgChlID;
    pstChlItem->pcChlName = (CHAR *)pcMsgChlName;

    LOS_ListAdd(&(g_stMsgChlList.stList), &(pstChlItem->stList));

    return MsgChlID;
}

/*****************************************************************************
 Function    : LOS_MsgChl_Write
 Description : write data to message channel
 Input       : osMsgChlId_t MsgChlID: Message Channel ID get by call
               VOID         *pData  : Data to write in MsgChl; The memory of Data must be malloc by Write.
               UINT32 timeout       : ms. osNoWait/osWaitForever write timeout time
 Output      : None
 Return      : LOS_OK: SUCCESS; LOS_NOK: FAIL
 *****************************************************************************/
UINT32 LOS_MsgChl_Write(osMsgChlId_t MsgChlID, VOID *pData, UINT32 timeout)
{
    UINT32 uwRet;
    UINT32 uwBufferSize;
    UINT32 ulData = (UINT32)(uintptr_t)pData;
    LosQueueCB *pstQueue = (LosQueueCB *)MsgChlID;

    if ((pstQueue == NULL) || (pData == NULL) || ((OS_INT_ACTIVE) && (timeout != 0))) {
        return LOS_ERRNO_QUEUE_READ_IN_INTERRUPT;
    }

    uwBufferSize = (UINT32)pstQueue->queueSize;
    uwRet = LOS_QueueWriteCopy((UINT32)pstQueue->queueId, (VOID*)&ulData, uwBufferSize, timeout);
    if (uwRet != LOS_OK) {
        PRINT_ERR("osMessageQueuePut put queue failed,errno 0x%x \n", uwRet);
    }

    return uwRet;
}

/*****************************************************************************
 Function    : LOS_MsgChl_Read
 Description : read data from a messaeg channel.
 Input       : osMsgChlId_t     MsgChlID: Message Channel ID get by call
               VOID         **ppMsgBuf  : the address of the point to save the buff address,
                                          the buff memory must be free by reader.
               UINT32 timeout           : ms. osNoWait/osWaitForever/read timeout time
 Output      : None
 Return      : osOK: SUCCESS; osErrorTimeout: Timeout; osErrorParameter, wrong parameter
 *****************************************************************************/
UINT32 LOS_MsgChl_Read(osMsgChlId_t MsgChlID, VOID **ppMsgBuf, UINT32 timeout)
{
    UINT32 ulData = 0;
    UINT32 uwRet;
    UINT32 uwBufferSize;
    LosQueueCB *pstQueue = (LosQueueCB *)MsgChlID;

    if ((pstQueue == NULL) || (ppMsgBuf == NULL) || ((OS_INT_ACTIVE) && (timeout != 0))) {
        return LOS_ERRNO_QUEUE_READ_IN_INTERRUPT;
    }

    uwBufferSize = (UINT32)pstQueue->queueSize;
    uwRet = LOS_QueueReadCopy((UINT32)pstQueue->queueId, &ulData, &uwBufferSize, timeout);
    if (uwRet != LOS_OK) {
        return uwRet;
    }

    *ppMsgBuf = (VOID *)(uintptr_t)ulData;

    return LOS_OK;
}

/*****************************************************************************
 Function    : LOS_MsgChl_Delete
 Description : Delete a message channle
 Input       : osMsgChlId_t MsgChlID: Message Channel ID get by call LOS_MsgChl_Open or LOS_MsgChl_GetChlbyName
 Output      : None
 Return      : None
 *****************************************************************************/
VOID LOS_MsgChl_Delete(osMsgChlId_t MsgChlID)
{
    LosQueueCB *pstQueue = (LosQueueCB *)MsgChlID;
    LOS_MSG_CHL_CTL_T *pstChlTmp = NULL;
    BOOL bFind = FALSE;

    LOS_DL_LIST_FOR_EACH_ENTRY(pstChlTmp, &(g_stMsgChlList.stList), LOS_MSG_CHL_CTL_T, stList) { /*lint !e413*/
        if (MsgChlID == pstChlTmp->MsgChlID) {
            bFind = TRUE;
            break;
        }
    }

    if (bFind) {
        (VOID)LOS_QueueDelete((UINT32)pstQueue->queueId);
        LOS_ListDelete(&(pstChlTmp->stList));
        LOS_MemFree(OS_SYS_MEM_ADDR, pstChlTmp);
    }

    return;
}

/*****************************************************************************
 Function    : LOS_MsgChl_GetChlbyName
 Description : Get a message channle by name
 Input       : const char *pcMsgChlName: name of message channel.
 Output      : None
 Return      : osMsgChlId_t :SUCCESS; NULL: FAIL
 *****************************************************************************/
osMsgChlId_t LOS_MsgChl_GetChlbyName(const char *pcMsgChlName)
{
    LOS_MSG_CHL_CTL_T *pstChlTmp = NULL;

    if (LOS_ListEmpty(&(g_stMsgChlList.stList))) {
        return NULL;
    }

    LOS_DL_LIST_FOR_EACH_ENTRY(pstChlTmp, &(g_stMsgChlList.stList), LOS_MSG_CHL_CTL_T, stList) { /*lint !e413*/
        if (strncmp(pstChlTmp->pcChlName, pcMsgChlName, LOS_MSG_CHL_NAME_MAX_LEN) == 0) {
            return pstChlTmp->MsgChlID;
        }
    }

    return NULL;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cpluscplus */
