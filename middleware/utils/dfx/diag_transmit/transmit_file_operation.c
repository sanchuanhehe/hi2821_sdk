/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: file operation
 * This file should be changed only infrequently and with great care.
 */
#include "transmit_file_operation.h"
#if CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "securec.h"
#include "errcode.h"
#include "unistd.h"
#include "fcntl.h"
#include "soc_log.h"
#include "sys/stat.h"
#include "dirent.h"
#include "transmit_debug.h"

static char *get_fullpath(const char *path, const char *file_name)
{
    char *fullpath = NULL;
    int ret = 0;
    uint32_t path_len = (uint32_t)strlen(path) + (uint32_t)strlen(file_name) + 2; /* name 2 */
    fullpath = (char *)dfx_malloc(0, path_len);
    if (fullpath == NULL) {
        dfx_log_err("[ERR] get_fullpath: malloc error\r\n");
        return NULL;
    }

    (void)memset_s(fullpath, path_len, 0, path_len);

    if (path[1] != '\0') {
        ret = snprintf_s(fullpath, path_len, strlen(path) + strlen(file_name) + 1, "%s/%s", path, file_name);
    } else {
        ret = snprintf_s(fullpath, path_len, strlen(file_name) + 1, "/%s", file_name);
    }
    if (ret < 0) {
        dfx_log_err("[ERR] get_fullpath: snprintf_s error ret = %d\r\n", ret);
        dfx_free(0, fullpath);
        return NULL;
    }

    return fullpath;
}

STATIC errcode_t transmit_file_node_info(const char *fullpath, transmit_file_report_node_hook handler,
    uintptr_t usr_data, const char *node_name)
{
    transmit_file_ls_node_info_t info;
    struct stat stat_info;
    (void)memset_s(&stat_info, sizeof(struct stat), 0, sizeof(struct stat));

    if (stat(fullpath, &stat_info) == 0) {
        if (S_ISDIR(stat_info.st_mode)) {
            info.is_dir = true;
            info.file_size = 0;
            info.name = node_name;
            handler(&info, usr_data);
        } else {
            info.is_dir = false;
            info.file_size = (uint32_t)stat_info.st_size;
            info.name = node_name;
            handler(&info, usr_data);
        }
    }
    return ERRCODE_SUCC;
}

errcode_t transmit_file_ls(const char *ls_path, transmit_file_report_node_hook handler, uintptr_t usr_data)
{
    errcode_t ret;
    DIR *d = NULL;
    struct dirent *pdirent = NULL;
    char *fullpath = NULL;
    char *fullpath_bak = NULL;
    unused(ret);

    d = opendir(ls_path);
    if (d == NULL) {
        dfx_log_err("[ERR]opendir failed, errno is %d. \r\n", get_errno());
        goto free_res;
    }

    do {
        pdirent = readdir(d);
        if (pdirent != NULL) {
            fullpath = get_fullpath(ls_path, pdirent->d_name);
            if (fullpath == NULL) {
                goto free_res;
            }

            fullpath_bak = fullpath;
            transmit_file_node_info(fullpath, handler, usr_data, pdirent->d_name);
            dfx_free(0, fullpath_bak);
        }
    } while (pdirent != NULL);

free_res:
    (void)closedir(d);
    return ERRCODE_SUCC;
}

errcode_t transmit_file_delete(const char *path)
{
    char *real_path = NULL;

    real_path = realpath(path, NULL);
    if (real_path == NULL) {
        dfx_log_err("[ERR]realpath failed, errno is %d. \r\n", get_errno());
        return -1;
    }

    if (remove(real_path) != 0) {
        if (remove(real_path) != 0) {
            dfx_log_err("[ERR]remove failed, errno is %d. \r\n", get_errno());
            return ERRCODE_FAIL;
        }
    }

    if (access(real_path, R_OK) != 0) {
        return ERRCODE_SUCC;
    } else {
        if (remove(real_path) != 0) {
            dfx_log_err("[ERR]remove again failed, errno is %d. \r\n", get_errno());
            return ERRCODE_FAIL;
        } else {
            return ERRCODE_SUCC;
        }
    }
}

