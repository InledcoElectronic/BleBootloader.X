/*
 * File:   pic16f1_uart.c
 * Author: liruya
 *
 * Created on March 10, 2017, 1:49 PM
 */


#include <pic.h>

#include "bootloader.h"
#include "ble.h"
#include "eusart.h"

void Run_Bootloader()
{
    uint8_t index = 0;
    uint8_t msg_length = 4;
    uint8_t ch;
    bool receiving = false;
    uint16_t idle_count = 0;

    while (1)
    {
        CLRWDT();
        
        while (!TXSTAbits.TRMT);    // wait for last byte to shift out 
        Check_Device_Reset ();      // Response has been sent.  Check to see if a reset was requested
        
        //**********************************************************************
        if ( EUSART_DataReady )
        {
            ch = EUSART_Read();
            frame.buffer[index++] = ch;
            if ( index == 2 )
            {
                if ( frame.command == 0x02 )
                {
                     msg_length += frame.data_length;
                }
                 else if ( frame.command == 0x68 )
                {
                     msg_length = 5;
                }
            }
            if ( index >= msg_length )
            {
                msg_length = ProcessBootBuffer(); 
                BLE_SendData(frame.buffer, msg_length);
                index = 0;
                msg_length = 4;
            }
            idle_count = 0;
            receiving = true;
        }
        if ( receiving )
        {
            idle_count++;
            if ( idle_count > 1000 )
            {
                idle_count = 0;
                receiving = false;
                index = 0;
                msg_length = 4;
            }
        }
        else
        {
            index = 0;
            msg_length = 4;
        }      
        //**********************************************************************
    }
}
// *****************************************************************************
