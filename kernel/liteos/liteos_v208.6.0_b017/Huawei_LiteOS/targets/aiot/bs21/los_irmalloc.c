/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: irmalloc adapter \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-10, Create file. \n
 */
#include "irmalloc_private.h"
#include "los_memory.h"
#include "securec.h"

// Turning this on makes the allocator functions panic if they are called from
// an ISR. They are not designed to be interrupt safe or particularly quick.
//
// Currently it's on as the check is quite small and we're not short of CPU time.
#define DEBUG_MALLOC_CHECK_ISR

#define MAX_NUM_DIGITS_IN_LONG_TYPE  10

void irmalloc_init_default(void)
{
}

static void *irmalloc_internal(size_t size)
{
    void *mem;
#ifdef DEBUG_MALLOC_TRACE_OWNER
    size += sizeof(uint32);
#endif
    mem = LOS_MemAlloc(m_aucSysMem0, (UINT32)size);
    return mem;
}

void *irmalloc(size_t size)
{
    void *mem = irmalloc_internal(size);
#ifdef DEBUG_MALLOC_TRACE_OWNER
    if (mem != NULL) {
        // mem is word aligned because all the sizes used by bget internally are multiples of 4.
        *(uint32 *) mem = (uintptr) __builtin_return_address(0);
        mem = (void *) ((uint32 *) mem + 1);
    }
#endif

#if defined(NBIOT_SCONS_DEFINE) && defined(LOG_IRMALLOC)
    NEW_INIT_STACK_MSG(allocMsg, irmalloc_alloc_t, IRMALLOC_ALLOC, LAYER_IRMALLOC, LAYER_IRMALLOC);
    allocMsg.size = size;
    allocMsg.caller = (uint32)__builtin_return_address(0);
    allocMsg.addr = (uint32)mem;
    log_message(&allocMsg.header);
#endif

    return mem;
}

void *irzalloc(size_t size)
{
    void *buf = LOS_MemAlloc(m_aucSysMem0, (UINT32)size);
#ifdef DEBUG_MALLOC_TRACE_OWNER
    if (buf != NULL) {
        // buf is word aligned because all the sizes used by bget internally are multiples of 4.
        *(uint32 *) buf = (uintptr) __builtin_return_address(0);
        buf = (void *) ((uint32 *) buf + 1);
    }
#endif

    if (buf != NULL) {
        memset_s(buf, size, 0, size);
    }

#if defined(NBIOT_SCONS_DEFINE) && defined(LOG_IRMALLOC)
    NEW_INIT_STACK_MSG(allocMsg, irmalloc_alloc_t, IRMALLOC_ALLOC, LAYER_IRMALLOC, LAYER_IRMALLOC);
    allocMsg.size = size;
    allocMsg.caller = (uint32)__builtin_return_address(0);
    allocMsg.addr = (uint32)buf;
    log_message(&allocMsg.header);
#endif

    return buf;
}

void *irrealloc(void *buf, size_t size)
{
    void *mem = NULL;
#if defined(NBIOT_SCONS_DEFINE) && defined(LOG_IRMALLOC)
    NEW_INIT_STACK_MSG(allocMsg, irmalloc_realloc_t, IRMALLOC_REALLOC, LAYER_IRMALLOC, LAYER_IRMALLOC);
    allocMsg.size = size;
    allocMsg.caller = (uint32)__builtin_return_address(0);
    allocMsg.addr = (uint32)buf;
    log_message(&allocMsg.header);
#endif

#ifdef DEBUG_MALLOC_TRACE_OWNER
    size += sizeof(uint32);
    // Give bget the pointer it returned.
    if (buf != NULL)
        buf = (void *) ((uint32 *) buf - 1);
#endif

    mem = LOS_MemRealloc(m_aucSysMem0, buf, (UINT32)size);
#ifdef DEBUG_MALLOC_TRACE_OWNER
    if (mem != NULL) {
        // mem is word aligned because all the sizes used by bget internally are multiples of 4.
        *(uint32 *) mem = (uintptr) __builtin_return_address(0);
        mem = (void *) ((uint32 *) mem + 1);
    }
#endif
    return mem;
}

/*lint -e429 */
/*lint -e593 */
void irfree(void *buf)
{
#if defined(NBIOT_SCONS_DEFINE) && defined(LOG_IRMALLOC)
    NEW_INIT_STACK_MSG(freeMsg, irmalloc_free_t, IRMALLOC_FREE, LAYER_IRMALLOC, LAYER_IRMALLOC);
    freeMsg.caller = (uint32)__builtin_return_address(0);
    freeMsg.addr = (uint32)buf;
    log_message(&freeMsg.header);
#endif

    if (buf == NULL) {
        return;
    }
#ifdef DEBUG_MALLOC_TRACE_OWNER
    // Restore the pointer we got from extram_bget originally.
    buf = (void *) ((uint32 *) buf - 1);
#endif
    (void)LOS_MemFree(m_aucSysMem0, buf);
}
/*lint +e429 */
/*lint +e593 */
void *irmalloc_dup(const void *orig, size_t size)
{
    void *ret = NULL;

    if (orig == NULL) {
        return NULL;
    }

    ret = LOS_MemAlloc(m_aucSysMem0, (UINT32)size);
#if defined(NBIOT_SCONS_DEFINE) && defined(LOG_IRMALLOC)
    NEW_INIT_STACK_MSG(allocMsg, irmalloc_dup_t, IRMALLOC_DUP, LAYER_IRMALLOC, LAYER_IRMALLOC);
    allocMsg.size = size;
    allocMsg.caller = (uint32)__builtin_return_address(0);
    allocMsg.addr = (uint32)ret;
    log_message(&allocMsg.header);
#endif

    if (ret == NULL) {
        return NULL;
    }
#ifdef DEBUG_MALLOC_TRACE_OWNER
    // ret is word aligned because all the sizes used by bget internally are multiples of 4.
    *((uint32 *)ret - 1) = (uintptr) __builtin_return_address(0);
#endif

    if (memcpy_s(ret, size, orig, size) != EOK) {
        (void)LOS_MemFree(m_aucSysMem0, ret);
        ret = NULL;
    }
    return ret;
}
