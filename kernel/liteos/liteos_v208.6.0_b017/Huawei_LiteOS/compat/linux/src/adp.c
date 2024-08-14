/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Adaptation
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

#include "linux/kernel.h"
#include "math.h"
#include "limits.h"
#include "sys/statfs.h"
#include "los_swtmr.h"
#ifdef LOSCFG_NET_LWIP_SACK
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/netdb.h"
#endif


long int syscall(long int sysno, ...)
{
    (VOID)sysno;
    PRINT_ERR("%s is not supported\n", __FUNCTION__);
    errno = ENOSYS;
    return -1;
}

void fs_show(const char *path)
{
    INT32 ret;
    struct statfs fss;
    if (path == NULL) {
        PRINT_ERR("path is NULL\n");
    }
    ret = statfs(path, &fss);
    PRINT_INFO("Filesystem %s info: \n", path);
    PRINT_INFO("----------------------------------------\n");
    if (ret == ENOERR) {
        PRINT_INFO("  Total clusters: %ull \n", fss.f_blocks);
        PRINT_INFO("  Cluster size: %ul \n", fss.f_bsize);
        PRINT_INFO("  Free clusters: %ull \n", fss.f_bfree);
    } else {
        ret = get_errno();
        PRINT_ERR("Get fsinfo failed: %d \n", ret);
    }
}

int fs_fssync(const char *path)
{
    (VOID)path;
    PRINT_ERR("%s is not supported\n", __FUNCTION__);
    errno = ENOSYS;
    return -1;
}

void linux_module_init(void)
{
    PRINT_ERR("%s is not supported\n", __FUNCTION__);
    errno = ENOSYS;
}

uint32_t do_div_imp(uint64_t *n, uint32_t base)
{
    UINT32 r;

    if ((n == NULL) || (base == 0)) {
        PRINT_ERR("%s invalid input param, base %u\n", __FUNCTION__, base);
        return 0;
    }

    r = *n % base;
    *n = *n / base;
    return r;
}

int32_t do_div_s64_imp(int64_t *n, int32_t base)
{
    INT32 r;

    if ((n == NULL) || (base == 0)) {
        PRINT_ERR("%s invalid input param, base:%d\n", __FUNCTION__, base);
        return 0;
    }

    r = *n % base;
    *n = *n / base;
    return r;
}

int setgroups(size_t size, const gid_t *list)
{
    (VOID)size;
    (VOID)list;
    PRINT_ERR("%s is not supported\n", __FUNCTION__);
    errno = ENOSYS;
    return -1;
}

int initgroups(const char *user, gid_t group)
{
    (VOID)user;
    (VOID)group;
    PRINT_ERR("%s is not supported\n", __FUNCTION__);
    errno = ENOSYS;
    return -1;
}

int chroot(const char *path)
{
    (VOID)path;
    PRINT_ERR("%s is not supported\n", __FUNCTION__);
    errno = ENOSYS;
    return -1;
}
