/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Queue
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

/**
 * @defgroup los_queue Queue
 * @ingroup kernel
 */

#ifndef _LOS_QUEUE_H
#define _LOS_QUEUE_H

#include "los_base.h"
#include "los_list.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup los_queue
 * Queue error code: The maximum number of queue resources is configured to 0.
 *
 * Value: 0x02000600.
 *
 * Solution: Configure the maximum number of queue resources to be greater than 0. If queue
 * modules are not used, set the configuration item for the tailoring of the maximum number
 * of queue resources to NO.
 * @deprecated This error code is obsolete since LiteOS 5.0.0.
 */
#define LOS_ERRNO_QUEUE_MAXNUM_ZERO         LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x00)

/**
 * @ingroup los_queue
 * Queue error code: The queue block memory fails to be initialized.
 *
 * Value: 0x02000601.
 *
 * Solution: Allocate the queue block bigger memory partition, or decrease the maximum
 * number of queue resources.
 */
#define LOS_ERRNO_QUEUE_NO_MEMORY           LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x01)

/**
 * @ingroup los_queue
 * Queue error code: The memory for queue creation fails to be requested.
 *
 * Value: 0x02000602.
 *
 * Solution: Allocate more memory for queue creation, or decrease the queue length and
 * the number of nodes in the queue to be created.
 */
#define LOS_ERRNO_QUEUE_CREATE_NO_MEMORY    LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x02)

/**
 * @ingroup los_queue
 * Queue error code: The size of the biggest message in the created queue is too big.
 *
 * Value: 0x02000603.
 *
 * Solution: Change the size of the biggest message in the created queue.
 */
#define LOS_ERRNO_QUEUE_SIZE_TOO_BIG        LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x03)

/**
 * @ingroup los_queue
 * Queue error code: The upper limit of the number of created queues is exceeded.
 *
 * Value: 0x02000604.
 *
 * Solution: Increase the configured number of resources for queues.
 */
#define LOS_ERRNO_QUEUE_CB_UNAVAILABLE      LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x04)

/**
 * @ingroup los_queue
 * Queue error code: Invalid queue.
 *
 * Value: 0x02000605.
 *
 * Solution: Ensure that the passed-in queue ID is valid.
 */
#define LOS_ERRNO_QUEUE_NOT_FOUND           LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x05)

/**
 * @ingroup los_queue
 * Queue error code: The task is forbidden to be blocked on a queue when the task is locked.
 *
 * Value: 0x02000606.
 *
 * Solution: Unlock the task before using a queue.
 */
#define LOS_ERRNO_QUEUE_PEND_IN_LOCK        LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x06)

/**
 * @ingroup los_queue
 * Queue error code: The time set for waiting to processing the queue expires.
 *
 * Value: 0x02000607.
 *
 * Solution: Check whether the expiry time setting is appropriate.
 */
#define LOS_ERRNO_QUEUE_TIMEOUT             LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x07)

/**
 * @ingroup los_queue
 * Queue error code: The queue that blocks a task cannot be deleted.
 *
 * Value: 0x02000608.
 *
 * Solution: Enable the task to obtain resources rather than be blocked on the queue.
 */
#define LOS_ERRNO_QUEUE_IN_TSKUSE           LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x08)

/**
 * @ingroup los_queue
 * Queue error code: The queue cannot be written during an interrupt when the time for
 * waiting to processing the queue expires.
 *
 * Value: 0x02000609.
 *
 * Solution: Set the expiry time to the never-waiting mode, or use asynchronous queues.
 */
#define LOS_ERRNO_QUEUE_WRITE_IN_INTERRUPT  LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x09)

/**
 * @ingroup los_queue
 * Queue error code: The queue is not created.
 *
 * Value: 0x0200060a.
 *
 * Solution: Check whether the passed-in queue handle value is valid.
 */
#define LOS_ERRNO_QUEUE_NOT_CREATE          LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0a)

/**
 * @ingroup los_queue
 * Queue error code: Queue reading and writing are not synchronous.
 *
 * Value: 0x0200060b.
 *
 * Solution: Synchronize queue reading with queue writing.
 */
#define LOS_ERRNO_QUEUE_IN_TSKWRITE         LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0b)

/**
 * @ingroup los_queue
 * Queue error code: Parameters passed in during queue creation are null pointers.
 *
 * Value: 0x0200060c.
 *
 * Solution: Ensure the passed-in parameters are not null pointers.
 */
