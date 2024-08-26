#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "uart_dma.h"
#include "usb_uart.h"
#include "car_drive.h"
// uart7 wifi uart0
// uart4 wifi uart1
// uart3 user uart

//extern DMA_HandleTypeDef hdma_usart1_rx;
//extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;

uart_dma_t g_uart_dma[ MAX_UART_DMA_NUM ] ;

uint8_t g_active_uart_num = 0 ;

const uart_map_t g_uart_port_map[ MAX_UART_PORT_NUM ] = { 
	{1,&huart1,0,0, &Uart_RxDataCallback , 0 , 0 , TARGET_CPU } ,
	{2,&huart2,&hdma_usart2_rx,&hdma_usart2_tx, &Uart_RxDataCallback , 0 , 0 , TARGET_USB} ,
	{3,&huart3,&hdma_usart3_rx,&hdma_usart3_tx, &Uart_RxDataCallback , 0 , 1 , TARGET_CPU} ,
	{0,0,0,0,0,0,0,0} ,
	{0,0,0,0,0,0,0,0} ,
	{0,0,0,0,0,0,0,0} ,
	{0,0,0,0,0,0,0,0} ,
	{0,0,0,0,0,0,0,0} ,
};



uint32_t Uart_RxDataCallback( Switch_Target_TypeDef target_type , uint32_t target_id , uint8_t * buf , uint32_t len )
{
	uint32_t ret = 0 ;
	if ( target_type == TARGET_USB )
	{
		ret = USBTxDataDMA( buf , len );
	}
	else if ( target_type == TARGET_UART )
	{
		ret = UartTxDataDMA( target_id , buf , len );		
	  //UartTxData( target_id , buf , len );		
	}
	else if(target_type == TARGET_CPU)
	{
		ret = UartCtrl_RxDataCallback(buf , len);
	}
	return ret ;
}

void HAL_UART_EndRxTransfer_IT(UART_HandleTypeDef *huart)
{
  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* At end of Rx process, restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
}

void HAL_UART_DMAStopRx(UART_HandleTypeDef *huart)
{
  uint32_t dmarequest = 0x00U;
  /* The Lock is not implemented on this API to allow the user application
     to call the HAL UART API under callbacks HAL_UART_TxCpltCallback() / HAL_UART_RxCpltCallback():
     when calling HAL_DMA_Abort() API the DMA TX/RX Transfer complete interrupt is generated
     and the correspond call back is executed HAL_UART_TxCpltCallback() / HAL_UART_RxCpltCallback()
     */

  /* Stop UART DMA Rx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
  if ((huart->RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
  {
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* Abort the UART DMA Rx channel */
    if (huart->hdmarx != NULL)
    {
      HAL_DMA_Abort(huart->hdmarx);
    }
    HAL_UART_EndRxTransfer_IT(huart);
  }

}


void Uart_DMA_Init( const uart_map_t * uart_map)
{
	
	uint32_t i ;
	 
	i = uart_map->index - 1 ;
	
	if ( i >= MAX_UART_DMA_NUM ) return ;
	
	HAL_UART_Init( uart_map->huart ) ;

	g_uart_dma[ i ].huart       = uart_map->huart ;
	g_uart_dma[ i ].hdma_rx     = uart_map->hdma_rx ;	
	g_uart_dma[ i ].rx_buf_head = 0 ;
	g_uart_dma[ i ].rx_buf_tail = 0 ;
	g_uart_dma[ i ].rx_buf_full = 0 ;
	g_uart_dma[ i ].RxCallback = uart_map->RxCallback ;
	
	g_uart_dma[ i ].hdma_tx     = uart_map->hdma_tx ;	
	g_uart_dma[ i ].tx_buf_head = 0 ;
	g_uart_dma[ i ].tx_buf_tail = 0 ;
	g_uart_dma[ i ].tx_buf_full = 0 ;
	g_uart_dma[ i ].tx_busy = 0 ;
	g_uart_dma[ i ].TxCallback = uart_map->TxCallback ;
	
	g_uart_dma[ i ].rx_size     = 0 ;
	g_uart_dma[ i ].tx_size     = 0 ;
	
	g_uart_dma[ i ].target_id   = uart_map->target_id ;
	g_uart_dma[ i ].target_type = uart_map->target_type ;
	
	__HAL_UART_ENABLE_IT( uart_map->huart , UART_IT_IDLE);    //使能空闲终中断
	if ( uart_map->hdma_rx != NULL )
	{
		HAL_UART_Receive_DMA( uart_map->huart , 
													( uint8_t *)g_uart_dma[ i ].uart_rx_buf[ 0 ] ,
													UART_RX_BUF_SIZE );
	}
	else
	{
		HAL_UART_Receive_IT( uart_map->huart , 
													( uint8_t *)g_uart_dma[ i ].uart_rx_buf[ 0 ] ,
													UART_RX_BUF_SIZE );
	}
}


