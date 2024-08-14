#include "flash_config_info.h"

#define _4K                             0x1000
#define _32K                            0x8000
#define _64K                            0x10000
#define CHIP_SIZE                       0x3ffff

#define FLASH_SIZE_512KB                0x80000
#define FLASH_SIZE_1MB                  0x100000
#define FLASH_SIZE_4MB                  0x400000
#define FLASH_SIZE_8MB                  0x800000
#define FLASH_SIZE_16MB                 0x1000000
#define FLASH_SIZE_32MB                 0x2000000

#define DISABLE                         0x0
#define ENABLE                          0x1
#define FLASH_MANUFACTURER_MAX          25
#define SPI_CMD_UNSUPPORT               {0x0, 0x0, 0x0, 0x1}
#define WR_ENABLE                       0x6

#define SFC_SUPPORT_IF_TYPE             0x6

#define FLASH_W25Q32                    0x1660EF
#define FLASH_W25Q64                    0x1760EF
#define FLASH_W25Q80                    0x1460EF
#define FLASH_W25Q40                    0x1360EF
#define FLASH_P25Q80                    0x146085
#define FLASH_GD25WD40                  0x1364C8
#define FLASH_GD25LE40                  0x1360C8
#define FLASH_GD25LE80                  0x1460C8
#define FLASH_EN25S80                   0x14381C
#define FLASH_UNKOWN                    0xFFFFFF

static const flash_cmd_execute_t g_default_quad_enable[] = {
    { FLASH_CMD_TYPE_END,         0, { 0x0 }}
};

static const spi_opreation_t g_default_read_cmds[] = {
    {SPI_CMD_SUPPORT, 0x03, 0x0, 0x0},
};

static const spi_opreation_t g_default_write_cmds[] = {
    {SPI_CMD_SUPPORT, 0x0, 0x0, 0},
    {SPI_CMD_SUPPORT, 0x02, 0x0, 0},
};

static const spi_opreation_t g_default_erase_cmds[] = {
    {SPI_CMD_SUPPORT, 0xC7, 0x0, CHIP_SIZE},
    {SPI_CMD_SUPPORT, 0xD8, 0x0, _64K},
    {SPI_CMD_SUPPORT, 0x20, 0x0, _4K},
};

static const flash_cmd_execute_t g_flash_winbond_bus_enable[] = {
    { FLASH_CMD_TYPE_CMD,         1, { WR_ENABLE }},
    { FLASH_CMD_TYPE_CMD,         2, { 0x31, 0x02 } },
    { FLASH_CMD_TYPE_PROCESSING,  3, { 0x05, 0, DISABLE } },
    { FLASH_CMD_TYPE_PROCESSING,  3, { 0x35, 1, ENABLE } },
    { FLASH_CMD_TYPE_END,         0, { 0x0 }}
};

static const flash_cmd_execute_t g_flash_common_bus_enable[] = {
    { FLASH_CMD_TYPE_CMD,         1, { WR_ENABLE }},
    { FLASH_CMD_TYPE_PROCESSING,  3, { 0x05, 0, DISABLE } },
    { FLASH_CMD_TYPE_END,         0, { 0x0 }}
};

static const flash_cmd_execute_t g_flash_gd_bus_enable[] = {
    { FLASH_CMD_TYPE_CMD,         1, { WR_ENABLE }},
    { FLASH_CMD_TYPE_PROCESSING,  3, { 0x05, 0, DISABLE } },
    { FLASH_CMD_TYPE_CMD,         3, { 0x1, 0x0, 0x2 } },
    { FLASH_CMD_TYPE_END,         0, { 0x0 }}
};

static const spi_opreation_t g_flash_common_read_cmds[] = {
    {SPI_CMD_SUPPORT, 0x03, 0x0, 0x0},
    {SPI_CMD_SUPPORT, 0x0B, 0x0, 0x1},
    {SPI_CMD_SUPPORT, 0x3B, 0x1, 0x1},
    {SPI_CMD_SUPPORT, 0xBB, 0x2, 0x1},
    {SPI_CMD_SUPPORT, 0x6B, 0x5, 0x1},
    {SPI_CMD_SUPPORT, 0xEB, 0x6, 0x3}
};

static const spi_opreation_t g_flash_gd25wd40_read_cmds[] = {
    {SPI_CMD_SUPPORT, 0x03, 0x0, 0x0},
    {SPI_CMD_SUPPORT, 0x0B, 0x0, 0x1},
    {SPI_CMD_SUPPORT, 0x3B, 0x1, 0x1},
    SPI_CMD_UNSUPPORT,
    SPI_CMD_UNSUPPORT,
    SPI_CMD_UNSUPPORT,
};

static const spi_opreation_t g_flash_esmt_read_cmds[] = {
    {SPI_CMD_SUPPORT, 0x03, 0x0, 0x0},
    {SPI_CMD_SUPPORT, 0x0B, 0x0, 0x1},
    {SPI_CMD_SUPPORT, 0x3B, 0x1, 0x1},
    {SPI_CMD_SUPPORT, 0xBB, 0x2, 0x1},
    SPI_CMD_UNSUPPORT,
    SPI_CMD_UNSUPPORT,
};

