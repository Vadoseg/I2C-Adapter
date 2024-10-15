
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <fcntl.h> 
#include <unistd.h>
#include <sstream>
#include <cstdint>
#include <cmath>

#include "i2c-functions.h"
#include "iic_rw.h"

// Документация к EEPROM (24c08): https://ww1.microchip.com/downloads/en/devicedoc/21081G.pdf
// Докуметация к Si570: https://www.skyworksinc.com/-/media/skyworks/sl/documents/public/data-sheets/si570-71.pdf 

/*****************************************************

    @brief      Checking status == true or false and sending
    a message
    
    @param      status true or false 
    @param      msg is a message when status == false

    @return     None
*****************************************************/
void Stat_check(bool &status, std::string msg){
    
    if (status == false){
        std::cout << msg;
    }

}

/*******************************************************************************

    @brief      Write in EEPROM by using 1 of 2 types of writing

    @param      fd is the EEPROM device descriptor
    @param      addr is the register number to start writing from
    @param      data the data that needs to be written

    @return     true if write in register was successful and false in other case.
*******************************************************************************/
bool Write_EEPROM(int &fd, unsigned char *addr, unsigned char *wr_data, size_t buf_size){    

    bool status = false;

    if (buf_size < 1 && buf_size > 16){       // if size will be more than page, then the start of the page will be overwrited
        
        std::cout << "\n ERROR WRITE DATA != BYTE OR PAGE \n";
        return false;
    }
    

    if (buf_size == 1){  // Choosing mode of writing (Byte or PageWrite)
        
        std::cout << "\n STARTING EEPROM BYTE WRITE \n";
    }
    else{
        
        std::cout << "\n STARTING EEPROM PAGE WRITE \n";

        std::cout << "\n PAGE SIZE: " << buf_size << std::endl;
        std::cout << "\n MAX PAGE SIZE: 16 \n";
    }
    
    printf("\n WRITING IN: 0x%X \n", *addr);    

    status = iic_wr(fd, addr, wr_data, buf_size);

    usleep(4000);    // Needed delay after writing 

    return status;   // return result of writing 
}

/*******************************************************************************

    @brief      Random read data from EEPROM

    @param      fd is the device descriptor
    @param      addr is the register number to be read

    @return     true if RANDOM read from EEPROM was successful and false in other case.

*******************************************************************************/
bool Read_EEPROM(int &fd, unsigned char *addr){
    std::cout << "\n START RANDOM READ FROM EEPROM \n";
    
    printf("\n READING ADDRESS: 0x%X \n", *addr);

    bool status = false;

    unsigned char read_buf[1] = {0x00};

    status = iic_rd(fd, addr, read_buf, 1);
    Stat_check(status, "\n IIC_READ ERROR - READ_EEPROM (RANDOM) \n");

    printf("\n CHECK READ = 0x%X \n", read_buf[0]);    
    
    return status;
}

/*******************************************************************************

    @brief      Read current address or sequential read from EEPROM

    @param      fd is the device descriptor
    @param      buf_size is size of read_buf

    @return     true if current or sequential read from EEPROM was successful and false in other case.

*******************************************************************************/
bool Read_EEPROM(int &fd, size_t buf_size){
    std::cout << "\n START READING FROM EEPROM \n";

    unsigned char read_buf[buf_size] = {0x00};

    if (buf_size == 1){
        std::cout << "\n READING CURRENT ADDRESS \n";   // If writed in addr = 1 then current addr will be 2 (Byte write) Start addr + pagesize = Current Address (Page Write)
    }
    else{
        std::cout << "\n START SEQUENTIAL READ FROM CURRENT ADDRESS\n";
        std::cout << "\n SIZE OF RD_BUF: " << buf_size << std::endl;
    }

    bool status = false;

    status = iic_rd(fd, read_buf, buf_size);
    Stat_check(status, "\n IIC_READ ERROR - READ_EEPROM (CURR OR SEQ) \n");

    for (size_t i = 0; i < buf_size; i++){
        printf("\n CHECK READ %d = 0x%X \n", i, read_buf[i]);    
    }

    return status;
}

/*******************************************************************************

    @brief      Reading register from Si570

    @param      fd is the device descriptor
    @param      addr is the register number to be read

    @return     true if read from Si570 register was successful and false in other case.
*******************************************************************************/
bool Read_Si570_Reg(int &fd, unsigned char *addr){
    std::cout << "\n START READ Si570 REGISTER \n";

    bool status = false;

    unsigned char read_buf [1]  = {0x00};
    
    status = iic_rd(fd, addr, read_buf, sizeof(read_buf));
    Stat_check(status, "\n IIC_READ ERROR - READ_Si570_REG \n");
    
    printf("\n READ HEX DATA FROM %d REG: 0x%X \n", *addr, read_buf[0]);

    return status;
}

