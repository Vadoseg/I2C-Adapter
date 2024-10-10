#include <string.h>
#include <iostream>
#include <fcntl.h>  
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c-functions.h"


int main(){
    bool status;
    int fd;


    std::string syspath = "/sys/class/i2c-dev";

    std::string clkname;
    clkname.append(syspath).append("/i2c-1/device/1-005d/name");

    std::cout << "\n START PROGRAMM \n";
    
    fd = Open_dev(Si570_dev);

    status = Read_Si570(fd);
    Stat_check(status, "\n READ FROM ALL REGS - FAILED \n");

    fd = Open_dev(EEPROM_dev);

    if (ioctl(fd, I2C_SLAVE_FORCE, EEMPROM_addr) < 0){
        std::cout << "\n CANNOT REACH SLAVE (EEPROM) \n";
        return false;
    }

    status = Write_EEPROM(fd, 0xFF);
    Stat_check(status, "\n FAILED WRITING TO EEPROM \n");

    status = Read_EEPROM(fd);
    Stat_check(status, "\n FAILED READING FROM EEPROM \n");

    return 0;
}