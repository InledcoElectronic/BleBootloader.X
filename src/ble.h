/* 
 * File:   ble.h
 * Author: liruya
 *
 * Created on March 10, 2017, 1:54 PM
 */

#ifndef BLE_H
#define	BLE_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C"
{
#endif

#define BLE_CMD_SLAVE	"AT+SET=1\r\n"
#define BLE_CMD_DATA	"AT+MODE=DATA\r\n"
#define BLE_CMD_ACK_OK	"AT+OK\r\n"

    extern void BLE_SendCMD(const unsigned char *pBuf);
    extern void BLE_SendData(unsigned char* pbuf, unsigned char len);
    extern void BLE_Init();

#ifdef	__cplusplus
}
#endif

#endif	/* BLE_H */

