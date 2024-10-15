
#ifndef SRC_I2C_FUNCTIONS_H_
#define SRC_I2C_FUNCTIONS_H_

#include <iostream>

#define Si570_dev               "/dev/i2c-1"
#define Si570_addr              0x5D

#define TCA9548_dev             "/dev/i2c-0"
#define TCA9548_addr            0x74

#define EEPROM_dev              "/dev/i2c-3"
#define EEMPROM_addr            0x54
#define EEPROM_PAGESIZE 16


#define Si570_Startup_Freq      148.5   // In MHz (Info from dts)



void Stat_check         (bool &status, std::string msg);

bool Write_Si570_Reg    (int &fd, unsigned char *addr, unsigned char *wr_data, size_t buf_size);
bool Write_Si570        (int &fd, unsigned char *wr_data, size_t buf_size);

bool Read_Si570_Reg     (int &fd, unsigned char *addr);
bool Read_Si570         (int &fd);

bool Write_EEPROM       (int &fd, unsigned char *addr, unsigned char *wr_data, size_t buf_size);

bool Read_EEPROM        (int &fd, unsigned char *addr);
bool Read_EEPROM        (int &fd, size_t buf_size);

bool Freq_calculation   (unsigned char *read_buf, size_t buf_size);

#endif /* SRC_I2C-FUNCTIONS_H_ */