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

void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
char KEYPAD_check(void);
void checkInput(char charIn);
void checkStudentID(void);
void SEG_show(int inputNum);
void showName(void);
void showTime(void);
void gameStart(void);
int LED_random(void);
void LED_reset(void);
void LOOSER(void);
void WINNER(void);
void LED_FINAL_SHOW(int myState);

int studentID = 0;
int timeStart = 0;
int state = 0;
int nameID = 0;
int name_upd = 0;
int reset_pushed = 0;
uint32_t firstReset = 0;
uint32_t secondReset = 0;
uint32_t difReset = 0;
int resetCount_init = 0;
int myTime = 0;
int endGame = 0;
int level = 3000;
int LED_status = 0;
int fault_num = 0;

#define RS 0x20 /* PB5 mask for reg select */
#define RW 0x40 /* PB6 mask for read/write */
#define EN 0x80 /* PB7 mask for enable */
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
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);
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
    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_TIM4_Init();
    /* USER CODE BEGIN 2 */
    LCD_init();

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        if (state == 2)
        {
            delayMs(level);
            if (LED_status > 0)
            {
                fault_num++;
                SEG_show(fault_num);
            }
            if (fault_num >= 3)
            {
                state = 404;
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
                LOOSER();
            }

            LED_reset();
            switch (LED_random())
            {
            case 0:
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
                LED_status = 1;
                break;
            case 1:
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
                LED_status = 2;
                break;
            case 2:
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
                LED_status = 3;
                break;
            case 3:
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
                LED_status = 4;
                break;
            }
        }

        if (endGame != 0)
        {
            state = 433;
            endGame = 0;
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
            WINNER();
        }

        if (state == 0)
        {
            char charIN = KEYPAD_check();
            if (charIN != 0x01)
                checkInput(charIN);
            checkStudentID();
        }
        if (state == 1)
        {
            if (name_upd == 1)
            {
                LCD_command(1);
                showName();
                name_upd = 0;
            }
        }
        if (state == 100)
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
            LED_reset();
            state = 0;
            studentID = 0;
            nameID = 0;
            name_upd = 0;
            reset_pushed = 0;
            firstReset = 0;
            secondReset = 0;
            difReset = 0;
            resetCount_init = 0;
            myTime = 59;
            endGame = 0;
            level = 3000;
            LED_status = 0;
            fault_num = 0;
            timeStart = 0;
            SEG_show(fault_num);
            HAL_TIM_Base_Stop_IT(&htim2);
            HAL_TIM_Base_Stop_IT(&htim4);

            LCD_command(1);
        }
        if (reset_pushed == 2)
        {
            gameStart();
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
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 15999;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM2_Init 2 */

    /* USER CODE END TIM2_Init 2 */
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

    /* USER CODE BEGIN TIM3_Init 0 */

    /* USER CODE END TIM3_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_IC_InitTypeDef sConfigIC = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* USER CODE BEGIN TIM3_Init 1 */

    /* USER CODE END TIM3_Init 1 */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 15999;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 65535;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM3_Init 2 */

    /* USER CODE END TIM3_Init 2 */
    HAL_TIM_MspPostInit(&htim3);
}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void)
{

    /* USER CODE BEGIN TIM4_Init 0 */

    /* USER CODE END TIM4_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM4_Init 1 */

    /* USER CODE END TIM4_Init 1 */
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 15999;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 999;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM4_Init 2 */

    /* USER CODE END TIM4_Init 2 */
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
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_12, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);

    /*Configure GPIO pins : PC13 PC14 PC0 PC1
                             PC2 PC3 PC4 PC5
                             PC6 PC7 PC12 */
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pin : PC15 */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : PA0 PA1 PA2 PA3
                             PA4 PA5 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PB10 PB8 PB9 */
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : PC8 PC9 PC10 PC11 */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pin : PA8 */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PA9 PA10 PA11 PA12 */
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PB4 PB5 PB6 PB7 */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* USER CODE BEGIN 4 */
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