#define LOS_ERRNO_QUEUE_CREAT_PTR_NULL      LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0c)

/**
 * @ingroup los_queue
 * Queue error code: The queue length or message node size passed in during queue creation is 0.
 *
 * Value: 0x0200060d.
 *
 * Solution: Pass in correct queue length and message node size.
 */
#define LOS_ERRNO_QUEUE_PARA_ISZERO         LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0d)

/**
 * @ingroup los_queue
 * Queue error code: The handle of the queue is invalid.
 *
 * Value: 0x0200060e.
 *
 * Solution: Check whether the passed-in queue handle value is valid.
 */
#define LOS_ERRNO_QUEUE_INVALID             LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0e)

/**
 * @ingroup los_queue
 * Queue error code: The pointer passed in during queue reading is null.
 *
 * Value: 0x0200060f.
 *
 * Solution: Check whether the passed-in pointer is null.
 */
#define LOS_ERRNO_QUEUE_READ_PTR_NULL       LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x0f)

/**
 * @ingroup los_queue
 * Queue error code: The buffer size passed in during queue reading is too small or too big.
 *
 * Value: 0x02000610.
 *
 * Solution: Pass in a correct buffer size between [sizeof(CHAR*), OS_NULL_SHORT].
 */
#define LOS_ERRNO_QUEUE_READSIZE_IS_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x10)

/**
 * @ingroup los_queue
 * Queue error code: The pointer passed in during queue writing is null.
 *
 * Value: 0x02000612.
 *
 * Solution: Check whether the passed-in pointer is null.
 */
#define LOS_ERRNO_QUEUE_WRITE_PTR_NULL      LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x12)

/**
 * @ingroup los_queue
 * Queue error code: The buffer size passed in during queue writing is 0.
 *
 * Value: 0x02000613.
 *
 * Solution: Pass in a correct buffer size.
 */
#define LOS_ERRNO_QUEUE_WRITESIZE_ISZERO    LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x13)

/**
 * @ingroup los_queue
 * Queue error code: The buffer size passed in during queue writing is bigger than the queue size.
 *
 * Value: 0x02000615.
 *
 * Solution: Decrease the buffer size, or use a queue in which nodes are bigger.
 */
#define LOS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG  LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x15)

/**
 * @ingroup los_queue
 * Queue error code: No free node is available during queue writing.
 *
 * Value: 0x02000616.
 *
 * Solution: Ensure that free nodes are available before queue writing.
 */
#define LOS_ERRNO_QUEUE_ISFULL              LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x16)

/**
 * @ingroup los_queue
 * Queue error code: The pointer passed in when the queue information is being obtained is null.
 *
 * Value: 0x02000617.
 *
 * Solution: Check whether the passed-in pointer is null.
 */
#define LOS_ERRNO_QUEUE_PTR_NULL            LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x17)

/**
 * @ingroup los_queue
 * Queue error code: The queue cannot be read during an interrupt
 * when the time for waiting to processing the queue expires.
 *
 * Value: 0x02000618.
 *
 * Solution: Set the expiry time to the never-waiting mode, or use asynchronous queues.
 */
#define LOS_ERRNO_QUEUE_READ_IN_INTERRUPT   LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x18)

/**
 * @ingroup los_queue
 * Queue error code: The handle of the queue passed-in when the memory for the queue is being freed is invalid.
 *
 * Value: 0x02000619.
 *
 * Solution: Check whether the passed-in queue handle value is valid.
 */
#define LOS_ERRNO_QUEUE_MAIL_HANDLE_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x19)

/**
 * @ingroup los_queue
 * Queue error code: The pointer to the memory to be freed is null.
 *
 * Value: 0x0200061a.
 *
 * Solution: Check whether the passed-in pointer is null.
 */
#define LOS_ERRNO_QUEUE_MAIL_PTR_INVALID    LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x1a)

/**
 * @ingroup los_queue
 * Queue error code: The memory for the queue fails to be freed.
 *
 * Value: 0x0200061b.
 *
 * Solution: Pass in correct input parameters.
 */
#define LOS_ERRNO_QUEUE_MAIL_FREE_ERROR     LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x1b)

/**
 * @ingroup los_queue
 * Queue error code: No resource is in the queue that is being read when the
 * time for waiting to processing the queue expires.
 *
 * Value: 0x0200061d.
 *
 * Solution: Ensure that the queue contains messages when it is being read.
 */
