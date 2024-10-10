
#ifndef SRC_I2C_FUNCTIONS_H_
#define SRC_I2C_FUNCTIONS_H_

#include <iostream>

#define Si570_dev               "/dev/i2c-1"
#define Si570_addr              0x5D

#define TCA9548_dev             "/dev/i2c-0"
#define TCA9548_addr            0x74

#define EEPROM_dev              "/dev/i2c-3"
#define EEMPROM_addr            0x54

#define Si570_Startup_Freq      148.5   // In MHz (Info from dts)

void Stat_check         (bool status, std::string msg);
int  Open_dev           (std::string dev_path);

bool Read_Si570_Reg     (int fd, unsigned char reg_num);
bool Read_Si570         (int fd);

bool Write_EEPROM       (int fd, int write_data);
bool Read_EEPROM        (int fd);

template <size_t size>
bool Freq_calculation   (unsigned char (&read_buf)[size]);

#endif /* SRC_I2C-FUNCTIONS_H_ */