void LCD_command(unsigned char command)
{
    HAL_GPIO_WritePin(GPIOB, RS, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, RW, GPIO_PIN_RESET);
    for (uint16_t GPIO_Pin = GPIO_PIN_0; GPIO_Pin != GPIO_PIN_8; GPIO_Pin = GPIO_Pin << 1)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_Pin, command % 2);
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
    for (uint16_t GPIO_Pin = GPIO_PIN_0; GPIO_Pin != GPIO_PIN_8; GPIO_Pin = GPIO_Pin << 1)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_Pin, data % 2);
        data = data >> 1;
    }
    HAL_GPIO_WritePin(GPIOB, EN, GPIO_PIN_SET); /* pulse E high */
    delayMs(0);
    HAL_GPIO_WritePin(GPIOB, EN, GPIO_PIN_RESET); /* clear E */

    delayMs(1);
}

void delayMs(int n)
{
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++)
            __NOP();
}

char KEYPAD_check(void)
{

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8))) // if the Col 1 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)))
            ; // wait till the button is pressed
        return '1';
    }

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9))) // if the Col 2 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9)))
            ; // wait till the button is pressed
        return '4';
    }

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10))) // if the Col 3 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10)))
            ; // wait till the button is pressed
        return '7';
    }

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11))) // if the Col 4 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11)))
            ; // wait till the button is pressed
        return '*';
    }

    //////////////

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8))) // if the Col 1 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)))
            ; // wait till the button is pressed
        return '2';
    }

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9))) // if the Col 2 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9)))
            ; // wait till the button is pressed
        return '5';
    }

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10))) // if the Col 3 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10)))
            ; // wait till the button is pressed
        return '8';
    }

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11))) // if the Col 4 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11)))
            ; // wait till the button is pressed
        return '0';
    }

    /////////////

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8))) // if the Col 1 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)))
            ; // wait till the button is pressed
        return '3';
    }

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9))) // if the Col 2 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9)))
            ; // wait till the button is pressed
        return '6';
    }

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10))) // if the Col 3 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10)))
            ; // wait till the button is pressed
        return '9';
    }

    if (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11))) // if the Col 4 is low
    {
        while (!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11)))
            ; // wait till the button is pressed
        return '#';
    }
}

void checkInput(char charIn)
{
    if ('0' <= charIn <= '9')
    {
        studentID *= 10;
        studentID += (charIn - 48);
        LCD_data(charIn);
    }
    if (charIn == '*')
    {
        state = 100;
        studentID = 0;
        reset_pushed = 0;
    }
}

void checkStudentID(void)
{
    if (studentID == 97243076 || studentID == 97243091)
    {
        state++;
        name_upd++;
        HAL_Delay(500);
        HAL_TIM_Base_Start_IT(&htim2);
    }
    if (timeStart == 0)
    {
        // LCD_data('!');
        HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
        timeStart = 1;
    }
}

void SEG_show(int inputNum)
{
    switch (inputNum)
    {
    case 0:
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    };
    break;
    case 1:
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    };
    break;
    case 2:
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    };
    break;
    case 3:
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    };
    break;
    }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        name_upd = 1;
        if (nameID == 0)
            nameID++;
        else
            nameID--;
    }
    if (htim->Instance == TIM4)
    {

        if (myTime >= 0)
        {
            showTime();
            myTime--;
        }
        else
            endGame = 1;

        switch (myTime)
        {
        case 51:
            level = 2000;
            break;
        case 45:
            level = 1000;
            break;
        case 35:
            level = 900;
            break;
        case 25:
            level = 800;
            break;
        case 15:
            level = 700;
            break;
        case 5:
            level = 600;
            break;

        default:
            break;
        }
    }
}

void showName(void)
{
    if (nameID == 1)
    {
        LCD_data('M');
        LCD_data('R');
        LCD_data('E');
        LCD_data('Z');
        LCD_data('A');
        LCD_data('.');
        LCD_data('A');
        LCD_data('M');
        LCD_data('I');
        LCD_data('N');
        LCD_data('I');
    }
    else
    {
        LCD_data('A');
        LCD_data('R');
        LCD_data('.');
        LCD_data('A');
        LCD_data('M');
        LCD_data('I');
        LCD_data('N');
        LCD_data('I');
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    // if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        if (resetCount_init == 0)
        {
            // LCD_command(1);
            // LCD_data('1');
            firstReset = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);
            resetCount_init++;
            __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else if (resetCount_init == 1)
        {
            // LCD_command(1);
            // LCD_data('2');
            secondReset = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);
            __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
            if (firstReset > secondReset)
            {
                difReset = secondReset - secondReset;
            }
            else
            {
                difReset = (0xffffffff - firstReset) + secondReset;
            }
            float secondDif = difReset / 1000;
            if (secondDif >= 2)
            {
                if (secondDif >= 3)
                {
                    state = 100;
                }
                reset_pushed++;
            }
            if (reset_pushed == 2)
            {
                HAL_TIM_Base_Stop_IT(&htim2);
                HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
            }
            firstReset = 0;
            resetCount_init = 0;
        }
    }
}

