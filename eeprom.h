/**
 * @file eeprom.h
 * @brief Methods for using 25AA02E48 Eeprom under chibios
 * @author Kristoffer Ödmark
 * @version 0.1
 * @date 2015-09-14
 */
#ifndef EEPROM_H_
#define EEPROM_H_
#include "hal.h"
#include "ch.h"

#if 1 // To get printouts
#define printf(...) chprintf(&SD1, __VA_ARGS__)
#include "chprintf.h"
extern SerialDriver SD1;
#else
#define printf(...)
#endif

#define EEPROM_SPI_PORT     GPIOB



#define EEPROM_SPI_PORT     GPIOB

#define EEPROM_SPI_CS       5
#define EEPROM_SPI_SCK      13
#define EEPROM_SPI_MISO     14
#define EEPROM_SPI_MOSI     15

#define EEPROM_SPI_BUS      SPID1


//Address of the EUID preprogrammed into the chip
#define EEPROM_UID_ADDRESS  0xFA
//Write enable command
#define EEPROM_WREN         0x06
//Read status register command
#define EEPROM_RDSR         0x05
//Write status register command
#define EEPROM_WSR         0x01

static const SPIConfig eeprom_spicfg = {
  NULL,
  GPIOB,
  EEPROM_SPI_CS,
  SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,
  SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0
};

void eeprom_init(void);
void eeprom_read_uid48(uint8_t *uid);
void eeprom_write(uint8_t address, uint8_t *data, uint32_t length);
void eeprom_read(uint8_t address, uint8_t *data, uint32_t length);
void eeprom_set_status(uint8_t status);
uint8_t eeprom_get_status(void);
void eeprom_enable_write(void);

void eeprom_tests(void );

#endif
