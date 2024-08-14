/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Dynload ElfLib HeadFile
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
 * @defgroup los_dynload Dynamic loading
 * @ingroup kernel
 */

#ifndef _LOS_LD_ELFLIB_H
#define _LOS_LD_ELFLIB_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup los_dynload
 * Define an enum type indicates load strategy.
 *
 * Type of load strategy of dynamic load, ZIP means using zipped shared object, NOZIP means using normal shared object.
 */
enum LOAD_STRATEGY {
    ZIP,
    NOZIP
};

/**
 * @ingroup los_dynload
 * Define the structure of the parameters used for dynamic.
 *
 * Information of specified parameters passed in during dynamic load.
 */
typedef struct tagDynloadParam {
    enum LOAD_STRATEGY enLoadStrategy;
} DYNLOAD_PARAM_S;

/**
 * @ingroup los_dynload
 * @brief Register the dynamic parameters.
 *
 * @par Description:
 * This API is used to register the dynamic load parameters.
 * @attention
 * <ul>
 * <li></li>
 * </ul>
 *
 * @param dynloadParam    [IN] dynamic load parameters to be registered.
 *
 * @par Dependency:
 * <ul><li>los_ld_elflib.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_FindSymByName | LOS_LdDestroy
 * @since Huawei LiteOS V100R001C00
 */
extern VOID LOS_DynParamReg(const DYNLOAD_PARAM_S *dynloadParam);

/**
 * @ingroup los_dynload
 * @brief Load a shared object file.
 *
 * @par Description:
 * This API is used to load a shared object file under a particular module file path.
 * @attention
 * <ul>
 * <li>The parameter passed to this API should be a legal path of a shared object file.</li>
 * </ul>
 *
 * @param elfFileName [IN] Shared object file path.
 *
 * @retval NULL  The shared object file fails to be loaded.
 * @retval VOID* The shared object file is successfully loaded.
 * @par Dependency:
 * <ul><li>los_ld_elflib.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ModuleUnload
 * @since Huawei LiteOS V100R001C00
 */
extern VOID *LOS_SoLoad(const CHAR *elfFileName);

/**
 * @ingroup los_dynload
 * @brief Load a shared object file(.so) or relocatable object file(.o) from the memory.
 *
 * @par Description:
 * This API is used to load a shared or relocatable object file that is in memory.
 * @attention
 * <ul>
 * <li>The shared object file does not depend on other shared objects.</li>
 * </ul>
 *
 * @param elfFileName [IN] Shared or relocatable object file name.
 * @param fileNameLen [IN] The length of shared or relocatable object file name.
 * @param elfFileBuf  [IN] Shared or relocatable object file buffer in memory.
 * @param bufLen      [IN] the length of shared or relocatable file buffer in memory.
 *
 * @retval NULL  The shared or relocatable object file fails to be loaded.
 * @retval VOID* The shared or relocatable object file is successfully loaded.
 * @par Dependency:
 * <ul><li>los_ld_elflib.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ModuleUnload
 * @since Huawei LiteOS V200R003C00
 */
extern VOID *LOS_MemLoad(const CHAR *elfFileName, UINT32 fileNameLen,
                         const CHAR *elfFileBuf, UINT32 bufLen);

/**
 * @ingroup los_dynload
 * @brief Load an relocatable object file(.o).
 *
 * @par Description:
 * This API is used to load an relocatable object file under a particular module file path.
 * @attention
 * <ul>
 * <li>The parameter passed to this API should be a legal path of an relocatable object file.</li>
 * </ul>
 *
 * @param elfFileName [IN] Relocatable object file path.
 *
 * @retval NULL  The relocatable object file fails to be loaded.
 * @retval VOID* The relocatable object file is successfully loaded.
 * @par Dependency:
 * <ul><li>los_ld_elflib.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ModuleUnload
 * @since Huawei LiteOS V100R001C00
 */
extern VOID *LOS_ObjLoad(const CHAR *elfFileName);

/**
 * @ingroup los_dynload
 * @brief Unload a module.
 *
 * @par Description:
 * This API is used to unload a module with a particular module handle.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param handle  [IN] Module handle.
 *
 * @retval #LOS_NOK  The module fails to be unloaded.
 * @retval #LOS_OK   The module is successfully unloaded.
 * @par Dependency:
 * <ul><li>los_ld_elflib.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ObjLoad
 * @since Huawei LiteOS V100R001C00
 */
extern INT32 LOS_ModuleUnload(VOID *handle);

/**
 * @ingroup los_dynload
 * @brief Destroy a dynamic loader.
 *
 * @par Description:
 * This API is used to destroy a dynamic linker.
 * @attention
 * <ul>
 * <li>When dynamic loading is no longer needed, call this API to destroy the dynamic linker.</li>
 * </ul>
 *
 * @param None.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_ld_elflib.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_FindSymByName
 * @since Huawei LiteOS V100R001C00
 */
extern VOID LOS_LdDestroy(VOID);

/**
 * @ingroup los_dynload
 * @brief Search for a symbol address.
 *
 * @par Description:
 * This API is used to search for the address of a symbol according to a particular module handle and symbol name.
 * @attention
 * <ul>
 * <li>If the value of handle is NULL, Huawei LiteOS searches for symbols (including system symbols) in the global
 * symbol table. If handle is set to a valid module handle, Huawei LiteOS searches for symbols in the module that
 * comes with the module handle.</li>
 * </ul>
 *
 * @param handle    [IN] Module handle.
 * @param name      [IN] Name of the symbol to be searched for.
 *
 * @retval NULL  The symbol address is not found.
 * @retval VOID* Symbol address.
 * @par Dependency:
 * <ul><li>los_ld_elflib.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_LdDestroy
 * @since Huawei LiteOS V100R001C00
 */
extern VOID *LOS_FindSymByName(VOID *handle, CHAR *name);

/**
 * @ingroup los_dynload
 * @brief Add a default path.
 *
 * @par Description:
 * This API is used to add a path to default paths.
 * @attention
 * <ul>
 * <li></li>
 * </ul>
 *
 * @param path    [IN] Path to be added to default paths.
 *
 * @retval #LOS_NOK   The path is added unsuccessfully.
 * @retval #LOS_OK    The path is added successfully.
 * @par Dependency:
 * <ul><li>los_ld_elflib.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_FindSymByName | LOS_LdDestroy
 * @since Huawei LiteOS V100R001C00
 */
extern INT32 LOS_PathAdd(CHAR *path);

/**
 * @ingroup los_dynload
 * @brief Set the memory pool address used by dynload
 *
 * @par Description:
 * This API is used to set the memory pool address used by dynload.
 * @attention
 * <ul>
 * <li>The parameter passed to this API should be a legal memory pool address by managed with LiteOS's memory
 * algorithm, and whose value is outside of the LiteOS system memory</li>
 * </ul>
 *
 * @param memPool [IN] the memory pool address.
 *
 * @retval TRUE   Set successful.
 * @retval FALSE Set failed.
 * @par Dependency:
 * <ul><li>los_ld_elflib.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_ModuleUnload
 * @since Huawei LiteOS V200R002C00
 */
extern BOOL LOS_DynMemPoolSet(VOID *memPool);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LOS_LD_ELFLIB_H */
