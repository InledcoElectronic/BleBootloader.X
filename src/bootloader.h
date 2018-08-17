/* 
 * File:   bootloader.h
 * Author: liruya
 *
 * Created on March 10, 2017, 1:31 PM
 */

#ifndef BOOTLOADER_H
#define	BOOTLOADER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include "pin.h"
#include <stdint.h>
#include <stdbool.h>
#include "bootloader.h"
#include "eusart.h"
    
#define _XTAL_FREQ  1000000
    
//#define  INPUT_PIN   1
//#define  OUTPUT_PIN  0
//
//#define  ANALOG_PIN  1
//#define  DIGITAL_PIN 0
//
//#define  PPS_LOCKED   1
//#define  PPS_UNLOCKED 0

//#define  OPEN_DRAIN_ENABLED     1  // Enable Open Drain operation
//#define  OPEN_DRAIN_DISABLE     0   // Regular operation

//#define  TRIS_BOOTLOADER_INDICATOR   TRISCbits.TRISC7
//#define  BOOTLOADER_INDICATOR        LATCbits.LATC7
//#define  BOOTLOADER_INDICATOR_PORT   PORTCbits.RC7
//#define  BOOTLOADER_INDICATOR_ANSEL  ANSELCbits.ANSC7
//#define  BL_INDICATOR_ON   	1
//#define  BL_INDICATOR_OFF 	0

/**
 * flash macro declaration
 */    
#define WRITE_FLASH_BLOCKSIZE   32u
#define ERASE_FLASH_BLOCKSIZE   32u
#define END_FLASH               8192u
#define APP_STATUS_ADDR         8191u
//#define USER_ID_ADDRESS         0x8001

// Frame Format
//
//  [<COMMAND><DATALEN><ADDRL><ADDRH><...DATA...>]
// These values are negative because the FSR is set to PACKET_DATA to minimize FSR reloads.
typedef union
{
    struct
    {
        uint8_t command;
        uint8_t data_length;
        uint16_t address;
        uint8_t data[0x40];
    };
    uint8_t  buffer[0x44];
}frame_t;
    
extern frame_t frame;

extern void SYSTEM_Initialize(void);
extern void BOOTLOADER_Initialize();
extern void Run_Bootloader();
extern bool Bootload_Required ();
extern uint8_t ProcessBootBuffer ();
extern void Check_Device_Reset ();

#ifdef	__cplusplus
}
#endif

#endif	/* BOOTLOADER_H */

