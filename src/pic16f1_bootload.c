//******************************************************************************
//        Software License Agreement
//
// ?2016 Microchip Technology Inc. and its subsidiaries. You may use this
// software and any derivatives exclusively with Microchip products.
//
// THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
// EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
// WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
// PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION WITH ANY
// OTHER PRODUCTS, OR USE IN ANY APPLICATION.
//
// IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
// INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
// WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
// BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
// FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
// ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
// THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
//
// MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS.
//******************************************************************************
//
//
//
// Memory Map
//   -----------------
//   |    0x0000     |   Reset vector
//   |               |
//   |    0x0004     |   Interrupt vector
//   |               |
//   |               |
//   |  Boot Block   |   (this program)
//   |               |
//   |    0x0400     |   Re-mapped Reset Vector
//   |    0x0404     |   Re-mapped High Priority Interrupt Vector
//   |               |
//   |       |       |
//   |               |
//   |  Code Space   |   User program space
//   |               |
//   |       |       |
//   |               |
//   |    0x3FFF     |
//   -----------------
//
//
//
// Definitions:
//
//   STX     -   Start of packet indicator
//   DATA    -   General data up to 255 bytes
//   COMMAND -   Base command
//   DLEN    -   Length of data associated to the command
//   ADDR    -   Address up to 24 bits
//   DATA    -   Data (if any)
//
//
// Commands:
//
//   RD_VER      0x00    Read Version Information
//   RD_MEM      0x01    Read Program Memory
//   WR_MEM      0x02    Write Program Memory
//   ER_MEM      0x03    Erase Program Memory (NOT supported by PIC16)
//   RD_EE       0x04    Read EEDATA Memory
//   WR_EE       0x05    Write EEDATA Memory
//   RD_CONFIG   0x06    Read Config Memory (NOT supported by PIC16)
//   WT_CONFIG   0x07    Write Config Memory (NOT supported by PIC16)
//   CHECKSUM    0x08    Calculate 16 bit checksum of specified region of memory
//   RESET       0x09    Reset Device and run application
//
// *****************************************************************************

#define  READ_VERSION   0x00
//#define  READ_FLASH     0x01
#define  WRITE_FLASH    0x02
#define  ERASE_FLASH    0x03
#define  CALC_CHECKSUM  0x09
#define  RESET_DEVICE   0x0A
#define  OTA_STATUS     0x68
//#define  READ_EE_DATA   0x04
//#define  WRITE_EE_DATA  0x05
//#define  READ_CONFIG    0x06
//#define  WRITE_CONFIG   0x07

// *****************************************************************************
#include "xc.h"       // Standard include
#include <stdint.h>
#include <stdbool.h>
#include "bootloader.h"
#include "ble.h"

// Register: NVMADR
//extern volatile uint16_t          NVMADR               @ 0x81A;
// Register: NVMDAT
//extern volatile uint16_t          NVMDAT               @ 0x81C;

//this should be in the device .h file.  
//extern volatile  uint16_t NVMDATA  @0x81C;

// *****************************************************************************
uint8_t  Get_Version_Data();
//uint8_t  Read_Flash();
uint8_t  Write_Flash();
uint8_t  Erase_Flash();
//uint8_t  Read_EE_Data();
//uint8_t  Write_EE_Data();
//uint8_t  Read_Config();
//uint8_t  Write_Config();
uint8_t  Calc_Checksum();
void     StartWrite();
void     BOOTLOADER_Initialize();
void     Run_Bootloader();
bool     Bootload_Required();

// *****************************************************************************
#define	MINOR_VERSION   0x00       // Version
#define	MAJOR_VERSION   0x01
#define APPLICATION_VALID  0x55

#define ERROR_ADDRESS_OUT_OF_RANGE   0xFE
#define ERROR_INVALID_COMMAND        0xFF
#define COMMAND_SUCCESS              0x01

// To be device independent, these are set by mcc in memory.h
#define  LAST_WORD_MASK          (WRITE_FLASH_BLOCKSIZE - 1)
#define  NEW_RESET_VECTOR        0x400
#define  NEW_INTERRUPT_VECTOR    0x404

