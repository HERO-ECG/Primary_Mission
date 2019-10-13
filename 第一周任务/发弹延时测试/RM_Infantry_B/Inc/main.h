/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__


#define PITCHSENSITIVITY 0.004f
#define YAWSENSITIVITY	0.004f

#define INFANTRY3
//#define INFANTRY4
//#define INFANTRY5

#ifdef	INFANTRY3

#define TX2_OFFSET_PITCH	0.2f
#define	TX2_OFFSET_YAW		0.2f
#define TX2_WINDMILL_OFFSET_PITCH	0.2f
#define	TX2_WINDMILL_OFFSET_YAW		-0.7f
#define YAW_OFFSET	60.0f
#define PITCH_OFFSET	273.5f

#endif

#ifdef	INFANTRY4

#define TX2_OFFSET_PITCH	0.1f
#define	TX2_OFFSET_YAW		-0.55f
#define TX2_WINDMILL_OFFSET_PITCH	-0.7f
#define	TX2_WINDMILL_OFFSET_YAW		-0.7f
#define YAW_OFFSET	0.7f
#define PITCH_OFFSET	273.5f

#endif

#ifdef	INFANTRY5

#define TX2_OFFSET_PITCH	0.1f
#define	TX2_OFFSET_YAW		-0.95f
#define TX2_WINDMILL_OFFSET_PITCH	1.4f
#define	TX2_WINDMILL_OFFSET_YAW		-2.05f
#define YAW_OFFSET	60.7f
#define PITCH_OFFSET	273.5f

#endif



/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

#define LED_GREEN_Pin       GPIO_PIN_0
#define LED_GREEN_GPIO_Port GPIOB
#define LED_RED_Pin         GPIO_PIN_1
#define LED_RED_GPIO_Port   GPIOB
#define LASER_Pin         GPIO_PIN_3
#define LASER_GPIO_Port   GPIOC

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

extern unsigned int time_start;
extern unsigned int time_end;
extern unsigned int time_real;
extern void Systick_start(void);
extern void Systick_end(void);


#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
