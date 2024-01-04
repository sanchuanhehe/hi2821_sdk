/*
 * Copyright (c) CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:  BUILD VERSION INFORMATION MODULE
 * Author:
 * Create:
 */

#include "build_version.h"
#include "sdk_version.h"
#include "build_version_info.h"
#include "chip_core_definition.h"
#include "memory_config.h"
#include "stdlib.h"

// lint complains about this not being set, so set it in case
#if !(defined VERSION_BUILD_TIME)
#define VERSION_BUILD_TIME 0
#endif

#if !(defined BUILD_TIME_STRING)
#define BUILD_TIME_STRING "unknown"
#endif

#if !(defined FIRMWARE_VERSION_STRING)
#if (defined APPLICATION_VERSION_STRING)
#define FIRMWARE_VERSION_STRING APPLICATION_VERSION_STRING
#else
#define FIRMWARE_VERSION_STRING "unknown"
#endif
#endif

#if !(defined FIRMWARE_GIT_HASH)
#define FIRMWARE_GIT_HASH "unknown"
#endif
#define MAX_VERSION_STRING 10

#if !(defined BUILD_VERSION_ID)
#define BUILD_VERSION_ID "unknown"
#endif

#if !(defined BUILD_VERSION_ID_DSP)
#define BUILD_VERSION_ID_DSP "unknown"
#endif

#if !(defined BUILD_BRANCH_DSP)
#define BUILD_BRANCH_DSP "unknown"
#endif

#if !(defined BUILD_CHANGE_ID_DSP)
#define BUILD_CHANGE_ID_DSP "unknown"
#endif

#if !(defined BUILD_VERSION)
#define BUILD_VERSION "user"
#endif

#if !(defined BUILD_BRANCH)
#define BUILD_BRANCH "unknown"
#endif

#if !(defined BUILD_CHANGE_ID)
#define BUILD_CHANGE_ID "unknown"
#endif

// Version String too long check
#define ct_assert(e) enum LENGTH_CHECK { ct_assert_value = 1 / ((!(!(e)))) }
ct_assert((sizeof(FIRMWARE_VERSION_STRING)) <= 48); //lint !e514 !e19

/* Populate version information */
#ifdef BUILD_APPLICATION_ROM
#define VERSION_STRING BUILD_TIME_STRING " " APPLICATION_VERSION_STRING
// plt_patch.py has process g_build_version_information, if change it in .c, need change at same time.
static const build_version_info_rom g_build_version_information __attribute__((section(BUILD_VERSION_SECTION))) = {
    CORE_VERSION,
    VERSION_BUILD_TIME,
    { VERSION_STRING }};
#else
static const build_version_info g_build_version_information __attribute__((section(".flash_version"))) = {
    CORE_VERSION,
    BUILD_VERSION_MAGIC_NUMBER,
    BUILD_VERSION_INFO_VERSION,
    0, // padding
    0, // it's means ssb length if target is ssb
    FIRMWARE_VERSION_STRING,
    {  HIFI0_IMAGE_PAGES, BT_IMAGE_PAGES, RECOVERY_IMAGE_PAGES, APP_IMAGE_PAGES },
    FIRMWARE_GIT_HASH,
};

/*lint -esym(528, g_build_version_id)*/
static const char g_build_version_id[] __attribute__((section(".change_id"))) = BUILD_VERSION_ID;
static const char g_build_version_id_dsp[] __attribute__((section(".dsp_change_id"))) = BUILD_VERSION_ID_DSP;

static const char g_build_branch[] __attribute__((section(".ramtext"))) = BUILD_BRANCH;
static const char g_build_change_id[] __attribute__((section(".ramtext"))) = BUILD_CHANGE_ID;
static const char g_build_branch_dsp[] __attribute__((section(".ramtext"))) = BUILD_BRANCH_DSP;
static const char g_build_change_id_dsp[] __attribute__((section(".ramtext"))) = BUILD_CHANGE_ID_DSP;
static const char g_build_version[] __attribute__((section(".ramtext"))) = BUILD_VERSION;
#endif // #if (defined BUILD_APPLICATION_ROM)

const build_version_info *build_version_get_info(void)
{
    return (build_version_info *)&g_build_version_information;
}

// probably should return max length
const char *get_version_string(void)
{
    return g_build_version_information.string;
}

const char *uapi_sdk_read_id(void)
{
    return get_version_string();
}

const char *get_git_hash(void)
{
#if (defined BUILD_APPLICATION_ROM)
    return NULL;
#else
    return g_build_version_information.hash;
#endif
}

#if !defined(BUILD_APPLICATION_ROM)
const char *get_version_build_mode(void)
{
    return g_build_version;
}

const char *get_version_branch(void)
{
    return g_build_branch;
}

const char *get_version_branch_dsp(void)
{
    return g_build_branch_dsp;
}

const char *get_version_change_id(void)
{
    return g_build_change_id;
}

const char *get_version_change_id_dsp(void)
{
    return g_build_change_id_dsp;
}

const char *get_version_id(void)
{
    return g_build_version_id;
}

const char *get_version_id_dsp(void)
{
    return g_build_version_id_dsp;
}
#endif

#ifndef VERSION_STRING
#define VERSION_STRING      "unknown"
#endif
#ifndef SDK_VERSION_STRING
#define SDK_VERSION_STRING  "unknown"
#endif
#define SSB_USER_VERSION_OFFSET   16
#define SSB_VERSION_MASK          0xFFFF

uint16_t g_build_version_ssb_user;
uint16_t g_build_version_ssb_sdk;

const char *uapi_get_version(void)
{
    return (const char *)VERSION_STRING;
}

const char *uapi_get_sdk_version(void)
{
    return (const char *)SDK_VERSION_STRING;
}

void uapi_set_ssb_version(uint32_t version)
{
    g_build_version_ssb_user = (uint16_t)((version >> SSB_USER_VERSION_OFFSET) & SSB_VERSION_MASK);
    g_build_version_ssb_sdk = (uint16_t)(version & SSB_VERSION_MASK) ;
}

uint16_t uapi_get_ssb_version(void)
{
    return g_build_version_ssb_user;
}

uint16_t uapi_get_ssb_sdk_version(void)
{
    return g_build_version_ssb_sdk;
}