#define APP_START_H     0x04
#define APP_START_L     0x00
#define APP_END_H       0x1F
#define APP_END_L       0xFF

#define _str(x)  #x
#define str(x)  _str(x)

volatile bool reset_pending = false;

// Force variables into Unbanked for 1-cycle accessibility 
uint8_t EE_Key_1    @ 0x70 = 0;
uint8_t EE_Key_2    @ 0x71 = 0;

frame_t  frame;

// *****************************************************************************
// The bootloader code does not use any interrupts.
// However, the application code may use interrupts.
// The interrupt vector on a PIC16F is located at
// address 0x0004. 
// The following function will be located
// at the interrupt vector and will contain a jump to
// the new application interrupt vector
void interrupt service_isr()
{
    asm ("pagesel  " str (NEW_INTERRUPT_VECTOR));
    asm ("goto   " str (NEW_INTERRUPT_VECTOR));
}

void BOOTLOADER_Initialize ()
{	
    if (Bootload_Required () == true)
    {
        SYSTEM_Initialize();
        BLE_Init();
//        BOOTLOADER_INDICATOR = BL_INDICATOR_ON;
        Run_Bootloader ();     // generic comms layer
//        BOOTLOADER_INDICATOR = BL_INDICATOR_OFF;
    }
    STKPTR = 0x1F;    
    asm ("pagesel " str(NEW_RESET_VECTOR));
    asm ("goto  "  str(NEW_RESET_VECTOR));
}

/**
 * read the last location in memory to see if the byte is 0x55
 * if yes,it runs application,otherwise it runs bootloader
 * @return 
 */
bool Bootload_Required ()
{
    NVMADR = END_FLASH - 1;
    NVMCON1 = 0x80;
    NVMCON1bits.RD = 1;
    NOP();
    NOP();
    if (NVMDATL != APPLICATION_VALID)
    {
        return (true);
    }

    return (false);
}

uint8_t  ProcessBootBuffer()
{
    uint8_t   len;
    
// Test the command field and sub-command.
    switch (frame.command)
    {
        case    READ_VERSION:
            len = Get_Version_Data();
            break;
//        case    READ_FLASH:
//            len = Read_Flash();
//            break;
        case    WRITE_FLASH:
            len = Write_Flash();
            break;
        case    ERASE_FLASH:
            len = Erase_Flash();
            break;
//        case    READ_CONFIG:
//            len = Read_Config();
//            break;
//        case    WRITE_CONFIG:
//            len = Write_Config();
//            break;
        case    CALC_CHECKSUM:
            len = Calc_Checksum();
            break;
        case    RESET_DEVICE:
            reset_pending = true;
            frame.data[0] = COMMAND_SUCCESS;
            frame.data_length = 0x01;
            len = 5;
            break;
        case    OTA_STATUS:
            frame.data[0] = COMMAND_SUCCESS;
            frame.data_length = 0x01;
            len = 5;
            break;
        default:
            frame.data[0] = ERROR_INVALID_COMMAND;
            frame.data_length = 0x01;
            len = 5;
            break;
    }
    return len;
}

// **************************************************************************************
//        Cmd     Length   ---   Address---------------
// In:   [<0x00> <0x00> <0x00><0x00>]
// OUT:  [<0x00> <0x08> <0x00><0x00> <VERL><VERH> <APP_START> <APP_END> <ER_SIZE><WR_SIZE>]
uint8_t  Get_Version_Data()
{
    frame.data_length = 8;
    frame.data[0] = MINOR_VERSION;
    frame.data[1] = MAJOR_VERSION;
    frame.data[2] = APP_START_L;
    frame.data[3] = APP_START_H;
    frame.data[4] = APP_END_L;       
    frame.data[5] = APP_END_H;
    frame.data[6] = ERASE_FLASH_BLOCKSIZE;       
    frame.data[7] = WRITE_FLASH_BLOCKSIZE;

    return  12;   // total length to send back 4 byte header + 8 byte payload
}

