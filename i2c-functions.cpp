
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <fcntl.h> 
#include <unistd.h>

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
bool Read_Si570_Reg(int fd, int reg_num){
    std::cout << "\n START READ Si570 REGISTER \n";

    bool status = false;

    unsigned char write_buf[1] = {reg_num};
    unsigned char read_buf[1]  = {0x00};
    
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

    unsigned char write_buf[1] = {0};
    unsigned char read_buf[5] = {read_regs.First_freq_bits, read_regs.Second_freq_bits, read_regs.Third_freq_bits, read_regs.Forth_freq_bits, read_regs.Fifth_freq_bits};

    write(fd, write_buf, 1);

    if (read(fd, (unsigned char *)read_regs.First_freq_bits, 1) != 1){        // TEST OF (unsigned char *)read_regs.First_freq_bits - DATA IN First_freq_bits NEED TO CHANGE
        std::cout << "\n READ ALL Si570 - FAIL\n";
        return status;
    }

    printf("TEEEEEEEEEEEEEEEEEEEEST 0x%X", read_regs.First_freq_bits);  // If ok need to make FOR with all freq_bits


    status = true;

    return status;
}

/*******************************************************************************

    @brief      Calculation of frequency in MHz for Si570

    @param      

    @return     returns fully calculated frequency from Si570
*******************************************************************************/
int Freq_calculaton(){
    std::cout << "\n START CALCULATION OF FREQUENCY FROM Si570 \n";
    
    uint8_t reg_freq = read_regs.First_freq_bits + read_regs.Second_freq_bits + read_regs.Third_freq_bits + read_regs.Forth_freq_bits + read_regs.Fifth_freq_bits;  // NEED TO DIVIDE HS_DIV, N1 FROM ACTUAL FREQUENCY

    int result   = 0;

    int HS_DIV   = 0,
        N1       = 0;     // Dividers

    int factory_fout = 156.25;  // In MHz (Info from dts)


    return result;
}