/*******************************************************************************

    @brief      Write in Si570 device register

    @param      fd is the device descriptor
    @param      addr is the register number to start writing from
    @param      wr_data the data that needs to be written
    @param      buf_size is size of wr_data

    @return     true if write in register was successful and false in other case.
*******************************************************************************/
bool Write_Si570_Reg(int &fd, unsigned char *addr, unsigned char *wr_data, size_t buf_size){
    std::cout << "\n START WRITING IN Si570 REG \n";
    
    printf("\n START ADDRESS: 0x%X \n", *addr);

    return  iic_wr(fd, addr, wr_data, buf_size);;

}

/*******************************************************************************

    @brief      Write in 7-12 Si570 device registers

    @param      fd is the device descriptor
    @param      wr_data the data that needs to be written

    @return     true if write in register was successful and false in other case.
*******************************************************************************/
bool Write_Si570(int &fd, unsigned char *wr_data, size_t buf_size){
    std::cout << "\n START WRITING IN Si570 \n";

    if (buf_size != 6){
        std::cout << "\n\n ERROR WRITING SI570 - BUF SIZE NEEDED TO BE 6, TO WRITE 7-12 REGS \n\n";
        return false;
    }

    unsigned char addr = 7;     // Start address needs to be 7 (Info from documentation)
    bool status = false;

    printf("\n START ADDRESS: 0x%X \n", addr);

    status = iic_wr(fd, &addr, wr_data, buf_size); 
    
    if (status == false){
        return false;
    }

    return true;

}

/*******************************************************************************

    @brief      Reading from all Si570 registers at once

    @param      fd is the device descriptor

    @return     true if read from Si570 all registers and calculations was successful and false in other case.
*******************************************************************************/
bool Read_Si570(int &fd){
    std::cout << "\n START FULL READ Si570 \n";

    bool status = false;

    unsigned char addr = 0x7;                 // Start address = 0x7. Reading registers from datasheet (from 7 to 12)
    unsigned char read_buf[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    status = iic_rd(fd, &addr, read_buf, sizeof(read_buf));
    if (status == false) {
        return false;
    }

    status = Freq_calculation(read_buf, sizeof(read_buf));

    return status;
}

/*******************************************************************************

    @brief      Calculation of frequency in MHz for Si570

    @param      read_buf is a buffer that stores the value from the Si570 registers 
    @param      buf_size is size of read_buf

    @return     returns true if calculation was successful
*******************************************************************************/
bool Freq_calculation(unsigned char *read_buf, size_t buf_size){
    std::cout << "\n START CALCULATION OF FREQUENCY FROM Si570 \n";

    std::ostringstream ss;

    float result        = 0;

    uint8_t HS_DIV   = (read_buf[0] >> 5 & 0b00000111) + 4;                                     // HS_DIV takes up only 3 bits and we need to add +4 bcs datasheet says this
        
    uint8_t N1       = (read_buf[0] << 3 & 0b1111100) + (read_buf[1] >> 6 & 0b0000011) + 1;     // We need 0b00000011, but we need to add +1 bcs datasheet says that data writes in registers with -1 (Look datasheet for more info)
    
    float   RFREQ    = (read_buf[1] & 0b00111111);


    ss << std::hex << std::uppercase << RFREQ;                              // Add 6 bits cut from register 8

    for (size_t i = 2; i < buf_size; i++){                             // i = 2, bcs first 2 bits were disassembled on N1, HS_DIV & RFREQ
        
        if (read_buf[i] <= 15){
            ss << std::hex << std::uppercase << "0";                        // Need to add 0 for correct calculation, bcs we need 9 hex bits
        }

        ss << std::hex << std::uppercase << static_cast<int>(read_buf[i]);  // Add another registers (static_cast<int> need to change unsigned char to int)
    }


    std::cout << "\n RFREQ IN HEX: " << ss.str() << "\n";
    
    RFREQ = std::stoll(ss.str(), nullptr, 16);              // Change string to int

    RFREQ = RFREQ / pow(2, 28);

    result = (Si570_Startup_Freq * HS_DIV * N1) / RFREQ;    // Formula from datasheet

    std::cout << "\n REAL FREQUENCY: " << result << "\n";

    if (result > 945 || result < 10){
        std::cout << "FREQUENCY ERROR - NUMBER OUT OF RANGE";
        return false;
    }
    
    return true;
}