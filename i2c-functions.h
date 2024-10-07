
#ifndef SRC_I2C_FUNCTIONS_H_
#define SRC_I2C_FUNCTIONS_H_

#include <iostream>
#include <cstdint>



#define Si570_dev       "/dev/i2c-1"
#define Si570_addr      0x5D

#define TCA9548_dev     "/dev/i2c-0"
#define TCA9548_addr    0x74

#define EEPROM_dev      "/dev/i2c-3"
#define EEMPROM_addr    0x54

typedef struct {

    uint8_t First_freq_bits    : 8;
    uint8_t Second_freq_bits   : 8;
    uint8_t Third_freq_bits    : 8;
    uint8_t Forth_freq_bits    : 8;
    uint8_t Fifth_freq_bits    : 8;    // There are 40 bits, but the last 2 bits belong to N1 - that is a divider

} Si570_DATA;

void Stat_check     (bool status, std::string msg);
int  Open_dev       (std::string dev_path);

bool Read_Si570_Reg (int fd, unsigned char reg_num);
bool Read_Si570     (int fd);

bool Write_EEPROM   (int fd, int write_data);
bool Read_EEPROM    (int fd);

template <size_t size>
float   Freq_calculaton(unsigned char (&read_buf)[size]);

#endif /* SRC_I2C-FUNCTIONS_H_ */