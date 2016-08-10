/**
 * @file eeprom.c
 * @brief Methods for using 25AA02E48 Eeprom under chibios
 * @author Kristoffer Ödmark
 * @version 0.1
 * @date 2015-09-14
 */
#include "eeprom.h"

void eeprom_select(void){
    spiAcquireBus(&EEPROM_SPI_BUS);  /* Acquire ownership of the bus.*/
    spiSelect(&EEPROM_SPI_BUS);      /* Slave Select assertion. */

}

void eeprom_unselect(void){
    spiUnselect(&EEPROM_SPI_BUS);       /* Slave Select de-assertion.*/
    spiReleaseBus(&EEPROM_SPI_BUS);     /* Ownership release.*/

}
void eeprom_init(void){
    palSetPad(GPIOB, EEPROM_SPI_CS);
    spiStart(&SPID2, &eeprom_spicfg);
}

void eeprom_write(uint8_t address,  uint8_t *data, uint32_t length){

    do{

        int writelen = length;
        if(length > 16){
            writelen = 16;
        }
        length -= writelen;

        eeprom_enable_write();

        eeprom_select();

        uint8_t header [2] = {0x02, address};
        spiSend(&EEPROM_SPI_BUS, 2, header);
        spiSend(&EEPROM_SPI_BUS, writelen, data);
        eeprom_unselect();

        //change where next writecycle writes and from where in the write array
        if(length > 0){
            address += writelen;
            data += writelen;
        }

        uint8_t status = eeprom_get_status();
        while ( status & 0x01){
            status = eeprom_get_status();
        }
    } while(length > 0);

}

void eeprom_enable_write(void){
    eeprom_select();
    uint8_t temp = EEPROM_WREN;
    spiSend(&EEPROM_SPI_BUS, 1, &temp);
    eeprom_unselect();
}
void eeprom_read(uint8_t address, uint8_t *data, uint32_t length){
    eeprom_select();
    uint8_t header [2] = {0x03, address};
    spiSend(&EEPROM_SPI_BUS, 2, header);
    spiReceive(&EEPROM_SPI_BUS, length, data);
    eeprom_unselect();
}

uint8_t eeprom_get_status(){
    eeprom_select();
    uint8_t status = EEPROM_RDSR;
    spiSend(&EEPROM_SPI_BUS, 1, &status);
    status = 0;
    spiReceive(&EEPROM_SPI_BUS, 1, &status);
    eeprom_unselect();
    return status;
}

void eeprom_tests(){
    printf("EEPROM TESTS:\n\r");

    // test if write enable latch is set
    printf("\tEnabling write: ");
    eeprom_enable_write();
    chThdSleepMilliseconds(100);
    uint8_t status = eeprom_get_status();
    if(status & 0x02){
        printf("Pass\n\r");
    }
    else{
        printf("Fail - %x\n\r", status);
    }

    chThdSleepMilliseconds(100);
    // test write and read from eeprom
    uint32_t testint = random_int();
    eeprom_write(0x00, (uint8_t *)&testint, sizeof(uint32_t));
    uint32_t verify = 0;
    eeprom_read(0x00, (uint8_t *)&verify, sizeof(uint32_t));
    printf("\tTesting write: ");
    if(verify == testint){
        printf("Pass - %i\n\r", verify);
    }
    else{
        printf("Fail - %i\n\r", verify);
    }

    chThdSleepMilliseconds(100);
    //test write larger than 16
    static uint8_t testarray[34]  = {
        [0] = 1,
        [1] = 2,
        [2] = 3,
        [3] = 4,
        [4] = 5,
        [5] = 6,
        [6] = 7,
        [7] = 8,
        [8] = 9,
        [9] = 10,
        [10] = 11,
        [11] = 12,
        [12] = 13,
        [13] = 14,
        [14] = 15,
        [15] = 16,
        [16] = 17,
        [17] = 18,
        [18] = 19,
        [19] = 20,
        [20] = 21,
        [21] = 22,
        [22] = 23,
        [23] = 24,
        [24] = 25,
        [25] = 26,
        [26] = 27,
        [27] = 28,
        [28] = 29,
        [29] = 30,
        [30] = 31,
        [31] = 32,
        [32] = 33,
        [33] = 34,
        };

    printf("\tWriting and reading back long array:\n\r");
    eeprom_write(0x00, testarray, sizeof(testarray) );
    static uint8_t verifyarray[sizeof(testarray)];
    eeprom_read(0x00, verifyarray, sizeof(testarray));

    for(unsigned int i = 0; i < sizeof(testarray); i++)
    {
        if(testarray[i] == verifyarray[i]) {
            printf("\tarray [%i]: pass\n\r", i);
        }
        else{
            printf("\tarray [%i]: fail\n\r", i);
        }

    }
    chThdSleepMilliseconds(100);
}
