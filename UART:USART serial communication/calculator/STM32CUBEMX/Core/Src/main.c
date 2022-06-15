/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>

#define RS 0x20 /* PB5 mask for reg select */
#define RW 0x40 /* PB6 mask for read/write */
#define EN 0x80 /* PB7 mask for enable */

// gpio defines for lcd
#define LCD_0_PIN GPIO_PIN_0
#define LCD_1_PIN GPIO_PIN_1
#define LCD_2_PIN GPIO_PIN_2
#define LCD_3_PIN GPIO_PIN_3
#define LCD_4_PIN GPIO_PIN_4
#define LCD_5_PIN GPIO_PIN_5
#define LCD_6_PIN GPIO_PIN_6
#define LCD_7_PIN GPIO_PIN_7
#define LCD_GPIO_SELECT GPIOA

#define KEYP_GPIO_SELECT GPIOC
#define KEYP_KPA GPIO_PIN_0
#define KEYP_KPB GPIO_PIN_1
#define KEYP_KPC GPIO_PIN_2
#define KEYP_KPD GPIO_PIN_3
#define KEYP_KP1 GPIO_PIN_4
#define KEYP_KP2 GPIO_PIN_5
#define KEYP_KP3 GPIO_PIN_6
#define KEYP_KP4 GPIO_PIN_7

void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
char KEYPAD_check(void);
void checkInput(char charIn);
void print_result(void);
void show_Operation(void);

uint8_t send_buffer[1] = {0};
uint8_t rec_buffer[1] = {0};

int num1 = 0;
int num2 = 0;
int mode = 0; // 0 for getting first number, 1 for getting operatin, 2 for getting second number, 3 for calculate, 4 for reset
char operation_symbol = 0;
int result = 0;
int reset_calc = 0; // reset lcd after one calculation ( when user starts to write new number)

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
    LCD_init();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        char charIN = KEYPAD_check();
        if (charIN != 0x01)
        {
            // LCD_data(charIN);
            send_buffer[0] = charIN;
            HAL_UART_Transmit_IT(&huart1, send_buffer, sizeof(send_buffer));
            HAL_UART_Receive_IT(&huart1, rec_buffer, sizeof(rec_buffer));
        }
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
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

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_RESET);

    /*Configure GPIO pins : PC0 PC1 PC2 PC3 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : PA0 PA1 PA2 PA3
                             PA4 PA5 PA6 PA7 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PC4 PC5 PC6 PC7 */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : PB5 PB6 PB7 */
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void delayMs(int n)
{
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++)
            __NOP();
}

void LCD_command(unsigned char command)
{
    HAL_GPIO_WritePin(GPIOB, RS, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, RW, GPIO_PIN_RESET);
    for (uint16_t GPIO_Pin = GPIO_PIN_0; GPIO_Pin != GPIO_PIN_8; GPIO_Pin = GPIO_Pin << 1)
    {
        HAL_GPIO_WritePin(LCD_GPIO_SELECT, GPIO_Pin, command % 2);
        command = command >> 1;
    }
    HAL_GPIO_WritePin(GPIOB, EN, GPIO_PIN_SET); /* pulse E high */ /* make a pulse E */
    delayMs(0);
    HAL_GPIO_WritePin(GPIOB, EN, GPIO_PIN_RESET); /* clear E */
    if (command < 4)
        delayMs(2); /* first two commands need more time */
    else
        delayMs(1);
}

void LCD_data(char data)
{
    HAL_GPIO_WritePin(GPIOB, RS, GPIO_PIN_SET);   /* RS = 1 */
    HAL_GPIO_WritePin(GPIOB, RW, GPIO_PIN_RESET); /* R/W = 0 */
    for (uint16_t GPIO_Pin = LCD_0_PIN; GPIO_Pin <= LCD_7_PIN; GPIO_Pin = GPIO_Pin << 1)
    {
        HAL_GPIO_WritePin(LCD_GPIO_SELECT, GPIO_Pin, data % 2);
        data = data >> 1;
    }
    HAL_GPIO_WritePin(GPIOB, EN, GPIO_PIN_SET); /* pulse E high */
    delayMs(0);
    HAL_GPIO_WritePin(GPIOB, EN, GPIO_PIN_RESET); /* clear E */

    delayMs(1);
}

void LCD_init(void)
{ /* initialize port then initialize LCD and making it ready for commanding */
    delayMs(30);
    LCD_command(0x30);
    delayMs(10);
    LCD_command(0x30);
    delayMs(1);
    LCD_command(0x30);
    LCD_command(0x38); /* set 8-bit data and two line of 5x7 font */
    LCD_command(0x06); /*  cursor after each character */
    LCD_command(0x01); /* clear screen and reset cursor */
    LCD_command(0x0F); /* display turn on*/
}