#define LOS_ERRNO_QUEUE_ISEMPTY             LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x1d)

/**
 * @ingroup los_queue
 * Queue error code: The buffer size passed in during queue reading is smaller than the queue size.
 *
 * Value: 0x0200061f.
 *
 * Solution: Increase the buffer size, or use a queue in which nodes are smaller.
 */
#define LOS_ERRNO_QUEUE_READ_SIZE_TOO_SMALL LOS_ERRNO_OS_ERROR(LOS_MOD_QUE, 0x1f)

/**
 * @ingroup los_queue
 * Structure of the block for queue information query
 */
typedef struct tagQueueInfo {
    UINT32 uwQueueID;       /**< Queue ID */
    UINT16 usQueueLen;      /**< Queue length, that is the number of node in the queue */
    UINT16 usQueueSize;     /**< Node size in the queue */
    UINT16 usQueueHead;     /**< The position of queue header node, it is an array subscript */
    UINT16 usQueueTail;     /**< The position of queue tail node, it is an array subscript */
    UINT16 usWritableCnt;   /**< Count of writable resources */
    UINT16 usReadableCnt;   /**< Count of readable resources */
    UINT64 uwWaitReadTask;  /**< Tasks waiting for reading message. It is a 64-bit unsigned
                                 integer, each bit represents a task ID. Therefore, 64 tasks
                                 can be represented, and the maximum task ID is 63. For example,
                                 0x0200060001002010 means there are 6 tasks whose IDs are 4,
                                 13, 24, 41, 42, and 57. */
    UINT64 uwWaitWriteTask; /**< Tasks waiting for writing message. It is a 64-bit unsigned
                                 integer same with #uwWaitReadTask, each bit represents a task ID. */
    UINT64 uwWaitMemTask;   /**< Tasks waiting for memory used by the MailBox in the CMSIS-RTOS.
                                 It is a 64-bit unsigned integer same with #uwWaitReadTask,
                                 each bit represents a task ID. */
} QUEUE_INFO_S;

#ifdef LOSCFG_QUEUE_STATIC_ALLOCATION
/**
 * @ingroup los_queue
 * Size for external queue memory.
 */
#define LOS_QUEUEMEM_SIZE(msgSize, len)   ((len) * ((msgSize) + sizeof(UINT32)))

/**
 * @ingroup los_queue
 * @brief Create a message queue.
 *
 * @par Description:
 * This API is used to create a message queue by transferring the static memory for the queue.
 * The input parameter "queueMem" is the pointer to the static memory the API can use directly.
 * The input parameter "memSize" is the static memory size.
 * @attention
 * <ul>
 * <li>There are LOSCFG_BASE_IPC_QUEUE_LIMIT queues available, change it's value when necessary.</li>
 * </ul>
 * @param queueName        [IN]    Message queue name. Reserved parameter, not used for now.
 * @param len              [IN]    Queue length. The value range is [1,0xffff].
 * @param queueId          [OUT]   ID of the queue control structure that is successfully created.
 * @param flags            [IN]    Queue mode. Reserved parameter, not used for now.
 * @param maxMsgSize       [IN]    Node size. The value range is [1,0xffff].
 * @param queueMem         [IN]    Static queue memory. The value range is [0,0xffffffff].
 * @param memSize          [IN]    Queue memory size. The value range is [1,0xffff].

 * @retval   #LOS_OK                               The message queue is successfully created.
 * @retval   #LOS_ERRNO_QUEUE_CB_UNAVAILABLE       The upper limit of the number of created queues is exceeded.
 * @retval   #LOS_ERRNO_QUEUE_CREATE_NO_MEMORY     Insufficient memory for queue creation.
 * @retval   #LOS_ERRNO_QUEUE_CREAT_PTR_NULL       Null pointer, queueID is NULL.
 * @retval   #LOS_ERRNO_QUEUE_PARA_ISZERO          The queue length or message node size passed in during queue
 * creation is 0.
 * @par Dependency:
 * <ul><li>los_queue.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_QueueDelete | LOS_QueueCreate
 * @since Huawei LiteOS V200R005C00
 */
extern UINT32 LOS_QueueCreateStatic(const CHAR *queueName,
                                    UINT16 len,
                                    UINT32 *queueId,
                                    UINT32 flags,
                                    UINT16 maxMsgSize,
                                    VOID *queueMem,
                                    UINT16 memSize);