int32_t transmit_file_open_for_read(const char *path)
{
    char *real_path = NULL;

    real_path = realpath(path, NULL);
    if (real_path == NULL) {
        dfx_log_err("[ERR]realpath failed, errno is %d. \r\n", get_errno());
        return -1;
    }

    int32_t fd = open(real_path, O_RDONLY);
    free(real_path);
    if (fd < 0) {
        dfx_log_err("[ERR]open failed, errno is %d. \r\n", get_errno());
        return -1;
    }

    return fd;
}

int32_t transmit_file_open_for_write(char *path)
{
    int fd;
    fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0);
    if (fd < 0) {
        if (transmit_file_mkdir(path) != 0) {
            dfx_log_err("[ERR]transmit_file_mkdir failed. \r\n");
            return -2; /* -2: return code for mkdir failed */
        }
        fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0);
        if (fd < 0) {
            dfx_log_err("[ERR]open failed, errno is %d. \r\n", get_errno());
            return fd;
        }
    }
    return fd;
}

int32_t transmit_file_open_for_rewrite(const char *path)
{
    int fd;
    fd = open(path, O_RDWR | O_CREAT, 0);
    if (fd < 0) {
        dfx_log_err("[ERR]open failed, errno is %d. \r\n", get_errno());
    }
    return fd;
}

void transmit_file_fsync(int32_t fd)
{
#ifdef CFG_DRIVERS_NANDFLASH
    fsync(fd);
#endif
}

void transmit_file_close(int32_t fd)
{
    if (close(fd) != 0) {
        if (close(fd) != 0) {
            dfx_log_err("[ERR]close failed, errno is %d. \r\n", get_errno());
        }
    }
}

int32_t transmit_file_read_fd(int32_t fd, uint32_t offset, uint8_t *buf, uint32_t size, bool burst)
{
    int len;
    int ret;

    if (!burst) {
        ret = (int)lseek(fd, offset, SEEK_SET);
        if (ret < 0) {
            dfx_log_err("[ERR]lseek failed, errno is %d. \r\n", get_errno());
        }
    }

    len = read(fd, buf, size);
    if (len < 0) {
        dfx_log_err("[ERR]read failed, errno is %d. \r\n", get_errno());
    }
    return len;
}

int32_t transmit_file_mkdir(const char *path)
{
    int path_len = (int)strlen(path);
    if (path_len <= 0) {
        return -1;
    }

    char *str_path = (char *)dfx_malloc(0, (uint32_t)path_len + 1);
    if (str_path == NULL) {
        return -1;
    }
    (void)memset_s(str_path, (uint32_t)path_len + 1, 0, (uint32_t)path_len + 1);
    if (strcpy_s(str_path, (uint32_t)path_len + 1, path) != EOK) {
        dfx_free(0, str_path);
        return -1;
    }

    for (int i = 0; i < path_len; i++) {
        if (i > 0 && str_path[i] == '/') {
            str_path[i] = '\0';
            if (access(str_path, F_OK) == 0) {
                str_path[i] = '/';
                continue;
            }

            if (mkdir(str_path, S_IREAD | S_IWRITE) != 0) {
                dfx_log_err("[ERR]mkdir %s failed, errno is %d. \r\n", str_path, get_errno());
                dfx_free(0, str_path);
                return -1;
            }
            str_path[i] = '/';
        }
    }
    dfx_free(0, str_path);
    return 0;
}