// **************************************************************************************
// Read Flash
// In:	[<0x01><DLEN> <ADDRL><ADDRH>] 
// OUT:	[<0x01><DLEN> <ADDRL><ADDRH> <DATA>... ]
//uint8_t Read_Flash()
//{
//    NVMADR = frame.address;
//    NVMCON1 = 0x80;
//    for (uint8_t i = 0; i < frame.data_length; i += 2)
//    {
//        NVMCON1bits.RD = 1;
//        NOP();
//        NOP();
//        frame.data[i]  = NVMDATL;
//        frame.data[i+1] = NVMDATH;
//        ++ NVMADR;
//    }
//    return (frame.data_length + 4);
//}

/**
 * In:	[<0x02><DLEN> <ADDRL><ADDRH> <DATA>...]
 * OUT:	[<0x02><0x01> <ADDRL><ADDRH> <SUCCESS/ERROR>]
 * @return 
 */
uint8_t Write_Flash()
{  
    if (frame.address < NEW_RESET_VECTOR || frame.address + (frame.data_length>>1) > END_FLASH)
    {
        frame.data_length = 0x01;
        frame.data[0] = ERROR_ADDRESS_OUT_OF_RANGE;
        return 5;
    }
    NVMADR = frame.address;
    NVMCON1 = 0xA4;       // Setup writes
    EE_Key_1 = 0x55;
    EE_Key_2 = 0xAA;
    for (uint8_t i = 0; i < frame.data_length; i += 2)
    {
        if (((NVMADRL & LAST_WORD_MASK) == LAST_WORD_MASK)
          || (i == frame.data_length - 2))
            NVMCON1bits.LWLO = 0;
        NVMDATL = frame.data[i];
        NVMDATH = frame.data[i+1];

        StartWrite();
        ++ NVMADR;
    }
    while(NVMCON1bits.WR);
    NVMCON1bits.WREN = 0;
    EE_Key_1 = 0x00;  // erase EE Keys
    EE_Key_2 = 0x00;
    frame.data_length = 0x01;
    frame.data[0] = COMMAND_SUCCESS;
    return 5;
}

// **************************************************************************************
// Erase Program Memory
// Erases data_length rows from program memory
/**
 * In: [<0x03><DLEN><ADDRL><ADDRH>]
 * Out: [<0x03><0x01><ADDRL><ADDRH><SUCCESS/ERROR>]
 * @return 
 */
uint8_t Erase_Flash ()
{
    if (frame.address < NEW_RESET_VECTOR )
    {
        frame.data_length = 0x01;
        frame.data[0] = ERROR_ADDRESS_OUT_OF_RANGE;
        return 5;
    }
    EE_Key_1 = 0x55;
    EE_Key_2 = 0xAA;
    NVMADR = frame.address;
    for (uint16_t i=0; i < frame.data_length; i++)
    {
        if ((NVMADR & 0x7FFF) >= END_FLASH)
        {
            frame.data_length = 0x01;
            frame.data[0] = ERROR_ADDRESS_OUT_OF_RANGE;
            return 5;
        }
        NVMCON1 = 0x94;       // Setup writes
        StartWrite();
        NVMADR += ERASE_FLASH_BLOCKSIZE;
    }
    while(NVMCON1bits.WR);
    NVMCON1bits.WREN = 0;
    EE_Key_1 = 0x00;  // erase EE Keys
    EE_Key_2 = 0x00;
    frame.data_length = 0x01;
    frame.data[0]  = COMMAND_SUCCESS;
    return 5;
}

// **************************************************************************************
// Read Config Words
// In:	[<0x06><DataLen.> <ADDRL><ADDRH> <checksum>]
// OUT:	[<0x06><DataLen.><0x00> <ADDRL><ADDRH> <config1.><2.> <checksum>]
//uint8_t Read_Config ()
//{
//    if (frame.data_length > 8 || frame.data_length&0x01 != 0x00)
//    {
//        frame.data_length = 0x01;
//        frame.data[0] = ERROR_ADDRESS_OUT_OF_RANGE;
//        return (6);
//    }
//    NVMADRL = frame.address_L;
//    NVMADRH = frame.address_H;
//    NVMCON1 = 0x40;      // can these be combined?
//    for (uint8_t  i= 0; i < frame.data_length; i += 2)
//    {
//        NVMCON1bits.RD = 1;
//        NOP();
//        NOP();
//        frame.data[i]   = NVMDATL;
//        frame.data[i+1] = NVMDATH;
//        ++ NVMADR;
//    }
//    return (5+frame.data_length);           
//}