static const spi_opreation_t g_flash_common_write_cmds[] = {
    {SPI_CMD_SUPPORT, 0x0, 0x0, 0},
    {SPI_CMD_SUPPORT, 0x02, 0x0, 0},
    SPI_CMD_UNSUPPORT,
    SPI_CMD_UNSUPPORT,
    {SPI_CMD_SUPPORT, 0x32, 0x5, 0},
    SPI_CMD_UNSUPPORT,
};

static const spi_opreation_t g_flash_common_erase_cmds[] = {
    {SPI_CMD_SUPPORT, 0xC7, 0x0, CHIP_SIZE},
    {SPI_CMD_SUPPORT, 0xD8, 0x0, _64K},
    {SPI_CMD_SUPPORT, 0x52, 0x0, _32K},
    {SPI_CMD_SUPPORT, 0x20, 0x0, _4K}
};

static const flash_spi_info_t g_flash_spi_infos[] = {
    { FLASH_W25Q32, FLASH_SIZE_4MB, 4,
      (spi_opreation_t *)g_flash_common_read_cmds, (spi_opreation_t *)g_flash_common_write_cmds,
      (spi_opreation_t *)g_flash_common_erase_cmds, (flash_cmd_execute_t *)g_flash_winbond_bus_enable },
    { FLASH_W25Q64, FLASH_SIZE_8MB, 4,
      (spi_opreation_t *)g_flash_common_read_cmds, (spi_opreation_t *)g_flash_common_write_cmds,
      (spi_opreation_t *)g_flash_common_erase_cmds, (flash_cmd_execute_t *)g_flash_winbond_bus_enable },
    { FLASH_W25Q40, FLASH_SIZE_512KB, 4,
      (spi_opreation_t *)g_flash_common_read_cmds, (spi_opreation_t *)g_flash_common_write_cmds,
      (spi_opreation_t *)g_flash_common_erase_cmds, (flash_cmd_execute_t *)g_flash_winbond_bus_enable },
    { FLASH_W25Q80, FLASH_SIZE_1MB, 4,
      (spi_opreation_t *)g_flash_common_read_cmds, (spi_opreation_t *)g_flash_common_write_cmds,
      (spi_opreation_t *)g_flash_common_erase_cmds, (flash_cmd_execute_t *)g_flash_winbond_bus_enable },
    { FLASH_GD25LE40, FLASH_SIZE_512KB, 4,
      (spi_opreation_t *)g_flash_common_read_cmds, (spi_opreation_t *)g_flash_common_write_cmds,
      (spi_opreation_t *)g_flash_common_erase_cmds, (flash_cmd_execute_t *)g_flash_gd_bus_enable },
    { FLASH_GD25WD40, FLASH_SIZE_512KB, 4,
      (spi_opreation_t *)g_flash_gd25wd40_read_cmds, (spi_opreation_t *)g_flash_common_write_cmds,
      (spi_opreation_t *)g_flash_common_erase_cmds, (flash_cmd_execute_t *)g_flash_common_bus_enable },
    { FLASH_GD25LE80, FLASH_SIZE_1MB, 4,
      (spi_opreation_t *)g_flash_common_read_cmds, (spi_opreation_t *)g_flash_common_write_cmds,
      (spi_opreation_t *)g_flash_common_erase_cmds, (flash_cmd_execute_t *)g_flash_gd_bus_enable },
    { FLASH_EN25S80, FLASH_SIZE_1MB, 4,
      (spi_opreation_t *)g_flash_esmt_read_cmds, (spi_opreation_t *)g_flash_common_write_cmds,
      (spi_opreation_t *)g_flash_common_erase_cmds, (flash_cmd_execute_t *)g_flash_common_bus_enable },
    { FLASH_P25Q80, FLASH_SIZE_1MB, 4,
      (spi_opreation_t *)g_flash_common_read_cmds, (spi_opreation_t *)g_flash_common_write_cmds,
      (spi_opreation_t *)g_flash_common_erase_cmds, (flash_cmd_execute_t *)g_flash_common_bus_enable },
};

static const flash_spi_info_t g_flash_spi_unknown_info = {
    FLASH_UNKOWN, FLASH_SIZE_512KB, 3,
    (spi_opreation_t *)g_default_read_cmds, (spi_opreation_t *)g_default_write_cmds,
    (spi_opreation_t *)g_default_erase_cmds, (flash_cmd_execute_t *)g_default_quad_enable
};

static flash_spi_info_t* g_flash_spi_infos_ptr = (flash_spi_info_t *)g_flash_spi_infos;
static uint8_t flash_table_size = sizeof(g_flash_spi_infos) / sizeof(flash_spi_info_t);

flash_spi_info_t *sfc_port_get_flash_spi_infos(void)
{
    return (flash_spi_info_t *)g_flash_spi_infos_ptr;
}

void sfc_port_set_flash_spi_infos(flash_spi_info_t *flash_table, uint8_t table_size)
{
    g_flash_spi_infos_ptr = flash_table;
    flash_table_size = table_size;
}

uint32_t sfc_port_get_flash_num(void)
{
    return (uint32_t)flash_table_size;
}

flash_spi_info_t *sfc_port_get_unknown_flash_info(void)
{
    return (flash_spi_info_t *)&g_flash_spi_unknown_info;
}
