/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Sort Link
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include "los_sortlink_pri.h"
#include "los_memory.h"
#include "los_exc.h"
#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
#include "los_task_pri.h"
#include "los_swtmr_pri.h"
#endif

#define OS_INVALID_VALUE    0xFFFFFFFF

#ifdef LOSCFG_BASE_CORE_USE_MULTI_LIST
LITE_OS_SEC_TEXT_INIT VOID OsSortLinkInit(SortLinkAttribute *sortLinkHeader, LOS_DL_LIST *list)
{
    UINT32 index;
    LOS_DL_LIST *listObject = list;

    sortLinkHeader->sortLink = listObject;
    sortLinkHeader->cursor = 0;
    for (index = 0; index < OS_TSK_SORTLINK_LEN; index++, listObject++) {
        LOS_ListInit(listObject);
    }
}

LITE_OS_SEC_TEXT VOID OsAdd2SortLink(const SortLinkAttribute *sortLinkHeader, SortLinkList *sortList)
{
    SortLinkList *listSorted = NULL;
    LOS_DL_LIST *listObject = NULL;
    UINT32 sortIndex;
    UINT32 rollNum;
    UINT32 timeout;

    /*
     * huge rollnum could cause carry to invalid high bit
     * and eventually affect the calculation of sort index.
     */
    if (sortList->idxRollNum > OS_TSK_MAX_ROLLNUM) {
        SET_SORTLIST_VALUE(sortList, OS_TSK_MAX_ROLLNUM);
    }
    timeout = sortList->idxRollNum;
    sortIndex = timeout & OS_TSK_SORTLINK_MASK;
    rollNum = (timeout >> OS_TSK_SORTLINK_LOGLEN) + 1;
    if (sortIndex == 0) {
        rollNum--;
    }
    EVALUATE_L(sortList->idxRollNum, rollNum);
    sortIndex = sortIndex + sortLinkHeader->cursor;
    sortIndex = sortIndex & OS_TSK_SORTLINK_MASK;
    EVALUATE_H(sortList->idxRollNum, sortIndex);

    listObject = sortLinkHeader->sortLink + sortIndex;
    if (listObject->pstNext == listObject) {
        LOS_ListTailInsert(listObject, &sortList->sortLinkNode);
    } else {
        listSorted = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
        do {
            if (ROLLNUM(listSorted->idxRollNum) <= ROLLNUM(sortList->idxRollNum)) {
                ROLLNUM_SUB(sortList->idxRollNum, listSorted->idxRollNum);
            } else {
                ROLLNUM_SUB(listSorted->idxRollNum, sortList->idxRollNum);
                break;
            }

            listSorted = LOS_DL_LIST_ENTRY(listSorted->sortLinkNode.pstNext, SortLinkList, sortLinkNode);
        } while (&listSorted->sortLinkNode != listObject);

        LOS_ListTailInsert(&listSorted->sortLinkNode, &sortList->sortLinkNode);
    }
}

LITE_OS_SEC_TEXT STATIC VOID OsCheckSortLink(const LOS_DL_LIST *listHead, const LOS_DL_LIST *listNode)
{
    LOS_DL_LIST *tmp = listNode->pstPrev;

    /* recursive check until double link round to itself */
    while (tmp != listNode) {
        if (tmp == listHead) {
            return;
        }
        tmp = tmp->pstPrev;
    }

    /* delete invalid sortlink node */
    OsBackTrace();
}

LITE_OS_SEC_TEXT VOID OsDeleteSortLink(const SortLinkAttribute *sortLinkHeader, SortLinkList *sortList)
{
    LOS_DL_LIST *listObject = NULL;
    SortLinkList *nextSortList = NULL;
    UINT32 sortIndex;

    sortIndex = SORT_INDEX(sortList->idxRollNum);
    listObject = sortLinkHeader->sortLink + sortIndex;

    /* check if pstSortList node is on the right sortlink */
    OsCheckSortLink(listObject, &sortList->sortLinkNode);

    if (listObject != sortList->sortLinkNode.pstNext) {
        nextSortList = LOS_DL_LIST_ENTRY(sortList->sortLinkNode.pstNext, SortLinkList, sortLinkNode);
        ROLLNUM_ADD(nextSortList->idxRollNum, sortList->idxRollNum);
    }
    LOS_ListDelete(&sortList->sortLinkNode);
}

