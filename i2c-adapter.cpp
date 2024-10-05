#include <string.h>
#include <iostream>
#include <fcntl.h>  
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c-functions.h"

// #include "device.h"

// https://habr.com/ru/articles/413249/
// https://www.kernel.org/doc/Documentation/i2c/dev-interface
// https://www.abelectronics.co.uk/kb/article/1092/i2c-part-3-i2c-tools-in-linux#:~:text=Available%20Commands%20in%20I2C%20Tools&text=i2cdump%20Examine%20and%20read%20I2C,transfer%20to%20a%20connected%20device.


int main(){
    bool status;
    int fd;

    std::string syspath = "/sys/class/i2c-dev";

    std::string clkname;
    clkname.append(syspath).append("/i2c-1/device/1-005d/name");

    std::cout << "\n START PROGRAMM \n";
    
    fd = Open_dev(Si570_dev);

    // for (int i = 7; i < 13; i++){
    //     status = Read_Si570_Reg(fd, i);
    //     Stat_check(status, "\n FAILED READING FROM Si570 \n");
    // }

    Read_Si570(fd);

    // fd = Open_dev(EEPROM_dev);

    // status = Write_EEPROM(fd, 0xFF);
    // Stat_check(status, "\n FAILED WRITING TO EEPROM \n");

    // status = Read_EEPROM(fd);
    // Stat_check(status, "\n FAILED READING FROM EEPROM \n");

    return 0;
}