#endif

#ifdef LOSCFG_QUEUE_DYNAMIC_ALLOCATION
/**
 * @ingroup los_queue
 * @brief Create a message queue.
 *
 * @par Description:
 * This API is used to create a message queue. Different from LOS_QueueCreateStatic,
 * the user does not need to transfer the queue memory. In the API, system allocated
 * the queue memory automatically.
 * @attention
 * There are LOSCFG_BASE_IPC_QUEUE_LIMIT queues available, change it's value when necessary.
 * This function is defined only when LOSCFG_QUEUE_DYNAMIC_ALLOCATION is defined.
 * @param queueName        [IN]  Message queue name. Reserved parameter, not used for now.
 * @param len              [IN]  Queue length. The value range is [1,0xffff].
 * @param queueId          [OUT] ID of the queue control structure that is successfully created.
 * @param flags            [IN]  Queue mode. Reserved parameter, not used for now.
 * @param maxMsgSize       [IN]  Node size. The value range is [1,0xffff].
 *
 * @retval   #LOS_OK                            The message queue is successfully created.
 * @retval   #LOS_ERRNO_QUEUE_CB_UNAVAILABLE    The upper limit of the number of created queues is exceeded.
 * @retval   #LOS_ERRNO_QUEUE_CREATE_NO_MEMORY  Insufficient memory for queue creation.
 * @retval   #LOS_ERRNO_QUEUE_CREAT_PTR_NULL    Null pointer, queueId is NULL.
 * @retval   #LOS_ERRNO_QUEUE_PARA_ISZERO       The queue length or message node size passed in during queue
 * creation is 0.
 * @par Dependency:
 * <ul><li>los_queue.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_QueueDelete | LOS_QueueCreateStatic
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueCreate(const CHAR *queueName,
                              UINT16 len,
                              UINT32 *queueId,
                              UINT32 flags,
                              UINT16 maxMsgSize);
#endif

/**
 * @ingroup los_queue
 * @brief Read a queue.
 *
 * @par Description:
 * This API is used to read data in a specified queue, and store the obtained data to the address specified
 * by bufferAddr. The address and the size of the data to be read are defined by users.
 * @attention
 * <ul>
 * <li>The specific queue should be created firstly.</li>
 * <li>Queue reading adopts the fist in first out (FIFO) mode. The data that is first stored in the queue is read
 * first.</li>
 * <li>bufferAddr stores the obtained data.</li>
 * <li>Do not read or write a queue in unblocking modes such as an interrupt.</li>
 * <li>This API cannot be called before the Huawei LiteOS is initialized.</li>
 * <li>The argument timeout is a relative time.</li>
 * <li>Do not call this API in software timer callback. </li>
 * </ul>
 *
 * @param queueId        [IN]     Queue ID created by LOS_QueueCreate or LOS_QueueCreateStatic.
 *                                The value range is [0, LOSCFG_BASE_IPC_QUEUE_LIMIT - 1].
 * @param bufferAddr     [OUT]    Starting address that stores the obtained data. The starting address must not be
 * null.
 * @param bufferSize     [IN/OUT] Where to maintain the buffer wanted-size before read, and the real-size after read.
 * @param timeout        [IN]     Expiry time. The value range is [0,LOS_WAIT_FOREVER](unit: Tick).
 *
 * @retval   #LOS_OK                              The queue is successfully read.
 * @retval   #LOS_ERRNO_QUEUE_INVALID             The handle of the queue that is being read is invalid.
 * @retval   #LOS_ERRNO_QUEUE_READ_PTR_NULL       The pointer passed in during queue reading is null.
 * @retval   #LOS_ERRNO_QUEUE_READ_IN_INTERRUPT   The queue cannot be read during an interrupt when the time for
 * waiting to processing the queue expires.
 * @retval   #LOS_ERRNO_QUEUE_NOT_CREATE          The queue to be read is not created.
 * @retval   #LOS_ERRNO_QUEUE_ISEMPTY             No resource is in the queue that is being read when the time for
 * waiting to processing the queue expires.
 * @retval   #LOS_ERRNO_QUEUE_PEND_IN_LOCK        The task is forbidden to be blocked on a queue when the task is
 * locked.
 * @retval   #LOS_ERRNO_QUEUE_TIMEOUT             The time set for waiting to processing the queue expires.
 * @retval   #LOS_ERRNO_QUEUE_READ_SIZE_TOO_SMALL The buffer size passed in during queue reading is less than
 * the queue size.
 * @par Dependency:
 * <ul><li>los_queue.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_QueueWriteCopy | LOS_QueueWriteHeadCopy | LOS_QueueCreate | LOS_QueueCreateStatic
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueReadCopy(UINT32 queueId,
                                VOID *bufferAddr,
                                UINT32 *bufferSize,
                                UINT32 timeout);

/**
 * @ingroup los_queue
 * @brief Write data into a queue.
 *
 * @par Description:
 * This API is used to write the data of the size specified by bufferSize and stored at the address specified by
 * bufferAddr into a queue.
 * @attention
 * <ul>
 * <li>The specific queue should be created firstly.</li>
 * <li>Do not read or write a queue in unblocking modes such as interrupt.</li>
 * <li>This API cannot be called before the Huawei LiteOS is initialized.</li>
 * <li>The data to be written is of the size specified by bufferSize and is stored at the address specified by
 * bufferAddr.</li>
 * <li>The argument timeout is a relative time.</li>
 * <li>Do not call this API in software timer callback. </li>
 * </ul>
 *
 * @param queueId        [IN]  Queue ID created by LOS_QueueCreate or LOS_QueueCreateStatic.
 *                             The value range is [0, LOSCFG_BASE_IPC_QUEUE_LIMIT - 1].
 * @param bufferAddr     [IN]  Starting address that stores the data to be written.The starting address must
 *                             not be null.
 * @param bufferSize     [IN]  Passed-in buffer size. The value range is [1,USHRT_MAX - sizeof(UINT32)].
 * @param timeout        [IN]  Expiry time. The value range is [0,LOS_WAIT_FOREVER](unit: Tick).
 *
 * @retval   #LOS_OK                              The data is successfully written into the queue.
 * @retval   #LOS_ERRNO_QUEUE_INVALID             The queue handle passed in during queue writing is invalid.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_PTR_NULL      The pointer passed in during queue writing is null.
 * @retval   #LOS_ERRNO_QUEUE_WRITESIZE_ISZERO    The buffer size passed in during queue writing is 0.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_IN_INTERRUPT  The queue cannot be written during an interrupt when the time
 * for waiting to processing the queue expires.
 * @retval   #LOS_ERRNO_QUEUE_NOT_CREATE          The queue into which the data is written is not created.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG  The buffer size passed in during queue writing is bigger than
 * the queue size.
 * @retval   #LOS_ERRNO_QUEUE_ISFULL              No free node is available during queue writing.
 * @retval   #LOS_ERRNO_QUEUE_PEND_IN_LOCK        The task is forbidden to be blocked on a queue when
 * the task is locked.
 * @retval   #LOS_ERRNO_QUEUE_TIMEOUT             The time set for waiting to processing the queue expires.
 * @par Dependency:
 * <ul><li>los_queue.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_QueueReadCopy | LOS_QueueWriteHeadCopy | LOS_QueueCreate | LOS_QueueCreateStatic
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueWriteCopy(UINT32 queueId,
                                 VOID *bufferAddr,
                                 UINT32 bufferSize,
                                 UINT32 timeout);

/**
 * @ingroup los_queue
 * @brief Read a queue.
 *
 * @par Description:
 * This API is used to read the address of data in a specified queue, and store it to the address specified by
 * bufferAddr.
 * @attention
 * <ul>
 * <li>The specific queue should be created firstly.</li>
 * <li>Queue reading adopts the fist in first out (FIFO) mode. The data that is first stored in the queue is
 * read first.</li>
 * <li>bufferAddr stores the obtained data address.</li>
 * <li>Do not read or write a queue in unblocking modes such as an interrupt.</li>
 * <li>This API cannot be called before the Huawei LiteOS is initialized.</li>
 * <li>The argument timeout is a relative time.</li>
 * <li>The bufferSize is not really used in LOS_QueueRead, because the interface is only used to
 * obtain the address of data.</li>
 * <li>The buffer which the bufferAddr pointing to must be greater than or equal to 4 bytes.</li>
 * <li>Do not call this API in software timer callback. </li>
 * </ul>
 *
 * @param queueId        [IN]  Queue ID created by LOS_QueueCreate or LOS_QueueCreateStatic.
 *                             The value range is [0, LOSCFG_BASE_IPC_QUEUE_LIMIT - 1].
 * @param bufferAddr     [OUT] Starting address that stores the obtained data. The starting address must
 *                             not be null.
 * @param bufferSize     [IN]  Passed-in buffer size,The value must be greater than sizeof(CHAR*).
 * @param timeout        [IN]  Expiry time. The value range is [0,LOS_WAIT_FOREVER](unit: Tick).
 *
 * @retval   #LOS_OK                               The queue is successfully read.
 * @retval   #LOS_ERRNO_QUEUE_INVALID              The handle of the queue that is being read is invalid.
 * @retval   #LOS_ERRNO_QUEUE_READ_PTR_NULL        The pointer passed in during queue reading is null.
 * @retval   #LOS_ERRNO_QUEUE_READ_IN_INTERRUPT    The queue cannot be read during an interrupt when the time for
 * waiting to processing the queue expires.
 * @retval   #LOS_ERRNO_QUEUE_NOT_CREATE           The queue to be read is not created.
 * @retval   #LOS_ERRNO_QUEUE_ISEMPTY              No resource is in the queue that is being read when the time for
 * waiting to processing the queue expires.
 * @retval   #LOS_ERRNO_QUEUE_PEND_IN_LOCK         The task is forbidden to be blocked on a queue when the task is
 * locked.
 * @retval   #LOS_ERRNO_QUEUE_TIMEOUT              The time set for waiting to processing the queue expires.
 * @par Dependency:
 * <ul><li>los_queue.h: The header file that contains the API declaration.</li></ul>
 * @see LOS_QueueWrite | LOS_QueueWriteHead | LOS_QueueCreate | LOS_QueueCreateStatic
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueRead(UINT32 queueId,
                            VOID *bufferAddr,
                            UINT32 bufferSize,
                            UINT32 timeout);

/**
 * @ingroup los_queue
 * @brief Write data into a queue.
 *
 * @par Description:
 * This API is used to write the address of data specified by bufferAddr into a queue.
 * @attention
 * <ul>
 * <li>The specific queue should be created firstly.</li>
 * <li>Do not read or write a queue in unblocking modes such as an interrupt.</li>
 * <li>This API cannot be called before the Huawei LiteOS is initialized.</li>
 * <li>The address of the data of the size specified by bufferSize and stored at the address specified by
 * bufferAddr is to be written.</li>
 * <li>The argument timeout is a relative time.</li>
 * <li>The bufferSize is not really used in LOS_QueueWrite, because the interface is only used to write the address
 * of data specified by bufferAddr into a queue.</li>
 * <li>Do not call this API in software timer callback. </li>
 * </ul>
 *
 * @param queueId        [IN] Queue ID created by LOS_QueueCreate or LOS_QueueCreateStatic.
 *                            The value range is [0, LOSCFG_BASE_IPC_QUEUE_LIMIT - 1].
 * @param bufferAddr     [IN] Starting address that stores the data to be written. The starting address
 *                            must not be null.
 * @param bufferSize     [IN] This parameter is not in use temporarily.
 * @param timeout        [IN] Expiry time. The value range is [0,LOS_WAIT_FOREVER](unit: Tick).
 *
 * @retval   #LOS_OK                             The data is successfully written into the queue.
 * @retval   #LOS_ERRNO_QUEUE_INVALID            The queue handle passed in during queue writing is invalid.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_PTR_NULL     The pointer passed in during queue writing is null.
 * @retval   #LOS_ERRNO_QUEUE_WRITESIZE_ISZERO   The buffer size passed in during queue writing is 0.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_IN_INTERRUPT The queue cannot be written during an interrupt when the time for
 * waiting to processing the queue expires.
 * @retval   #LOS_ERRNO_QUEUE_NOT_CREATE         The queue into which the data is written is not created.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG The buffer size passed in during queue writing is bigger than
 * the queue size.
 * @retval   #LOS_ERRNO_QUEUE_ISFULL             No free node is available during queue writing.
 * @retval   #LOS_ERRNO_QUEUE_PEND_IN_LOCK       The task is forbidden to be blocked on a queue when the task is
 * locked.
 * @retval   #LOS_ERRNO_QUEUE_TIMEOUT            The time set for waiting to processing the queue expires.
 * @par Dependency:
 * <ul><li>los_queue.h: The header file that contains the API declaration.</li></ul>
 * @see LOS_QueueRead | LOS_QueueWriteHead | LOS_QueueCreate | LOS_QueueCreateStatic
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueWrite(UINT32 queueId,
                             VOID *bufferAddr,
                             UINT32 bufferSize,
                             UINT32 timeout);

/**
 * @ingroup los_queue
 * @brief Write data into a queue header.
 *
 * @par Description:
 * This API is used to write the data of the size specified by bufferSize and stored at the address specified by
 * bufferAddr into a queue header.
 * @attention
 * <ul>
 * <li>Do not read or write a queue in unblocking modes such as an interrupt.</li>
 * <li>This API cannot be called before the Huawei LiteOS is initialized.</li>
 * <li>The address of the data of the size specified by bufferSize and stored at the address specified by
 * bufferAddr is to be written.</li>
 * <li>The argument timeout is a relative time.</li>
 * <li>LOS_QueueRead and LOS_QueueWriteHead are a set of interfaces, and the two groups of interfaces need to
 * be used.</li>
 * <li>Do not call this API in software timer callback. </li>
 * </ul>
 *
 * @param queueId        [IN]  Queue ID created by LOS_QueueCreate or LOS_QueueCreateStatic.
 *                             The value range is [0, LOSCFG_BASE_IPC_QUEUE_LIMIT - 1].
 * @param bufferAddr     [OUT] Starting address that stores the data to be written. The starting address
 *                             must not be null.
 * @param bufferSize     [IN]  This parameter is not in use temporarily.
 * @param timeout        [IN]  Expiry time. The value range is [0,LOS_WAIT_FOREVER](unit: Tick).
 *
 * @retval   #LOS_OK                             The data is successfully written into the queue.
 * @retval   #LOS_ERRNO_QUEUE_INVALID            The queue handle passed in during queue writing is invalid.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_PTR_NULL     The pointer passed in during queue writing is null.
 * @retval   #LOS_ERRNO_QUEUE_WRITESIZE_ISZERO   The buffer size passed in during queue writing is 0.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_IN_INTERRUPT The queue cannot be written during an interrupt when the time for
 * waiting to processing the queue expires. waiting to processing the queue expires.
 * @retval   #LOS_ERRNO_QUEUE_NOT_CREATE         The queue into which the data is written is not created.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG The buffer size passed in during queue writing is bigger than
 * the queue size.
 * @retval   #LOS_ERRNO_QUEUE_ISFULL             No free node is available during queue writing.
 * @retval   #LOS_ERRNO_QUEUE_PEND_IN_LOCK       The task is forbidden to be blocked on a queue when the task is
 * locked.
 * @retval   #LOS_ERRNO_QUEUE_TIMEOUT            The time set for waiting to processing the queue expires.
 * @par Dependency:
 * <ul><li>los_queue.h: The header file that contains the API declaration.</li></ul>
 * @see LOS_QueueWrite | LOS_QueueRead | LOS_QueueCreate | LOS_QueueCreateStatic
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueWriteHead(UINT32 queueId,
                                 VOID *bufferAddr,
                                 UINT32 bufferSize,
                                 UINT32 timeout);

/**
 * @ingroup los_queue
 * @brief Write data into a queue header.
 *
 * @par Description:
 * This API is used to write the data of the size specified by bufferSize and stored at the address specified by
 * bufferAddr into a queue header.
 * @attention
 * <ul>
 * <li>Do not read or write a queue in unblocking modes such as an interrupt.</li>
 * <li>This API cannot be called before the Huawei LiteOS is initialized.</li>
 * <li>The address of the data of the size specified by bufferSize and stored at the address specified by
 * bufferAddr is to be written.</li>
 * <li>The argument timeout is a relative time.</li>
 * <li>LOS_QueueRead and LOS_QueueWriteHead are a set of interfaces, and the two groups of interfaces need to be
 * used.</li>
 * <li>Do not call this API in software timer callback. </li>
 * </ul>
 *
 * @param queueId        [IN]  Queue ID created by LOS_QueueCreate or LOS_QueueCreateStatic.
 *                             The value range is [0, LOSCFG_BASE_IPC_QUEUE_LIMIT - 1].
 * @param bufferAddr     [OUT] Starting address that stores the data to be written.
 *                             The starting address must not be null.
 * @param bufferSize     [IN]  Passed-in buffer size, which must not be 0. The value range is [1,0xffffffff].
 * @param timeout        [IN]  Expiry time. The value range is [0,LOS_WAIT_FOREVER](unit: Tick).
 *
 * @retval   #LOS_OK                             The data is successfully written into the queue.
 * @retval   #LOS_ERRNO_QUEUE_INVALID            The queue handle passed in during queue writing is invalid.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_PTR_NULL     The pointer passed in during queue writing is null.
 * @retval   #LOS_ERRNO_QUEUE_WRITESIZE_ISZERO   The buffer size passed in during queue writing is 0.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_IN_INTERRUPT The queue cannot be written during an interrupt when the time for
 * waiting to processing the queue expires.
 * @retval   #LOS_ERRNO_QUEUE_NOT_CREATE         The queue into which the data is written is not created.
 * @retval   #LOS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG The buffer size passed in during queue writing is bigger than
 * the queue size.
 * @retval   #LOS_ERRNO_QUEUE_ISFULL             No free node is available during queue writing.
 * @retval   #LOS_ERRNO_QUEUE_PEND_IN_LOCK       The task is forbidden to be blocked on a queue when the task is
 * locked.
 * @retval   #LOS_ERRNO_QUEUE_TIMEOUT            The time set for waiting to processing the queue expires.
 * @par Dependency:
 * <ul><li>los_queue.h: The header file that contains the API declaration.</li></ul>
 * @see LOS_QueueWriteCopy | LOS_QueueReadCopy
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueWriteHeadCopy(UINT32 queueId,
                                     VOID *bufferAddr,
                                     UINT32 bufferSize,
                                     UINT32 timeout);

/**
 * @ingroup los_queue
 * @brief Delete a queue.
 *
 * @par Description:
 * This API is used to delete a queue.
 * @attention
 * <ul>
 * <li>This API cannot be used to delete a queue that is not created.</li>
 * <li>A synchronous queue fails to be deleted if any tasks are blocked on it, or some queues are being read or
 * written.</li>
 * </ul>
 *
 * @param queueId     [IN] Queue ID created by LOS_QueueCreate or LOS_QueueCreateStatic.
 *                         The value range is [0, LOSCFG_BASE_IPC_QUEUE_LIMIT - 1].
 *
 * @retval   #LOS_OK                      The queue is successfully deleted.
 * @retval   #LOS_NOK                     Failed to release the queue memory.
 * @retval   #LOS_ERRNO_QUEUE_NOT_FOUND   The queue cannot be found.
 * @retval   #LOS_ERRNO_QUEUE_NOT_CREATE  The queue handle passed in when the queue is being deleted is
 * incorrect.
 * @retval   #LOS_ERRNO_QUEUE_IN_TSKUSE   The queue that blocks a task cannot be deleted.
 * @retval   #LOS_ERRNO_QUEUE_IN_TSKWRITE Queue reading and writing are not synchronous.
 * @par Dependency:
 * <ul><li>los_queue.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_QueueCreate | LOS_QueueCreateStatic
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueDelete(UINT32 queueId);

/**
 * @ingroup los_queue
 * @brief Obtain queue information.
 *
 * @par Description:
 * This API is used to obtain queue information.
 * @attention
 * The specific queue should be created firstly before getting the queue information.
 * @param queueId       [IN]  Queue ID created by LOS_QueueCreate or LOS_QueueCreateStatic.
 *                            The value range is [0, LOSCFG_BASE_IPC_QUEUE_LIMIT - 1].
 * @param queueInfo     [OUT] The queue information to be read must not be null.
 *
 * @retval   #LOS_OK                     The queue information is successfully obtained.
 * @retval   #LOS_ERRNO_QUEUE_PTR_NULL   The pointer to the queue information to be obtained is null.
 * @retval   #LOS_ERRNO_QUEUE_INVALID    The handle of the queue that is being read is invalid.
 * @retval   #LOS_ERRNO_QUEUE_NOT_CREATE The queue in which the information to be obtained is stored is
 * not created.
 *
 * @par Dependency:
 * <ul><li>los_queue.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_QueueCreate | LOS_QueueCreateStatic
 * @since Huawei LiteOS V100R001C00
 */
extern UINT32 LOS_QueueInfoGet(UINT32 queueId, QUEUE_INFO_S *queueInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_QUEUE_H */
