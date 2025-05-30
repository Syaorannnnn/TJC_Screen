/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
RingBuf ringBuffer;
uint8_t RxBuffer[1];    // 串口接收缓冲区
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(huart->Instance == LCD_UART_INS)	// 判断是由哪个串口触发的中断
	{
		write1ByteToRingBuffer(RxBuffer[0]);
		HAL_UART_Receive_IT(&LCD_UART,RxBuffer,1);		// 重新使能串口接收中断
	}
}

#pragma region Send_Functions

/**
 * @brief 发送单个字符
 * 
 * @param huart 
 * @param ch 
 */
void SendChar(UART_HandleTypeDef *huart,char ch)
{
	uint8_t ch2 = (uint8_t)ch;
  //等待发送完成
	while(__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET);
  //发送单个字符
	HAL_UART_Transmit_IT(&huart, &ch2, 1);
}

/**
 * @brief 发送字符串
 * 
 * @param huart 
 * @param str 
 */
void SendString(UART_HandleTypeDef *huart,char *str)
{
  //当前地址不在结尾且字符串首地址非空
  while(*str != 0 && str != 0)
  {
    SendChar(&huart, *str);
    str++;
  }
}

#pragma endregion

#pragma region RingBuffer
/**
 * @brief 初始化环形缓冲区
 * 
 */
void initRingBuffer()
{
	//初始化相关信息
	ringBuffer.Head = 0;
	ringBuffer.Tail = 0;
	ringBuffer.Length = 0;
}

/**
 * @brief 向环形缓冲区内写入1字节
 * 
 * @param data 
 */
void write1ByteToRingBuffer(uint8_t data)
{
	if(ringBuffer.Length >= RINGBUFFER_LEN) //判断缓冲区是否已满
	{
	return;
	}
	ringBuffer.Ring_data[ringBuffer.Tail]=data;
	ringBuffer.Tail = (ringBuffer.Tail+1)%RINGBUFFER_LEN;//防止越界非法访问
	ringBuffer.Length++;
}

/**
 * @brief 从环形缓冲区内删除size字节的数据
 * 
 * @param size 
 */
void deleteRingBuffer(uint16_t size)
{
	if(size >= ringBuffer.Length)
	{
	    initRingBuffer();
	    return;
	}
	for(int i = 0; i < size; i++)
	{
		ringBuffer.Head = (ringBuffer.Head+1)%RINGBUFFER_LEN;//防止越界非法访问
		ringBuffer.Length--;
		return;
	}

}

/**
 * @brief 从position位置读取1字节数据
 * 
 * @param position 
 * @return uint8_t 
 */
uint8_t read1ByteFromRingBuffer(uint16_t position)
{
	uint16_t realPosition = (ringBuffer.Head + position) % RINGBUFFER_LEN;

	return ringBuffer.Ring_data[realPosition];
}

/**
 * @brief 获取当前环形缓冲区数据长度
 * 
 * @return uint16_t 
 */
uint16_t getRingBufferLength()
{
	return ringBuffer.Length;
}

/**
 * @brief 判断缓冲区是否已满
 * 
 * @return uint8_t 
 */
bool isRingBufferOverflow()
{
	return ringBuffer.Length < RINGBUFFER_LEN;
}

#pragma endregion
/* USER CODE END 1 */
