#ifndef __usb_uart_H
#define __usb_uart_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "common.h"
	 

#define USB_RX_BUF_SIZE    64
#define USB_TX_BUF_SIZE    64
#define MAX_USB_BUF_NUM    8	 

typedef struct 	
{
	uint8_t usb_rx_buf[ MAX_USB_BUF_NUM ][ USB_RX_BUF_SIZE ];
	uint32_t  rx_buf_size[ MAX_USB_BUF_NUM ] ;
	__IO uint32_t  rx_buf_head ;
	__IO uint32_t  rx_buf_tail ;
	__IO uint32_t  rx_buf_full ;
	
	uint8_t usb_tx_buf[ MAX_USB_BUF_NUM ][ USB_TX_BUF_SIZE ] ;
	uint32_t  tx_buf_size[ MAX_USB_BUF_NUM ] ;
	__IO uint32_t  tx_buf_head ;
	__IO uint32_t  tx_buf_tail ;
	__IO uint32_t  tx_buf_full ;	
	__IO uint32_t  tx_busy     ;	
	
	uint32_t  rx_size ;
	uint32_t  tx_size ;
	
	uint32_t  target_id ;
	Switch_Target_TypeDef target_type ;
	
}usb_buf_t;
	 
void RcvDataFromUSB( uint8_t* Buf, uint32_t Len );
void SendDataToUSB( uint8_t* Buf, uint32_t Len );

uint32_t USBTxDataDMA(uint8_t* Buf, uint32_t Len );
void CheckUSBRxData( void );
void CheckUSBTxData( void );
void Usb2UartInit( void );

extern usb_buf_t g_usb_buf ;

#ifdef __cplusplus
}
#endif
#endif /*__usb_uart_H */
