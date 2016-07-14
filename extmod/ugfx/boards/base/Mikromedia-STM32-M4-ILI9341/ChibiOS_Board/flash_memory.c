#include "hal.h"
#include "flash_memory.h"

static const unsigned short _SERIAL_FLASH_CMD_RDID  = 0x9F;    // 25P80
static const unsigned short _SERIAL_FLASH_CMD_READ  = 0x03;
static const unsigned short _SERIAL_FLASH_CMD_WRITE = 0x02;
static const unsigned short _SERIAL_FLASH_CMD_WREN  = 0x06;
static const unsigned short _SERIAL_FLASH_CMD_RDSR  = 0x05;
static const unsigned short _SERIAL_FLASH_CMD_ERASE = 0xC7;    // 25P80
static const unsigned short _SERIAL_FLASH_CMD_EWSR  = 0x06;    // 25P80
static const unsigned short _SERIAL_FLASH_CMD_WRSR  = 0x01;
static const unsigned short _SERIAL_FLASH_CMD_SER   = 0xD8;    //25P80

static const SPIConfig flash_spicfg = {
  0,
  GPIOD,
  GPIOD_FLASH_CS,
  0
};

bool flash_is_write_busy(void) {
  static uint8_t is_write_busy_cmd[1];
  is_write_busy_cmd[0] = _SERIAL_FLASH_CMD_RDSR;
  
  uint8_t result[1];

  spiAcquireBus(&SPID3);
  spiStart(&SPID3, &flash_spicfg);
  spiSelect(&SPID3);
  spiSend(&SPID3, sizeof(is_write_busy_cmd), is_write_busy_cmd);
  spiReceive(&SPID3, sizeof(result), result);
  spiUnselect(&SPID3);
  spiReleaseBus(&SPID3);

  return result[0]&0x01;
}

void flash_write_enable(void) {
  spiAcquireBus(&SPID3);
  spiStart(&SPID3, &flash_spicfg);
  spiSelect(&SPID3);
  spiSend(&SPID3, 1, &_SERIAL_FLASH_CMD_WREN);
  spiUnselect(&SPID3);
  spiReleaseBus(&SPID3);
}

void flash_sector_erase(uint32_t sector) {
  flash_write_enable();
  static uint8_t sector_erase_cmd[4];
  sector_erase_cmd[0] = _SERIAL_FLASH_CMD_SER;
  sector_erase_cmd[1] = (sector >> 16) & 0xFF;
  sector_erase_cmd[2] = (sector >> 8) & 0xFF;
  sector_erase_cmd[3] = sector & 0xFF;


  spiAcquireBus(&SPID3);
  spiStart(&SPID3, &flash_spicfg);
  spiSelect(&SPID3);
  spiSend(&SPID3, sizeof(sector_erase_cmd), sector_erase_cmd);
  spiUnselect(&SPID3);
  spiReleaseBus(&SPID3);

  /* wait for complete */
  while(flash_is_write_busy());
}

void flash_read(uint32_t address, size_t bytes, uint8_t *out) {
  static uint8_t sector_read_cmd[4];
  sector_read_cmd[0] = _SERIAL_FLASH_CMD_READ;
  sector_read_cmd[1] = (address >> 16) & 0xFF;
  sector_read_cmd[2] = (address >> 8) & 0xFF;
  sector_read_cmd[3] = address & 0xFF;

  spiAcquireBus(&SPID3);
  spiStart(&SPID3, &flash_spicfg);
  spiSelect(&SPID3);
  spiSend(&SPID3, sizeof(sector_read_cmd), sector_read_cmd);
  spiReceive(&SPID3, bytes, out);
  spiUnselect(&SPID3);
  spiReleaseBus(&SPID3);
}

void flash_write(uint32_t address, size_t bytes, const uint8_t *data) {
  static uint8_t flash_write_cmd[4];

  flash_write_enable();

  flash_write_cmd[0] = _SERIAL_FLASH_CMD_WRITE;
  flash_write_cmd[1] = (address >> 16) & 0xFF;
  flash_write_cmd[2] = (address >> 8) & 0xFF;
  flash_write_cmd[3] = address & 0xFF;

  spiAcquireBus(&SPID3);
  spiStart(&SPID3, &flash_spicfg);
  spiSelect(&SPID3);
  spiSend(&SPID3, sizeof(flash_write_cmd), flash_write_cmd);
  spiSend(&SPID3, bytes, data);
  spiUnselect(&SPID3);
  spiReleaseBus(&SPID3);

  /* wait for complete */
  while(flash_is_write_busy());
}

bool flash_tp_calibrated(void) {
  uint8_t out[1];
  flash_read(0x0F0000, 1, out);

  return (out[0] == 0x01);
}

void flash_tp_calibration_save(uint16_t instance, const uint8_t *calbuf, size_t sz) {
  if (instance) return;
  flash_sector_erase(0x0F0000);
  uint8_t calibrated = 0x01;
  flash_write(0x0F0000, 1, &calibrated);
  flash_write(0x0F0001, sz, calbuf);
}
const char *flash_tp_calibration_load(uint16_t instance) {
  static uint8_t foo[24];

  if (instance) return 0;
  if (!flash_tp_calibrated()) return 0;
  flash_read(0x0F0001, 24, foo);

  return (char *)foo;
}