void Uart_RxIDLE_Handler( int32_t port )
{
	int32_t i , temp , len ;
	
	i = g_uart_port_map[port-1].index ;
	
	if ( i == 0 ) return ;
	i = i - 1 ;
	
	if( __HAL_UART_GET_FLAG( g_uart_dma[ i ].huart , UART_FLAG_IDLE) != RESET )
	{
		__HAL_UART_CLEAR_IDLEFLAG( g_uart_dma[ i ].huart );
		__HAL_UART_CLEAR_PEFLAG( g_uart_dma[ i ].huart );

//		temp = g_uart_dma[ i ].huart->Instance->SR;  
//		temp = g_uart_dma[ i ].huart->Instance->DR;
		if ( g_uart_dma[ i ].hdma_rx != NULL )
		{
			HAL_UART_DMAStopRx( g_uart_dma[ i ].huart );
			temp  = __HAL_DMA_GET_COUNTER( g_uart_dma[ i ].hdma_rx ) ;
			len = UART_RX_BUF_SIZE - temp ;
			if ( len > 0 )
			{
				g_uart_dma[ i ].rx_buf_size[ g_uart_dma[ i ].rx_buf_tail ] =  len;   
				g_uart_dma[ i ].rx_size += len; 
				if ( g_uart_dma[ i ].rx_buf_tail == ( MAX_UART_BUF_NUM - 1 ) )
					g_uart_dma[ i ].rx_buf_tail = 0 ;
				else
					g_uart_dma[ i ].rx_buf_tail ++ ;
			}

			HAL_UART_Receive_DMA( g_uart_dma[ i ].huart , 
														( uint8_t *)g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_tail ] ,
														UART_RX_BUF_SIZE );
		}
		else
		{
      HAL_UART_EndRxTransfer_IT( g_uart_dma[ i ].huart );			
			temp = g_uart_dma[ i ].huart->RxXferCount ;
			len = UART_RX_BUF_SIZE - temp ;
			if ( len > 0 )
			{
				g_uart_dma[ i ].rx_buf_size[ g_uart_dma[ i ].rx_buf_tail ] =  len ;
				if ( g_uart_dma[ i ].rx_buf_tail == ( MAX_UART_BUF_NUM - 1 ) )
					g_uart_dma[ i ].rx_buf_tail = 0 ;
				else
					g_uart_dma[ i ].rx_buf_tail ++ ;
			}

			HAL_UART_Receive_IT( g_uart_dma[ i ].huart , 
														( uint8_t *)g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_tail ] ,
														UART_RX_BUF_SIZE );
			
		}
			
		if ( len >0 && g_uart_dma[ i ].rx_buf_tail == g_uart_dma[ i ].rx_buf_head ) 
		{
			g_uart_dma[ i ].rx_buf_full = 1 ;
		}
	}	
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	
//	if(huart->Instance == USART1) {	
//		i = 0 ;
//	} else if(huart->Instance == USART2) {	
//		i = 1 ;
//	} else if(huart->Instance == USART3) {	
//		i = 2 ;
//	} else if(huart->Instance == UART4) {	
//		i = 3 ;
//	} else if(huart->Instance == UART5) {	
//		i = 4 ;
//	} else  
//	{	
//		return ;
//	}
//  sprintf( ( char *)err , "uart %d : error code:%X\r\n",i+1 ,huart->ErrorCode ); 
//  SysLog( err ) ;			
//  SendUartDataToUART( g_host_monitor_uart_map ,  err );

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  uint32_t port , i  ;

	if(huart->Instance == USART1) {	
		port = 1 ;
	} else if(huart->Instance == USART2) {	
		port = 2 ;
	} else if(huart->Instance == USART3) {	
		port = 3 ;
	} else
	{	
		return ;
	}
	
	i = g_uart_port_map[port-1].index ;
	
	if ( i == 0 ) return ;
	i = i - 1 ;

	g_uart_dma[ i ].rx_buf_size[ g_uart_dma[ i ].rx_buf_tail ] =  UART_RX_BUF_SIZE ;                           
	g_uart_dma[ i ].rx_size += UART_RX_BUF_SIZE; 
	if ( g_uart_dma[ i ].rx_buf_tail == ( MAX_UART_BUF_NUM - 1 ) )
		g_uart_dma[ i ].rx_buf_tail = 0 ;
	else
		g_uart_dma[ i ].rx_buf_tail ++ ;
	
	if ( g_uart_dma[ i ].hdma_rx != NULL )
	{
		HAL_UART_Receive_DMA( g_uart_dma[ i ].huart , 
													( uint8_t *)g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_tail ] ,
													UART_RX_BUF_SIZE );
	}
	else
	{
		HAL_UART_Receive_IT( g_uart_dma[ i ].huart , 
													( uint8_t *)g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_tail ] ,
													UART_RX_BUF_SIZE );
	}

	if ( g_uart_dma[ i ].rx_buf_tail == g_uart_dma[ i ].rx_buf_head ) 
	{
		g_uart_dma[ i ].rx_buf_full = 1 ;
	}
	
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  uint32_t port , i  ;

	if(huart->Instance == USART1) {	
		port = 1 ;
	} else if(huart->Instance == USART2) {	
		port = 2 ;
	} else if(huart->Instance == USART3) {	
		port = 3 ;
	} else 
	{	
		return ;
	}
	
	i = g_uart_port_map[port-1].index ;	
	if ( i == 0 ) return ;
	i = i - 1 ;

	if ( g_uart_dma[ i ].tx_buf_head == ( MAX_UART_BUF_NUM - 1 ) )
		g_uart_dma[ i ].tx_buf_head = 0 ;
	else
		g_uart_dma[ i ].tx_buf_head ++ ;
	g_uart_dma[ i ].tx_buf_full = 0 ;
  g_uart_dma[ i ].tx_busy = 0 ;
}


