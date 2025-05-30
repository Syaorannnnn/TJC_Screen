/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define       RINGBUFFER_LEN	  (500)
#define       usize             getRingBufferLength()
#define       udelete(x)        deleteRingBuffer(x)
#define       u(x)              read1ByteFromRingBuffer(x)

#define       LCD_UART          huart1
#define       LCD_UART_INS      USART1

extern uint8_t RxBuffer[1];
extern uint32_t msTicks;
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
typedef struct
{
    uint16_t Head;
    uint16_t Tail;
    uint16_t Length;
    uint8_t  Ring_data[RINGBUFFER_LEN];
}RingBuf;

/*********************发送功能****************************/

void SendChar(UART_HandleTypeDef *huart,char ch);
void SendString(UART_HandleTypeDef *huart,char *str);

/*********************发送功能****************************/

void initRingBuffer();
void write1ByteToRingBuffer(uint8_t data);
uint8_t read1ByteFromRingBuffer(uint16_t position);
void deleteRingBuffer(uint16_t size);
uint16_t getRingBufferLength();
bool isRingBufferOverflow();
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

