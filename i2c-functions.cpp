
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <fcntl.h> 
#include <unistd.h>
#include <sstream>
#include <cmath>

#include "i2c-functions.h"

static Si570_DATA read_regs;   // Try to make it global struct and with that there is no need to transmit it to the functions

/*****************************************************

    @brief      Checking status == true or false and sending
    a message
    
    @param      status true or false 
    @param      msg is a message when status == false

    @return     None
*****************************************************/
void Stat_check(bool status, std::string msg){
    
    if (status == false){
        std::cout << msg;
    }

}

/*****************************************************

    @brief      Opening device as fd
    
    @param      dev_path is descriptor of device path 

    @return     Opened file as integer
*****************************************************/
int Open_dev(std::string dev_path){
    int fd;

    fd = open (dev_path.c_str(), O_RDWR);

    if (fd == -1) {
    
        std::cout << "\nDevice not opened.\n";
        return -1;
    }

    return fd;
}

/*******************************************************************************

    @brief      Reading from Si570

    @param      fd is the device descriptor
    @param      reg_num is the register number that needs to be read

    @return     true if read from Si570 register was successful and false in other case.
*******************************************************************************/
bool Read_Si570_Reg(int fd, unsigned char reg_num){
    std::cout << "\n START READ Si570 REGISTER \n";

    bool status = false;

    unsigned char write_buf[1] = {reg_num};
    unsigned char read_buf[1]  = {0x00000000};
    
    if (ioctl(fd, I2C_SLAVE_FORCE, Si570_addr) < 0){
        std::cout << "\n CANNOT REACH SLAVE (Si570) \n";
        return status;
    }

    if (write(fd, write_buf, 1) != 1){
        std::cout << "\n Si570 WRITE - FAIL \n";
        return status;
    }

    if (read(fd, read_buf, 1) != 1){
        std::cout << "\n Si570 READ - FAIL \n";
        return status;
    }
    
    printf("\n READ HEX DATA FROM %d REG: 0x%X \n", reg_num, read_buf[0]);
    printf("\n READ DECIMAL DATA FROM %d REG: %d \n", reg_num, read_buf[0]);
    status = true;

    return status;
}

/*******************************************************************************

    @brief      Write data to EEPROM

    @param      fd is the device descriptor
    @param      write_data is needed to transmit

    @return     true if write to EEPROM was successful and false in other case.

*******************************************************************************/
bool Write_EEPROM(int fd, int write_data){
    std::cout << "\n START WRITING TO EEPROM \n";

    bool status = false;

    unsigned char write_buf[1] = {0xFF};

    if (ioctl(fd, I2C_SLAVE_FORCE, EEMPROM_addr) < 0){
        std::cout << "\n CANNOT REACH SLAVE (EEPROM) \n";
        return status;
    }

    if (write(fd, write_buf, 1) != 1){
        std::cout << "\n EEPROM WRITE - FAIL \n";
        return status;
    }
    
    status = true;

    return status;
}

/*******************************************************************************

    @brief      Read data from EEPROM

    @param      fd is the device descriptor

    @return     true if read from EEPROM was successful and false in other case.

*******************************************************************************/
bool Read_EEPROM(int fd){
    std::cout << "\n START READING FROM EEPROM \n";

    bool status = false;
    unsigned char read_buf[1] = {0x00};

    if (ioctl(fd, I2C_SLAVE_FORCE, EEMPROM_addr) < 0){
        std::cout << "\n CANNOT REACH SLAVE (EEPROM) \n";
        return status;
    }

    if (read(fd, read_buf, 1) != 1){
        std::cout << "\n EEPROM READ - FAIL \n";
        return status;
    }
    

    printf("\n CHECK READ = 0x%X \n", read_buf);
    printf("\n READ DECIMAL DATA: %d \n", read_buf);    // read_buf[0]
    status = true;
    
    return status;
}

/*******************************************************************************

    @brief      Reading from all Si570 registers at once

    @param      fd is the device descriptor

    @return     true if read from Si570 all registers was successful and false in other case.
*******************************************************************************/
bool Read_Si570(int fd){
    std::cout << "\n START FULL READ Si570 \n";

    bool status = false;

    unsigned char write_buf[6] = {7, 8, 9, 10, 11, 12};
    unsigned char read_buf[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    if (ioctl(fd, I2C_SLAVE_FORCE, Si570_addr) < 0){
        std::cout << "\n CANNOT REACH SLAVE (Si570) \n";
        return status;
    }

    for (int i = 0; i < 6; i++){

        if (write(fd, &write_buf[i], 1) != 1){
            std::cout << "\n READ ALL Si570 - FAIL ON " << i << " REG \n";
            return status;
        }

        if (read(fd, &read_buf[i], 1) != 1){
            std::cout << "\n READ ALL Si570 - FAIL ON " << i << " REG \n";
            return status;
        }

        // std::cout << "\n DATA FROM " << i << " REG: " << std::hex << read_buf[i];
        printf("\n CHECK READ = 0x%X \n", read_buf[i]);
    }

    Freq_calculaton(read_buf);

    status = true;

    return status;
}

/*******************************************************************************

    @brief      Calculation of frequency in MHz for Si570

    @param      read_buf is a buffer that stores the value from the Si570 registers 

    @return     returns fully calculated frequency from Si570
*******************************************************************************/
template <size_t size>
float Freq_calculaton(unsigned char (&read_buf)[size]){
    std::cout << "\n START CALCULATION OF FREQUENCY FROM Si570 \n";

    std::ostringstream ss;
    float result       = 0;
    float factory_fout = 156.25;  // In MHz (Info from dts)

    uint8_t HS_DIV   = (read_buf[0] >> 5 & 0b00000111) + 4;                                     // HS_DIV takes up only 3 bits and we need to add +4 bcs datasheet says this
    
    uint8_t N1       = (read_buf[0] << 3 & 0b1111100) + (read_buf[1] >> 6 & 0b0000011) + 1;     // We need 0b00000011, but we need to add +1 bcs datasheet says that data writes in registers with -1 (Look datasheet for more info)

    float RFREQ        = (read_buf[1] & 0b00111111);


    ss << std::hex << std::uppercase << RFREQ;   // Add 6 bits cut from register 8

    for (int i = 2; i < sizeof(read_buf); i++){
        ss << std::hex << std::uppercase << static_cast<int>(read_buf[i]);  // Add another registers (static_cast<int> need to change unsigned char to int)
    }

    std::cout << "\n RFREQ IN HEX: " << ss.str() << "\n";
    
    RFREQ = std::stoi(ss.str(), nullptr, 16); // Change string to int

    RFREQ = RFREQ / pow(2, 28);

    result = (factory_fout * HS_DIV * N1) / RFREQ;  // Formula

    std::cout << "\n REAL FREQUENCY: " << result << "\n";

    return result;
}