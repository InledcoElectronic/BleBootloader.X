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
#ifndef BOOTLOADER_H
#define	BOOTLOADER_H

    
void    BOOTLOADER_Initialize();
void    Run_Bootloader();
bool    Bootload_Required ();
uint8_t ProcessBootBuffer ();
void 	Check_Device_Reset ();

#define  INPUT_PIN   1
#define  OUTPUT_PIN  0

#define  ANALOG_PIN  1
#define  DIGITAL_PIN 0

#define  PPS_LOCKED   1
#define  PPS_UNLOCKED 0

#define  OPEN_DRAIN_ENABLED     1  // Enable Open Drain operation
#define  OPEN_DRAIN_DISABLE     0   // Regular operation



// Frame Format
//
//  [<COMMAND><DATALEN><ADDRL><ADDRH><ADDRU><...DATA...>]
// These values are negative because the FSR is set to PACKET_DATA to minimize FSR reloads.
typedef union
{
    struct
    {
        uint8_t     command;
        uint16_t    data_length;
        uint8_t     EE_key_1;
        uint8_t     EE_key_2;
        uint8_t    address_L;
        uint8_t    address_H;
        uint8_t    address_U;
        uint8_t    address_unused;
        uint8_t     data[0x40];
    };
    uint8_t  buffer[0x49];
}frame_t;

#endif	/* BOOTLOADER_H */