errcode_t transmit_file_rmdir(const char *path)
{
    DIR *dirp;
    struct dirent *dp;
    struct stat stat_info;
    char *dir_name = NULL;
    errcode_t ret;

    /* 目录不存在，直接返回 */
    if (access(path, F_OK) != 0) {
        dfx_log_err("rmdir: dir is not exsit\r\n");
        return ERRCODE_FAIL;
    }

    /* 获取目录属性失败，返回错误 */
    if (stat(path, &stat_info) < 0) {
        dfx_log_err("rmdir: get stat error\r\n");
        return ERRCODE_FAIL;
    }

    if (S_ISDIR(stat_info.st_mode)) {   /* 目录文件，递归删除目录中内容 */
        dirp = opendir(path);
        while ((dp = readdir(dirp)) != NULL) {
            /* 忽略 . 和 .. */
            if ((strcmp(dp->d_name, ".") == 0) || (strcmp(dp->d_name, "..") == 0)) {
                continue;
            }

            dir_name = get_fullpath(path, dp->d_name);
            if (dir_name == NULL) {
                closedir(dirp);
                return ERRCODE_MALLOC;
            }
            ret = transmit_file_rmdir((const char *)dir_name);   /* 递归删除 */
            if (ret != ERRCODE_SUCC) {
                dfx_free(0, dir_name);
                closedir(dirp);
                return ret;
            }
            dfx_free(0, dir_name);
        }
        closedir(dirp);

        if (rmdir(path) != 0) { /* 删除空目录 */
            dfx_log_err("[ERR]rmdir %s failed, errno is %d. \r\n", path, get_errno());
            return (errcode_t)get_errno();
        }
    } else {
        // 普通文件直接删除
        if (remove(path) != 0) {
            dfx_log_err("[ERR]remove %s failed, errno is %d. \r\n", path, get_errno());
            return (errcode_t)get_errno();
        }
    }

    return ERRCODE_SUCC;
}

int32_t transmit_file_write(char *path, unsigned offset, const uint8_t *buf, uint32_t size)
{
    int fd;
    int ret;
    ssize_t len;
    fd = open(path, O_RDWR | O_CREAT, 0);
    if (fd < 0) {
        if (transmit_file_mkdir(path) != 0) {
            dfx_log_err("[ERR]transmit_file_mkdir failed. \r\n");
            return -1;
        }
        fd = open(path, O_RDWR | O_CREAT, 0);
        if (fd < 0) {
            dfx_log_err("[ERR]open failed, errno is %d. \r\n", get_errno());
            return -1;
        }
    }

    int pos = (int)lseek(fd, offset, SEEK_SET);
    if (pos < 0) {
        dfx_log_err("[ERR]lseek failed, errno is %d. \r\n", get_errno());
    }

    len = write(fd, buf, size);
    if (len < 0) {
        dfx_log_err("[ERR]write failed, errno is %d. \r\n", get_errno());
    }

    ret = close(fd);
    if (ret < 0) {
        dfx_log_err("[ERR]close failed, errno is %d. \r\n", get_errno());
    }

    return len;
}

int32_t transmit_file_write_fd(int fd, unsigned offset, const uint8_t *buf, uint32_t size)
{
    ssize_t len;

    int pos = (int)lseek(fd, offset, SEEK_SET);
    if (pos < 0) {
        dfx_log_err("[ERR]lseek failed, errno is %d. \r\n", get_errno());
    }

    len = write(fd, buf, size);
    if (len < 0) {
        dfx_log_err("[ERR]write failed, errno is %d. \r\n", get_errno());
    }

    return len;
}

int32_t transmit_file_read(const char *path, uint32_t offset, uint8_t *buf, uint32_t size)
{
    int len;
    int ret;
    char *real_path = NULL;

    real_path = realpath(path, NULL);
    if (real_path == NULL) {
        dfx_log_err("[ERR]realpath failed, errno is %d. \r\n", get_errno());
        return -1;
    }

    int fd = open(real_path, O_RDONLY);
    free(real_path);
    if (fd < 0) {
        dfx_log_err("[ERR]open failed, errno is %d. \r\n", get_errno());
        return -1;
    }

    ret = (int)lseek(fd, offset, SEEK_SET);
    if (ret < 0) {
        dfx_log_err("[ERR]lseek failed, errno is %d. \r\n", get_errno());
    }
    len = read(fd, buf, size);
    if (len < 0) {
        dfx_log_err("[ERR]read failed, errno is %d. \r\n", get_errno());
    }
    ret = close(fd);
    if (ret < 0) {
        dfx_log_err("[ERR]close failed, errno is %d. \r\n", get_errno());
    }

    return len;
}
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */
#endif /* CONFIG_DFX_SUPPORT_TRANSMIT_FILE */
