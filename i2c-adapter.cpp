#include <string.h>
#include <iostream>
#include <fcntl.h>  
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

#include "i2c-functions.h"
#include "iic_rw.h"

int main(){
    bool status;
    int fd;

    unsigned char addr = 0x0; 
    unsigned char write_Si570_data[6] = {0xA0, 0xC2, 0xEC, 0x71, 0xF, 0x42};

    unsigned char write_data_Byte[1]  = {0xF1};
    unsigned char write_data_Page[EEPROM_PAGESIZE] = {0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    unsigned char write_data_PageClean[EEPROM_PAGESIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    std::string syspath = "/sys/class/i2c-dev";

    std::string clkname;
    clkname.append(syspath).append("/i2c-1/device/1-005d/name");

    std::cout << "\n START PROGRAMM \n";
    
    Open_dev(fd, Si570_dev);

    status = activate_slave(fd, Si570_addr);                // Maybe get this function inside other functions
    Stat_check(status, "\n CANNOT REACH SLAVE (Si570) \n");


    status = Write_Si570(fd, write_Si570_data, sizeof(write_Si570_data));

    status = Read_Si570(fd);
    Stat_check(status, "\n READ FROM ALL REGS - FAILED \n");

    Open_dev(fd, EEPROM_dev);

    status = activate_slave(fd, EEMPROM_addr);
    Stat_check(status, "\n CANNOT REACH SLAVE (EEPROM) \n");



    std::cout << "\n\n====================== BYTE EEPROM WRITE ===================\n\n";

    status = Write_EEPROM(fd, &addr, write_data_Byte, sizeof(write_data_Byte));
    Stat_check(status, "\n FAILED WRITING TO EEPROM \n");

    std::cout << "\n\n====================== CURRENT EEPROM READ ===================\n\n";

    status = Read_EEPROM(fd, 1);
    Stat_check(status, "\n FAILED READING FROM EEPROM \n");

    std::cout << "\n\n====================== PAGE EEPROM WRITE ===================\n\n";

    status = Write_EEPROM(fd, &addr, write_data_Page, sizeof(write_data_Page));
    Stat_check(status, "\n FAILED WRITING TO EEPROM \n");

    std::cout << "\n\n====================== RANDOM EEPROM READ ===================\n\n";

    status = Read_EEPROM(fd, &addr);
    Stat_check(status, "\n FAILED READING FROM EEPROM \n");

    std::cout << "\n\n====================== SEQUENTIAL EEPROM READ ===================\n\n";

    status = Read_EEPROM(fd, EEPROM_PAGESIZE);
    Stat_check(status, "\n FAILED READING FROM EEPROM \n");

    return 0;
}