
#include "main.h"
#include "usart.h"
#include "usbd_cdc_if.h"
#include "usb_uart.h"
#include "uart_dma.h"
#include "esp32.h"


extern USBD_HandleTypeDef hUsbDeviceFS;
usb_buf_t g_usb_buf ;

//////////////////////////////////////////////////////////////////////////////////////////////


void Usb2UartInit( void )
{
	g_usb_buf.rx_buf_full = 0 ;
	g_usb_buf.rx_buf_tail = 0 ;
	g_usb_buf.rx_buf_head = 0 ;
	
	g_usb_buf.tx_buf_full = 0 ;
	g_usb_buf.tx_buf_tail = 0 ;
	g_usb_buf.tx_buf_head = 0 ;
	g_usb_buf.tx_busy = 0 ;

	g_usb_buf.rx_size     = 0 ;
	g_usb_buf.tx_size     = 0 ;
	
	g_usb_buf.target_type = TARGET_UART ;
	g_usb_buf.target_id   = 2 ; // uart2
	
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, (uint8_t *)g_usb_buf.usb_rx_buf[ g_usb_buf.rx_buf_tail ] );
//  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
}

void RcvDataFromUSB( uint8_t* Buf, uint32_t Len )
{
	
	g_usb_buf.rx_buf_size[ g_usb_buf.rx_buf_tail ]  = Len ;
	g_usb_buf.rx_size     += Len ;
	
	if ( g_usb_buf.rx_buf_tail == ( MAX_USB_BUF_NUM - 1 ) )
		g_usb_buf.rx_buf_tail = 0 ;
	else
		g_usb_buf.rx_buf_tail ++ ;
	if ( g_usb_buf.rx_buf_tail == g_usb_buf.rx_buf_head ) 
	{
		g_usb_buf.rx_buf_full = 1 ;
	}
	else
	{
		USBD_CDC_SetRxBuffer(&hUsbDeviceFS, (uint8_t *)g_usb_buf.usb_rx_buf[ g_usb_buf.rx_buf_tail ] );
		USBD_CDC_ReceivePacket(&hUsbDeviceFS);
	}
}

uint32_t USBTxDataDMA(  uint8_t * buf , uint32_t len )
{
	if ( g_usb_buf.tx_buf_full == 1 ) return  0 ;
	
	memcpy( g_usb_buf.usb_tx_buf[ g_usb_buf.tx_buf_tail ] , buf , len );	
	g_usb_buf.tx_buf_size[ g_usb_buf.tx_buf_tail ] =  len ;              
	
	if ( g_usb_buf.tx_buf_tail == ( MAX_USB_BUF_NUM - 1 ) )
		g_usb_buf.tx_buf_tail = 0 ;
	else
		g_usb_buf.tx_buf_tail ++ ;
	
	if ( g_usb_buf.tx_buf_tail == g_usb_buf.tx_buf_head ) 
	{
		g_usb_buf.tx_buf_full = 1 ;
	}
	return 1 ;
}


void SendDataToUSB( uint8_t* Buf, uint32_t Len )
{
  uint8_t result = USBD_OK;
	g_usb_buf.tx_size     += Len ;
	do 
	{
		result = CDC_Transmit_FS( Buf , Len  ) ;
		if ( result == USBD_FAIL ) 
		{
      break;			
		}
		else if( result == USBD_BUSY ) 
		{
      break;			
		}
  } while ( result != USBD_OK ) ;
	HAL_Delay( 10 );
}

void CheckUSBRxData( void )
{	
	if ( g_usb_buf.rx_buf_full == 1 || 
			 g_usb_buf.rx_buf_head != g_usb_buf.rx_buf_tail  )
	{
		if ( USB_RxDataCallback( g_usb_buf.usb_rx_buf[ g_usb_buf.rx_buf_head ] , g_usb_buf.rx_buf_size[ g_usb_buf.rx_buf_head ] ) == 0 ) return ;
		
		if ( g_usb_buf.rx_buf_head == ( MAX_USB_BUF_NUM - 1 ) )
			g_usb_buf.rx_buf_head = 0 ;
		else
			g_usb_buf.rx_buf_head ++ ;
		if ( g_usb_buf.rx_buf_full == 1 )
		{
			USBD_CDC_SetRxBuffer(&hUsbDeviceFS, (uint8_t *)g_usb_buf.usb_rx_buf[ g_usb_buf.rx_buf_tail ] );
			USBD_CDC_ReceivePacket(&hUsbDeviceFS);
		}
		g_usb_buf.rx_buf_full = 0 ;
	}
}

void CheckUSBTxData( void )
{
  uint8_t * tx_buf ;
	int32_t len ;
	
	//if ( g_usb_buf.tx_busy == 1 ) return ;
	if ( g_usb_buf.tx_buf_full == 1 )
	{
		printf("Uart Tx overflow!\n");
	}
	if ( g_usb_buf.tx_buf_full == 1 || 
			g_usb_buf.tx_buf_head != g_usb_buf.tx_buf_tail  )
	{				
		len = g_usb_buf.tx_buf_size[ g_usb_buf.tx_buf_head ] ;
		tx_buf = g_usb_buf.usb_tx_buf[ g_usb_buf.tx_buf_head ] ;
		SendDataToUSB(  tx_buf , len );
		//g_usb_buf.tx_busy = 1 ;			
		if ( g_usb_buf.tx_buf_head == ( MAX_USB_BUF_NUM - 1 ) )
			g_usb_buf.tx_buf_head = 0 ;
		else
			g_usb_buf.tx_buf_head ++ ;
		g_usb_buf.tx_buf_full = 0 ;
	}			

}

