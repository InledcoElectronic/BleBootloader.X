/*
 * File:   ble.c
 * Author: liruya
 *
 * Created on March 10, 2017, 2:12 PM
 */


#include <stdbool.h>

#include "ble.h"
#include "eusart.h"
#include "bootloader.h"
#include "pin.h"

void BLE_SendCMD ( const unsigned char *pBuf )
{
    unsigned char ack[] = { 'A', 'T', '+', 'O', 'K', '\r', '\n' };
    unsigned char index = 0;
    unsigned char rev = RCREG;
    while ( *pBuf != '\0' )
    {
        EUSART_Write(*pBuf++);
    }

    //等待ble应答 并判断指令是否成功
    while ( index < sizeof (ack ) )
    {
        rev = EUSART_Read();
        if ( rev == ack[index] )
        {
            index++;
        }
        else
        {
            index = 0;
        }
    }
}

void BLE_SendData ( unsigned char* pbuf, unsigned char len )
{
//    BLE_WKP = 0;
//    __delay_us(800);
    while ( len-- )
    {
        EUSART_Write(*pbuf++);
    }
    //wait for the frame transmission completed
    while ( !TRMT );
//    __delay_us(200);
//    BLE_WKP = 1;
}

void BLE_Init ( )
{
    BLE_RST = 0;
    __delay_ms(20);
    BLE_RST = 1; //release from reset
    __delay_ms(240); //delay 200ms after power on

    //设置ble从机模式
    BLE_SendCMD(BLE_CMD_SLAVE);
    __delay_ms(20);
    //设置ble透传模式
    BLE_SendCMD(BLE_CMD_DATA);
    __delay_ms(20);
//    BLE_WKP = 1;
}