// **************************************************************************************
// Write Config Words
//uint8_t Write_Config ()
//{    
//    if ((frame.data_length&0x01) != 0x00
//            || frame.address_H != 0x80 || frame.address_L < 0x07 || frame.address_L > 0x0A
//            || frame.address_L+(frame.data_length>>1) > 0x0B )
//    {
//        frame.data_length = 0x01;
//        frame.data[0] = ERROR_ADDRESS_OUT_OF_RANGE;
//        return (6);
//    }
//    NVMADRL = frame.address_L;
//    NVMADRH = frame.address_H;
//    NVMCON1 = 0xC4;       // Setup writes
//    EE_Key_1 = 0x55;
//    EE_Key_2 = 0xAA;
//    for (uint8_t  i = 0; i < frame.data_length; i += 2)
//    {
//        NVMDATL = frame.data[i];
//        NVMDATH = frame.data[i+1];
//
//        StartWrite();
//        ++ NVMADR;
//    }
//    frame.data_length = 0x01;
//    frame.data[0] = COMMAND_SUCCESS;
//    EE_Key_1 = 0x00;  // erase EE Keys
//    EE_Key_2 = 0x00;
//    return (6);
//}

/**
 * In:	[<0x09><0x02> <ADDRL><ADDRH> <ENDL><ENDH>]
 * Out:	[<0x09><0x04> <ADDRL><ADDRH> <ENDL><ENDH> <CRCL><CRCH>]
 * @return 
 */
uint8_t Calc_Checksum()
{
    uint16_t endAddr = (frame.data[1]<<8)|frame.data[0];
    if (frame.address < NEW_RESET_VECTOR || endAddr >= END_FLASH)
    {
        frame.data_length = 0x01;
        frame.data[0] = ERROR_ADDRESS_OUT_OF_RANGE;
        return 5;
    }    
    NVMADR = frame.address;
    NVMCON1 = 0x80;
    uint16_t check_sum = 0;
    for (uint16_t i = frame.address; i < endAddr; i++)
    {
        NVMCON1bits.RD = 1;
        NOP();
        NOP();
        check_sum += NVMDAT;
        ++ NVMADR;
    }
     frame.data[2] = check_sum & 0xFF;
     frame.data[3] = check_sum >> 8;
     return 8;
}

// *****************************************************************************
// Unlock and start the write or erase sequence.

void StartWrite()
{
    CLRWDT();
//    NVMCON2 = EE_Key_1;
//    NVMCON2 = EE_Key_2;
//    NVMCON1bits.WR = 1;       // Start the write
// had to switch to assembly - compiler doesn't comprehend no need for bank switch
    asm ("movf " str(_EE_Key_1) ",w");
    asm ("movwf " str(BANKMASK(NVMCON2)));
    asm ("movf  " str(_EE_Key_2) ",w");
    asm ("movwf " str(BANKMASK(NVMCON2)));
    asm ("bsf  "  str(BANKMASK(NVMCON1)) ",1");       // Start the write

    NOP();
    NOP();
}

// *****************************************************************************
// Check to see if a device reset had been requested.  We can't just reset when
// the reset command is issued.  Instead we have to wait until the acknowledgement
// is finished sending back to the host.  Then we reset the device.
void Check_Device_Reset ()
{
    if (reset_pending == true)
    {
    // This section writes last location in
    // memory to the application valid value.
    // indicating a valid application is loaded.
        NVMADR  = END_FLASH - 1;
        NVMCON1 = 0x84;
        NVMDATL = APPLICATION_VALID;
        NVMCON2 = 0x55;
        NVMCON2 = 0xAA;
        NVMCON1bits.WR = 1;
        NOP();
        NOP();
        while(NVMCON1bits.WR);
        NVMCON1bits.WREN = 0;

//        TRIS_BOOTLOADER_INDICATOR = OUTPUT_PIN;
//        BOOTLOADER_INDICATOR = BL_INDICATOR_OFF;
        //wait for bluetooth module response to app,otherwise reset will interrupt bluetooth sending data
        __delay_ms(96);
        RESET();
    }
}
// *****************************************************************************
// *****************************************************************************