LITE_OS_SEC_TEXT STATIC INLINE UINT32 OsCalcExpireTime(UINT32 rollNum, UINT32 sortIndex, UINT16 curSortIndex)
{
    UINT32 expireTime;

    if (sortIndex > curSortIndex) {
        sortIndex = sortIndex - curSortIndex;
    } else {
        sortIndex = OS_TSK_SORTLINK_LEN - curSortIndex + sortIndex;
    }
    expireTime = ((rollNum - 1) << OS_TSK_SORTLINK_LOGLEN) + sortIndex;
    return expireTime;
}

STATIC UINT32 OsSortLinkGetNextExpireSortLinkList(const SortLinkAttribute *sortLinkHeader, SortLinkList **OutlistSorted)
{
    UINT16 cursor;
    UINT32 minSortIndex = OS_INVALID_VALUE;
    UINT32 minRollNum = OS_TSK_LOW_BITS_MASK;
    UINT32 expireTime = OS_INVALID_VALUE;
    LOS_DL_LIST *listObject = NULL;
    SortLinkList *listSorted = NULL;
    UINT32 i;

    cursor = (sortLinkHeader->cursor + 1) & OS_TSK_SORTLINK_MASK;

    for (i = 0; i < OS_TSK_SORTLINK_LEN; i++) {
        listObject = sortLinkHeader->sortLink + ((cursor + i) & OS_TSK_SORTLINK_MASK);
        if (!LOS_ListEmpty(listObject)) {
            listSorted = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
            if (minRollNum > ROLLNUM(listSorted->idxRollNum)) {
                minRollNum = ROLLNUM(listSorted->idxRollNum);
                minSortIndex = (cursor + i) & OS_TSK_SORTLINK_MASK;
                *OutlistSorted = listSorted;
            }
        }
    }

    if (minRollNum != OS_TSK_LOW_BITS_MASK) {
        expireTime = OsCalcExpireTime(minRollNum, minSortIndex, sortLinkHeader->cursor);
    }

    return expireTime;
}

LITE_OS_SEC_TEXT UINT32 OsSortLinkGetNextExpireTime(const SortLinkAttribute *sortLinkHeader)
{
    UINT32 expireTime;
    SortLinkList *ExpirelistSorted = NULL;
    /* public function, ExpirelistSorted not used */
    expireTime = OsSortLinkGetNextExpireSortLinkList(sortLinkHeader, &ExpirelistSorted);
    (VOID)ExpirelistSorted;

    return expireTime;
}

#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
UINT32 OsSortLinkGetNextTaskExpireTime(UINT32 *taskId, UINTPTR *handler)
{
    UINT32 expireTime;
    SortLinkList *listSorted = NULL;
    LosTaskCB *taskCB = NULL;

    expireTime = OsSortLinkGetNextExpireSortLinkList(&OsPercpuGet()->taskSortLink, &listSorted);
    if (expireTime != OS_INVALID_VALUE) {
        taskCB = LOS_DL_LIST_ENTRY(listSorted, LosTaskCB, sortList);
        *handler = (UINTPTR)(taskCB->taskEntry);
        *taskId = taskCB->taskId;
    }

    return expireTime;
}

UINT32 OsSortLinkGetNextSwtmrExpireTime(UINT32 *swtmrId, UINTPTR *handler)
{
    UINT32 expireTime;
    SortLinkList *listSorted = NULL;
    LosSwtmrCB *swtmrCB = NULL;

    expireTime = OsSortLinkGetNextExpireSortLinkList(&OsPercpuGet()->swtmrSortLink, &listSorted);
    if (expireTime != OS_INVALID_VALUE) {
        swtmrCB = LOS_DL_LIST_ENTRY(listSorted, LosSwtmrCB, sortList);
        *swtmrId = swtmrCB->timerId;
        *handler = (UINTPTR)(swtmrCB->handler);
    }

    return expireTime;
}
#endif

LITE_OS_SEC_TEXT VOID OsSortLinkUpdateExpireTime(UINT32 sleepTicks, SortLinkAttribute *sortLinkHeader)
{
    SortLinkList *sortList = NULL;
    LOS_DL_LIST *listObject = NULL;
    UINT32 i;
    UINT32 sortIndex;
    UINT32 rollNum;

    if (sleepTicks == 0) {
        return;
    }
    sortIndex = sleepTicks & OS_TSK_SORTLINK_MASK;
    rollNum = (sleepTicks >> OS_TSK_SORTLINK_LOGLEN) + 1;
    if (sortIndex == 0) {
        rollNum--;
        sortIndex = OS_TSK_SORTLINK_LEN;
    }

    for (i = 0; i < OS_TSK_SORTLINK_LEN; i++) {
        listObject = sortLinkHeader->sortLink + ((sortLinkHeader->cursor + i) & OS_TSK_SORTLINK_MASK);
        if (listObject->pstNext != listObject) {
            sortList = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
            ROLLNUM_SUB(sortList->idxRollNum, rollNum - 1);
            if ((i > 0) && (i < sortIndex)) {
                ROLLNUM_DEC(sortList->idxRollNum);
            }
        }
    }
    sortLinkHeader->cursor = (sortLinkHeader->cursor + sleepTicks - 1) % OS_TSK_SORTLINK_LEN;
}