void UartTxData( uint32_t port , uint8_t * buf , uint32_t len )
{
	uint32_t i ;
	if ( port > MAX_UART_PORT_NUM || port == 0 ) return ;
	
	i = g_uart_port_map[port-1].index ;	
	if ( i == 0 ) return ;
	i = i - 1 ;
	g_uart_dma[ i ].tx_size += len;
	HAL_UART_Transmit(g_uart_dma[ i ].huart , buf , len , HAL_MAX_DELAY );
}


uint32_t UartTxDataDMA( uint32_t port , uint8_t * buf , uint32_t len )
{
	uint32_t i ;
	if ( len > UART_TX_BUF_SIZE ) return 0 ;
	if ( port > MAX_UART_PORT_NUM || port == 0 ) return 0 ;
	
	i = g_uart_port_map[port-1].index ;	
	if ( i == 0 ) return 0 ;
	i = i - 1 ;

	if ( g_uart_dma[ i ].tx_buf_full == 1 ) return 0 ;

	memcpy( g_uart_dma[ i ].uart_tx_buf[ g_uart_dma[ i ].tx_buf_tail ] , buf , len );	
	g_uart_dma[ i ].tx_buf_size[ g_uart_dma[ i ].tx_buf_tail ] =  len ;              
	
	if ( g_uart_dma[ i ].tx_buf_tail == ( MAX_UART_BUF_NUM - 1 ) )
		g_uart_dma[ i ].tx_buf_tail = 0 ;
	else
		g_uart_dma[ i ].tx_buf_tail ++ ;
	
	if ( g_uart_dma[ i ].tx_buf_tail == g_uart_dma[ i ].tx_buf_head ) 
	{
		g_uart_dma[ i ].tx_buf_full = 1 ;
	}
	return 1 ;
}

