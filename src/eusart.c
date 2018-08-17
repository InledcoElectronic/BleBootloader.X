/*
 * File:   eusart.c
 * Author: liruya
 *
 * Created on March 10, 2017, 11:17 AM
 */


#include "eusart.h"

void EUSART_Initialize()
{
    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
    BAUD1CON = 0x08;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
    RC1STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave; 
    TX1STA = 0x24;

    // Baud Rate = 9600; SP1BRGL 25; 
    SP1BRGL = 25;

    // Baud Rate = 9600; SP1BRGH 0; 
    SP1BRGH = 0;
    
//    // Baud Rate = 9600; SP1BRGL 25; 
//    SP1BRGL = 0xA0;
//
//    // Baud Rate = 9600; SP1BRGH 0; 
//    SP1BRGH = 0x01;

}

uint8_t EUSART_Read()
{

    while(!PIR1bits.RCIF);

    if(RC1STAbits.OERR)
    {
        // EUSART error - restart
        RC1STAbits.CREN = 0; 
        RC1STAbits.CREN = 1; 
    }

    return RC1REG;
}

void EUSART_Write(unsigned char byte)
{
    while(!PIR1bits.TXIF);
    TX1REG = byte;    // Write the data byte to the USART.
}