LITE_OS_SEC_TEXT_MINOR UINT32 OsSortLinkGetTargetExpireTime(const SortLinkAttribute *sortLinkHeader,
                                                            const SortLinkList *targetSortList)
{
    SortLinkList *listSorted = NULL;
    LOS_DL_LIST *listObject = NULL;
    UINT32 sortIndex = SORT_INDEX(targetSortList->idxRollNum);
    UINT32 rollNum = ROLLNUM(targetSortList->idxRollNum);

    listObject = sortLinkHeader->sortLink + sortIndex;

    listSorted = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
    while (listSorted != targetSortList) {
        rollNum += ROLLNUM(listSorted->idxRollNum);
        listSorted = LOS_DL_LIST_ENTRY((listSorted->sortLinkNode).pstNext, SortLinkList, sortLinkNode);
    }
    return OsCalcExpireTime(rollNum, sortIndex, sortLinkHeader->cursor);
}

#else /* LOSCFG_BASE_CORE_USE_SINGLE_LIST */
LITE_OS_SEC_TEXT_INIT VOID OsSortLinkInit(SortLinkAttribute *sortLinkHeader, LOS_DL_LIST *list)
{
    LOS_DL_LIST *listObject = list;

    sortLinkHeader->sortLink = listObject;
    LOS_ListInit(listObject);
}

LITE_OS_SEC_TEXT VOID OsAdd2SortLink(const SortLinkAttribute *sortLinkHeader, SortLinkList *sortList)
{
    SortLinkList *listSorted = NULL;
    LOS_DL_LIST *listObject = NULL;

    /*
     * huge rollnum could cause carry to invalid high bit
     * and eventually affect the calculation of sort index.
     */
    if (sortList->idxRollNum > OS_TSK_MAX_ROLLNUM) {
        SET_SORTLIST_VALUE(sortList, OS_TSK_MAX_ROLLNUM);
    }

    listObject = sortLinkHeader->sortLink;

    if (listObject->pstNext == listObject) {
        LOS_ListTailInsert(listObject, &sortList->sortLinkNode);
    } else {
        listSorted = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
        do {
            if (ROLLNUM(listSorted->idxRollNum) <= ROLLNUM(sortList->idxRollNum)) {
                ROLLNUM_SUB(sortList->idxRollNum, listSorted->idxRollNum);
            } else {
                ROLLNUM_SUB(listSorted->idxRollNum, sortList->idxRollNum);
                break;
            }

            listSorted = LOS_DL_LIST_ENTRY(listSorted->sortLinkNode.pstNext, SortLinkList, sortLinkNode);
        } while (&listSorted->sortLinkNode != listObject);

        LOS_ListTailInsert(&listSorted->sortLinkNode, &sortList->sortLinkNode);
    }
}

LITE_OS_SEC_TEXT STATIC VOID OsCheckSortLink(const LOS_DL_LIST *listHead, const LOS_DL_LIST *listNode)
{
    LOS_DL_LIST *tmp = listNode->pstPrev;

    /* recursive check until double link round to itself */
    while (tmp != listNode) {
        if (tmp == listHead) {
            return;
        }
        tmp = tmp->pstPrev;
    }

    /* delete invalid sortlink node */
    OsBackTrace();
}

LITE_OS_SEC_TEXT VOID OsDeleteSortLink(const SortLinkAttribute *sortLinkHeader, SortLinkList *sortList)
{
    LOS_DL_LIST *listObject = NULL;
    SortLinkList *nextSortList = NULL;

    listObject = sortLinkHeader->sortLink;

    /* check if pstSortList node is on the right sortlink */
    OsCheckSortLink(listObject, &sortList->sortLinkNode);

    if (listObject != sortList->sortLinkNode.pstNext) {
        nextSortList = LOS_DL_LIST_ENTRY(sortList->sortLinkNode.pstNext, SortLinkList, sortLinkNode);
        ROLLNUM_ADD(nextSortList->idxRollNum, sortList->idxRollNum);
    }
    LOS_ListDelete(&sortList->sortLinkNode);
}

