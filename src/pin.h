/* 
 * File:   pin.h
 * Author: liruya
 *
 * Created on March 10, 2017, 11:43 AM
 */

#ifndef PIN_H
#define	PIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#define PORTA_PPS_MASK      0x00
#define PORTB_PPS_MASK      0x08
#define PORTC_PPS_MASK      0x10
#define OUT_PPS_VALUE_TX    0x14
#define IN_PPS_VAUE(portmask, n)    (portmask+n)
#define TXD_PORT    PORTB
#define TXD_PIN     4
#define RXD_PORT    PORTC
#define RXD_PIN     2
#define RXD_PORT_MASK       PORTC_PPS_MASK
#define BLE_WKP             LATCbits.LATC1
#define BLE_WKP_TRIS        TRISCbits.TRISC1
#define BLE_WKP_ANSEL       ANSELCbits.ANSC1
#define BLE_INT             PORTAbits.RA2
#define BLE_INT_TRIS        TRISAbits.TRISA2
#define BLE_INT_ANSEL       ANSELAbits.ANSA2
#define BLE_RST             LATBbits.LATB5
#define BLE_RST_TRIS        TRISBbits.TRISB5
#define BLE_RST_ANSEL       ANSELBbits.ANSB5
#define BLE_TXD             LATBbits.LATB4
#define BLE_TXD_TRIS        TRISBbits.TRISB4
#define BLE_TXD_ANSEL       ANSELBbits.ANSB4
#define BLE_RXD_TRIS        TRISCbits.TRISC2
#define BLE_RXD_ANSEL       ANSELCbits.ANSC2
#define RXD_PPS             RXPPSbits.RXPPS
#define TXD_PPS             RB4PPSbits.RB4PPS
    
void PIN_MANAGER_Initialize ();


#ifdef	__cplusplus
}
#endif

#endif	/* PIN_H */