void StartAllUartDMAReceive( )
{
	uint32_t i ;
	for ( i = 0 ; i < MAX_UART_PORT_NUM ; i++ )
	{
		if ( g_uart_port_map[i].index > 0 		)    		 
		{
			g_active_uart_num ++ ;
			Uart_DMA_Init( g_uart_port_map+i ); 
		}
	}
}

void StopAllUart( void )
{
	uint32_t i , index ;
	for ( i = 0 ; i < MAX_UART_PORT_NUM ; i++ )
	{
		if ( g_uart_port_map[i].index > 0 ) 
		{
			index	= g_uart_port_map[i].index - 1 ;
			if ( g_uart_dma[ index ].hdma_rx != NULL )
			{
	      	HAL_UART_DMAStop( g_uart_dma[ index ].huart );
			}
			else
			{
          HAL_UART_EndRxTransfer_IT( g_uart_dma[ index ].huart );			
			}
    	__HAL_UART_DISABLE_IT( g_uart_dma[ index ].huart , UART_IT_IDLE);    //使能空闲终中断
			HAL_UART_DeInit( g_uart_dma[ index ].huart ) ;
		}
	}
}

void CheckUartRxData( void )
{
  uint8_t * rx_buf ;
	int32_t len ;
	
	int32_t i ;
 	for ( i = 0 ; i < g_active_uart_num ; i++ )
	{
		if (  g_uart_dma[ i ].rx_buf_full == 1 )
		{
			printf("Uart Rx overflow!\n");
		}
		if ( g_uart_dma[ i ].rx_buf_full == 1 || 
				g_uart_dma[ i ].rx_buf_head != g_uart_dma[ i ].rx_buf_tail  )
		{				
			len = g_uart_dma[ i ].rx_buf_size[ g_uart_dma[ i ].rx_buf_head ] ;			
			rx_buf = g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_head ] ;
			if ( g_uart_dma[i].RxCallback( g_uart_dma[ i ].target_type , g_uart_dma[ i ].target_id ,rx_buf , len ) == 0 ) return ;
			if ( g_uart_dma[ i ].rx_buf_head == ( MAX_UART_BUF_NUM - 1 ) )
				g_uart_dma[ i ].rx_buf_head = 0 ;
			else
				g_uart_dma[ i ].rx_buf_head ++ ;
			g_uart_dma[ i ].rx_buf_full = 0 ;
			
		}			
	}
}

void CheckUartTxData( void )
{
  uint8_t * tx_buf ;
	int32_t len ;
	
	int32_t i ;
 	for ( i = 0 ; i < g_active_uart_num ; i++ )
	{
		if ( g_uart_dma[ i ].tx_busy == 1 ) continue ;
		if ( g_uart_dma[ i ].tx_buf_full == 1 )
		{
			printf("Uart Tx overflow!\n");
		}
		if ( g_uart_dma[ i ].tx_buf_full == 1 || 
				g_uart_dma[ i ].tx_buf_head != g_uart_dma[ i ].tx_buf_tail  )
		{				
			len = g_uart_dma[ i ].tx_buf_size[ g_uart_dma[ i ].tx_buf_head ] ;
			tx_buf = g_uart_dma[ i ].uart_tx_buf[ g_uart_dma[ i ].tx_buf_head ] ;
			g_uart_dma[ i ].tx_size += len;
			HAL_UART_Transmit_DMA(g_uart_dma[ i ].huart , tx_buf , len );
			g_uart_dma[ i ].tx_busy = 1 ;			
		}			
	}

}