SortLinkList *OsSortLinkGetNextExpireSortLinkList(const SortLinkAttribute *sortLinkHeader)
{
    LOS_DL_LIST *listObject = NULL;
    SortLinkList *listSorted = NULL;

    listObject = sortLinkHeader->sortLink;
    if (!LOS_ListEmpty(listObject)) {
        listSorted = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
    }

    return listSorted;
}

LITE_OS_SEC_TEXT UINT32 OsSortLinkGetNextExpireTime(const SortLinkAttribute *sortLinkHeader)
{
    UINT32 expireTime = OS_INVALID_VALUE;
    SortLinkList *listSorted = NULL;

    listSorted = OsSortLinkGetNextExpireSortLinkList(sortLinkHeader);
    if (listSorted == NULL) {
        return expireTime;
    }
    expireTime = listSorted->idxRollNum;

    return expireTime;
}

#ifdef LOSCFG_LOWPOWER_SLEEP_DEBUG
UINT32 OsSortLinkGetNextTaskExpireTime(UINT32 *taskId, UINTPTR *handler)
{
    UINT32 expireTime = OS_INVALID_VALUE;
    SortLinkList *listSorted = NULL;
    LosTaskCB *taskCB = NULL;
    SortLinkAttribute *sortLinkHeader = &OsPercpuGet()->taskSortLink;

    listSorted = OsSortLinkGetNextExpireSortLinkList(sortLinkHeader);
    if (listSorted == NULL) {
        return expireTime;
    }

    expireTime = listSorted->idxRollNum;
    if (expireTime != OS_INVALID_VALUE) {
        taskCB = LOS_DL_LIST_ENTRY(listSorted, LosTaskCB, sortList);
        *handler = (UINTPTR)(taskCB->taskEntry);
        *taskId = taskCB->taskId;
    }

    return expireTime;
}

UINT32 OsSortLinkGetNextSwtmrExpireTime(UINT32 *swtmrId, UINTPTR *handler)
{
    UINT32 expireTime = OS_INVALID_VALUE;
    SortLinkList *listSorted = NULL;
    LosSwtmrCB *swtmrCB = NULL;
    SortLinkAttribute *sortLinkHeader = &OsPercpuGet()->swtmrSortLink;

    listSorted = OsSortLinkGetNextExpireSortLinkList(sortLinkHeader);
    if (listSorted == NULL) {
        return expireTime;
    }

    expireTime = listSorted->idxRollNum;
    if (expireTime != OS_INVALID_VALUE) {
        swtmrCB = LOS_DL_LIST_ENTRY(listSorted, LosSwtmrCB, sortList);
        *swtmrId = swtmrCB->timerId;
        *handler = (UINTPTR)(swtmrCB->handler);
    }

    return expireTime;
}
#endif
LITE_OS_SEC_TEXT VOID OsSortLinkUpdateExpireTime(UINT32 sleepTicks, SortLinkAttribute *sortLinkHeader)
{
    SortLinkList *sortList = NULL;
    LOS_DL_LIST *listObject = NULL;

    if (sleepTicks == 0) {
        return;
    }

    listObject = sortLinkHeader->sortLink;
    if (!LOS_ListEmpty(listObject)) {
        sortList = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);
        ROLLNUM_SUB(sortList->idxRollNum, sleepTicks - 1);
    }
}

LITE_OS_SEC_TEXT_MINOR UINT32 OsSortLinkGetTargetExpireTime(const SortLinkAttribute *sortLinkHeader,
                                                            const SortLinkList *targetSortList)
{
    SortLinkList *listSorted = NULL;
    LOS_DL_LIST *listObject = NULL;
    UINT32 rollNum = targetSortList->idxRollNum;

    listObject = sortLinkHeader->sortLink;
    listSorted = LOS_DL_LIST_ENTRY(listObject->pstNext, SortLinkList, sortLinkNode);

    while (listSorted != targetSortList) {
        rollNum += listSorted->idxRollNum;
        listSorted = LOS_DL_LIST_ENTRY((listSorted->sortLinkNode).pstNext, SortLinkList, sortLinkNode);
    }

    return rollNum;
}
#endif /* LOSCFG_BASE_CORE_USE_MULTI_LIST */
