/* 
 * File:   eusart.h
 * Author: liruya
 *
 * Created on 2017?¨º3??10??, ????10:01
 */

#ifndef EUSART_H
#define	EUSART_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define EUSART_DataReady  (PIR1bits.RCIF)
    
extern void EUSART_Initialize();
extern unsigned char EUSART_Read();
extern void EUSART_Write(unsigned char byte);


#ifdef	__cplusplus
}
#endif

#endif	/* EUSART_H */