char KEYPAD_check(void)
{
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP3, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP4, GPIO_PIN_SET);

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPA)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPA)))
            ; // wait till the button is pressed
        return '7';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPB)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPB)))
            ; // wait till the button is pressed
        return '4';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPC)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPC)))
            ; // wait till the button is pressed
        return '1';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPD)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPD)))
            ; // wait till the button is pressed
        return 'c';
    }

    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP3, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP4, GPIO_PIN_SET);

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPA)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPA)))
            ; // wait till the button is pressed
        return '8';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPB)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPB)))
            ; // wait till the button is pressed
        return '5';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPC)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPC)))
            ; // wait till the button is pressed
        return '2';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPD)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPD)))
            ; // wait till the button is pressed
        return '0';
    }

    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP4, GPIO_PIN_SET);

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPA)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPA)))
            ; // wait till the button is pressed
        return '9';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPB)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPB)))
            ; // wait till the button is pressed
        return '6';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPC)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPC)))
            ; // wait till the button is pressed
        return '3';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPD)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPD)))
            ; // wait till the button is pressed
        return 'e';
    }

    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP3, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEYP_GPIO_SELECT, KEYP_KP4, GPIO_PIN_RESET);

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPA)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPA)))
            ; // wait till the button is pressed
        return 'd';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPB)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPB)))
            ; // wait till the button is pressed
        return 'x';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPC)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPC)))
            ; // wait till the button is pressed
        return 'm';
    }

    if (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPD)))
    {
        while (!(HAL_GPIO_ReadPin(KEYP_GPIO_SELECT, KEYP_KPD)))
            ; // wait till the button is pressed
        return 's';
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    HAL_UART_Receive_IT(&huart1, rec_buffer, sizeof(rec_buffer));
    // LCD_data(rec_buffer[0]);
    if (mode == 0)
    {
        if (reset_calc == 0)
        {
            LCD_command(1);
            reset_calc++;
        }
        if (0 <= rec_buffer[0] - 48 && rec_buffer[0] - 48 <= 9)
        {
            LCD_data(rec_buffer[0]);
            num1 *= 10;
            num1 += (rec_buffer[0] - 48);
        }
        else if (rec_buffer[0] == 'c')
        {
            mode = 4;
        }
        else
        {
            mode++;
            operation_symbol = rec_buffer[0];
                }
    }

    if (mode == 1)
    {

        if (0 <= rec_buffer[0] - 48 && rec_buffer[0] - 48 <= 9)
        {

            mode++;
        }
        else if (rec_buffer[0] == 'c')
        {
            mode = 4;
        }
        else
        {
            operation_symbol = rec_buffer[0];
            show_Operation();
        }
    }

    if (mode == 2)
    {
        if (0 <= rec_buffer[0] - 48 && rec_buffer[0] - 48 <= 9)
        {
            num2 *= 10;
            num2 += (rec_buffer[0] - 48);
            LCD_data(rec_buffer[0]);
        }
        else if (rec_buffer[0] == 'c')
        {
            mode = 4;
        }
        else if (rec_buffer[0] == 'e')
        {
            mode++;
            LCD_command(1);
            LCD_data('=');
        }
    }

    if (mode == 3)
    {
        switch (operation_symbol)
        {
        case 'd':
            result = num1 / num2;
            break;
        case 'x':
            result = num1 * num2;
            break;
        case 'm':
            result = num1 - num2;
            break;
        case 's':
            result = num1 + num2;
            break;
        default:
            LCD_data('?');
        }
        print_result();
        mode++;
    }
    if (mode == 4)
    {
        num1 = 0;
        num2 = 0;
        result = 0;
        mode = 0;
        operation_symbol = 0;
        reset_calc = 0;
    }
}

void print_result(void)
{
    char resVal[100];
    sprintf(resVal, "%d", result);
    for (int i = 0; i < 101; i++)
    {
        if (resVal[i] == '\0')
            break;
        LCD_data(resVal[i]);
    }
}

void show_Operation(void)
{
    switch (operation_symbol)
    {
    case 'd':
        LCD_data('/');
        break;
    case 'x':
        LCD_data('*');
        break;
    case 'm':
        LCD_data('-');
        break;
    case 's':
        LCD_data('+');
        break;

    default:
        LCD_data('!');
        break;
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
