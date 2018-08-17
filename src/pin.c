/*
 * File:   pin.c
 * Author: liruya
 *
 * Created on March 10, 2017, 11:44 AM
 */


#include <xc.h>
#include "pin.h"
#include "bootloader.h"

void PIN_MANAGER_Initialize()
{
    /**
    LATx registers
    */   
    LATA = 0x00;    
    LATB = 0x10;    
    LATC = 0x00;    
    
    TRISA = 0x37;
    TRISB = 0xC0;
    TRISC = 0x05;

    /**
    ANSELx registers
    */   
    ANSELC = 0x00;
    ANSELB = 0x00;
    ANSELA = 0x00;

    /**
    WPUx registers
    */ 
    WPUB = 0x00;
    WPUA = 0x04;
    WPUC = 0x00;

    /**
    ODx registers
    */   
    ODCONA = 0x00;
    ODCONB = 0x00;
    ODCONC = 0x00;
    
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS

//    RXPPSbits.RXPPS = IN_PPS_VAUE(RXD_PORT_MASK, RXD_PIN);
//    TXD_PPS = OUT_PPS_VALUE_TX;
    RXPPSbits.RXPPS = 0x12;   //RC2->EUSART:RX;
    RB4PPSbits.RB4PPS = 0x14;   //RB4->EUSART:TX;

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
}