void showTime(void)
{
    LCD_command(1);
    char resVal[100];
    sprintf(resVal, "%d", myTime);
    for (int i = 0; i < 101; i++)
    {
        if (resVal[i] == '\0')
            break;
        LCD_data(resVal[i]);
    }
}

void gameStart(void)
{
    state = 2;
    myTime = 59;
    reset_pushed = 0;
    LCD_command(1);
    LCD_data('6');
    LCD_data('0');
    HAL_TIM_Base_Start_IT(&htim4);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (state == 2)
    {
        LED_reset();
        if (GPIO_Pin == GPIO_PIN_9)
        {
            if (LED_status != 1)
                fault_num++;
            SEG_show(fault_num);
        }
        if (GPIO_Pin == GPIO_PIN_10)
        {
            if (LED_status != 2)
                fault_num++;
            SEG_show(fault_num);
        }
        if (GPIO_Pin == GPIO_PIN_11)
        {
            if (LED_status != 3)
                fault_num++;
            SEG_show(fault_num);
        }
        if (GPIO_Pin == GPIO_PIN_12)
        {
            if (LED_status != 4)
                fault_num++;
            SEG_show(fault_num);
        }
        if (fault_num >= 3)
        {
            state = 404;
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
            LOOSER();
        }
        LED_status = 0;
    }
}
void LOOSER(void)
{
    HAL_TIM_Base_Stop_IT(&htim2);
    HAL_TIM_Base_Stop_IT(&htim4);
    // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    while (state == 404)
    {
        LCD_command(1);
        LCD_data('L');
        LCD_data('O');
        LCD_data('O');
        LCD_data('S');
        LCD_data('E');
        LCD_data('R');
        LED_FINAL_SHOW(0);
        LCD_command(1);
        LED_FINAL_SHOW(0);
    }
}

void WINNER(void)
{
    HAL_TIM_Base_Stop_IT(&htim2);
    HAL_TIM_Base_Stop_IT(&htim4);
    while (state == 433)
    {
        LCD_command(1);
        LCD_data('W');
        LCD_data('I');
        LCD_data('N');
        LCD_data('N');
        LCD_data('E');
        LCD_data('R');
        LED_FINAL_SHOW(1);
        LCD_command(1);
        LED_FINAL_SHOW(1);
    }
}

void LED_FINAL_SHOW(int myState)
{

    if (myState == 0)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);

        /*
        ////////////////// PWM CODE ///////////////////
        *remeber to delete final 500ms delay and writePin at the end of the function
        in order to run on pwm, in addition add WIN port to MUX in proteus.

        for (int i = 10000; i >= 0; i -= 20)
        {
            TIM3->CCR2 = i;
            delayMs(1);
        }
        for (int i = 0; i <= 10000; i += 20)
        {
            TIM3->CCR2 = i;
            delayMs(1);
        }*/
        //////////////////////////////////////////////
        delayMs(500);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
        /*
        ////////////////// PWM CODE ///////////////////
                *remeber to delete final 500ms delay and writePin at the end of the function
                in order to run on pwm

                for (int i = 10000; i >= 0; i -= 20)
                {
                    TIM3->CCR2 = i;
                    delayMs(1);
                }
                for (int i = 0; i <= 10000; i += 20)
                {
                    TIM3->CCR2 = i;
                    delayMs(1);
                }*/
        //////////////////////////////////////////////
        delayMs(500);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
    }
    delayMs(500);
}
void LED_reset(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
}

int LED_random(void)
{
    srand(__HAL_TIM_GetCounter(&htim3));
    return rand() % 4;
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
