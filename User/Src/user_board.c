#include "user_common.h"

volatile Motor_Pin_TypeDer GD32_Motor_Pin[5][MAX_NUM_AXIS] =
{
  { {GD32_X2_STP_Pin, GD32_X2_STP_GPIO_Port}, {GD32_X_STP_Pin, GD32_X_STP_GPIO_Port}, {GD32_Y_STP_Pin, GD32_Y_STP_GPIO_Port},
    {GD32_Z_STP_Pin, GD32_Z_STP_GPIO_Port}, {GPIO_PIN_11, GPIOC},
    {GD32_E_STP_Pin, GD32_E_STP_GPIO_Port}, {GD32_B_STP_Pin, GD32_B_STP_GPIO_Port}
  },
  { {GD32_X2_DIR_Pin, GD32_X2_DIR_GPIO_Port}, {GD32_X_DIR_Pin, GD32_X_DIR_GPIO_Port}, {GD32_Y_DIR_Pin, GD32_Y_DIR_GPIO_Port},
    {GD32_Z_DIR_Pin, GD32_Z_DIR_GPIO_Port}, {GPIO_PIN_10, GPIOC}, {GD32_E_DIR_Pin, GD32_E_DIR_GPIO_Port}, {GD32_B_DIR_Pin, GD32_B_DIR_GPIO_Port}
  },
  { {GD32_X2_EN_Pin, GD32_X2_EN_GPIO_Port}, {GD32_X_EN_Pin, GD32_X_EN_GPIO_Port},    {GD32_Y_EN_Pin, GD32_Y_EN_GPIO_Port},
    {GD32_Z_EN_Pin, GD32_Z_EN_GPIO_Port}, {GPIO_PIN_2, GPIOD}, {GD32_E_EN_Pin, GD32_E_EN_GPIO_Port}, {GD32_B_EN_Pin, GD32_B_EN_GPIO_Port}
  },
  { {GD32_SIG_X2_Pin, GD32_SIG_X2_GPIO_Port}, {NULL, NULL},  {GD32_SIG_Y_Pin, GD32_SIG_Y_GPIO_Port},
    {GD32_SIG_Z_MIN_Pin, GD32_SIG_Z_MIN_GPIO_Port}, {GD32_SIG_DOOR_Pin, GD32_SIG_DOOR_GPIO_Port}, {NULL, NULL}, {NULL, NULL}
  },
  { {NULL, NULL},  {GD32_SIG_X_Pin, GD32_SIG_X_GPIO_Port}, {NULL, NULL},
    {GD32_SIG_Z_MAX_Pin, GD32_SIG_Z_MAX_GPIO_Port}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}
  },
};

volatile Motor_Pin_TypeDer STM32_Motor_Pin[5][MAX_NUM_AXIS] =
{
  { {STM32_X2_STP_Pin, STM32_X2_STP_GPIO_Port}, {STM32_X_STP_Pin, STM32_X_STP_GPIO_Port}, {STM32_Y_STP_Pin, STM32_Y_STP_GPIO_Port},
    {STM32_Z_STP_Pin, STM32_Z_STP_GPIO_Port}, {GPIO_PIN_11, GPIOC}, {STM32_E_STP_Pin, STM32_E_STP_GPIO_Port}, {STM32_B_STP_Pin, STM32_B_STP_GPIO_Port}
  },
  { {STM32_X2_DIR_Pin, STM32_X2_DIR_GPIO_Port}, {STM32_X_DIR_Pin, STM32_X_DIR_GPIO_Port}, {STM32_Y_DIR_Pin, STM32_Y_DIR_GPIO_Port},
    {STM32_Z_DIR_Pin, STM32_Z_DIR_GPIO_Port}, {GPIO_PIN_10, GPIOC}, {STM32_E_DIR_Pin, STM32_E_DIR_GPIO_Port},  {STM32_B_DIR_Pin, STM32_B_DIR_GPIO_Port}
  },
  { {STM32_X2_EN_Pin, STM32_X2_EN_GPIO_Port}, {STM32_X_EN_Pin, STM32_X_EN_GPIO_Port},    {STM32_Y_EN_Pin, STM32_Y_EN_GPIO_Port},
    {STM32_Z_EN_Pin, STM32_Z_EN_GPIO_Port}, {GPIO_PIN_2, GPIOD}, {STM32_E_EN_Pin, STM32_E_EN_GPIO_Port},    {STM32_B_EN_Pin, STM32_B_EN_GPIO_Port}
  },
  { {STM32_SIG_X2_Pin, STM32_SIG_X2_GPIO_Port}, {NULL, NULL},  {STM32_SIG_Y_Pin, STM32_SIG_Y_GPIO_Port},
    {STM32_SIG_Z_MIN_Pin, STM32_SIG_Z_MIN_GPIO_Port}, {STM32_SIG_DOOR_Pin, STM32_SIG_DOOR_GPIO_Port}, {NULL, NULL},  {NULL, NULL}
  },
  { {NULL, NULL},  {STM32_SIG_X_Pin, STM32_SIG_X_GPIO_Port}, {NULL, NULL},
    {STM32_SIG_Z_MAX_Pin, STM32_SIG_Z_MAX_GPIO_Port}, {NULL, NULL}, {NULL, NULL},  {NULL, NULL}
  },
};

volatile Heat_Pin_TypeDer GD32_Heat_Pin[4] =
{
  {GD32_HEAT_NOZ_E_Pin, GD32_HEAT_NOZ_E_GPIO_Port}, {GD32_HEAT_BED_Pin, GD32_HEAT_BED_GPIO_Port},
  {GD32_HEAT_CAVITY_Pin, GD32_HEAT_CAVITY_GPIO_Port}, {GD32_HEAT_NOZ_B_Pin, GD32_HEAT_NOZ_B_GPIO_Port}
};

volatile Heat_Pin_TypeDer STM32_Heat_Pin[4] =
{
  {STM32_HEAT_NOZ_E_Pin, STM32_HEAT_NOZ_E_GPIO_Port}, {STM32_HEAT_BED_Pin, STM32_HEAT_BED_GPIO_Port},
  {STM32_HEAT_CAVITY_Pin, STM32_HEAT_CAVITY_GPIO_Port}, {STM32_HEAT_NOZ_B_Pin, STM32_HEAT_NOZ_B_GPIO_Port}
};

volatile ADC_Pin_TypeDer GD32_ADC_Pin[4] =
{
  {GD32_TEMP_E_Pin, ADC_CHANNEL_3, GD32_TEMP_E_GPIO_Port}, {GD32_TEMP_BED_Pin, ADC_CHANNEL_6, GD32_TEMP_BED_GPIO_Port},
  {GD32_TEMP_CAVITY_Pin, ADC_CHANNEL_1, GD32_TEMP_CAVITY_GPIO_Port}, {GD32_TEMP_B_Pin, ADC_CHANNEL_0, GD32_TEMP_B_GPIO_Port}
};

volatile ADC_Pin_TypeDer STM32_ADC_Pin[4] =
{
  {STM32_TEMP_E_Pin, ADC_CHANNEL_3, STM32_TEMP_E_GPIO_Port}, {STM32_TEMP_BED_Pin, ADC_CHANNEL_6, STM32_TEMP_BED_GPIO_Port},
  {STM32_TEMP_CAVITY_Pin, ADC_CHANNEL_1, STM32_TEMP_CAVITY_GPIO_Port}, {STM32_TEMP_B_Pin, ADC_CHANNEL_0, STM32_TEMP_B_GPIO_Port}
};


static void board_stm32_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(STM32_HEAT_NOZ_B_GPIO_Port, STM32_HEAT_NOZ_B_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, STM32_LCD_BACKLIGHT_Pin | STM32_B_DIR_Pin | STM32_HEAT_CAVITY_Pin | STM32_X2_STP_Pin
                    | STM32_X2_DIR_Pin | STM32_Y_STP_Pin | STM32_Y_DIR_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, STM32_Y_EN_Pin | STM32_B_STP_Pin | STM32_X2_EN_Pin | STM32_FAN_FILTER_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, STM32_E_STP_Pin | STM32_LIGHT_BAR_Pin | STM32_HEAT_BED_Pin | STM32_Z_STP_Pin
                    | STM32_Z_DIR_Pin | STM32_Z_EN_Pin | STM32_HEAT_NOZ_E_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, STM32_E_EN_Pin | STM32_B_EN_Pin | STM32_TOUCH_MISO_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, STM32_TOUCH_MOSI_Pin | STM32_TOUCH_CS_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(STM32_FAN_EB_MOTOR_GPIO_Port, STM32_FAN_EB_MOTOR_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, STM32_X_STP_Pin | STM32_X_DIR_Pin | STM32_X_EN_Pin | STM32_TOUCH_SCK_Pin
                    | STM32_E_DIR_Pin | STM32_TOUCH_PEN_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin : PE3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  /*Configure GPIO pin : STM32_HEAT_NOZ_B_Pin */
  GPIO_InitStruct.Pin = STM32_HEAT_NOZ_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STM32_HEAT_NOZ_B_GPIO_Port, &GPIO_InitStruct);
  /*Configure GPIO pins : STM32_SIG_X2_Pin STM32_SIG_Y_Pin */
  GPIO_InitStruct.Pin = STM32_SIG_X2_Pin | STM32_SIG_Y_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  /*Configure GPIO pins : STM32_LCD_BACKLIGHT_Pin STM32_B_DIR_Pin STM32_HEAT_CAVITY_Pin STM32_X2_STP_Pin
                           STM32_X2_DIR_Pin STM32_Y_STP_Pin STM32_Y_DIR_Pin */
  GPIO_InitStruct.Pin = STM32_LCD_BACKLIGHT_Pin | STM32_B_DIR_Pin | STM32_HEAT_CAVITY_Pin | STM32_X2_STP_Pin
                        | STM32_X2_DIR_Pin | STM32_Y_STP_Pin | STM32_Y_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  /*Configure GPIO pins : STM32_Y_EN_Pin STM32_B_STP_Pin STM32_X2_EN_Pin STM32_FAN_FILTER_Pin */
  GPIO_InitStruct.Pin = STM32_Y_EN_Pin | STM32_B_STP_Pin | STM32_X2_EN_Pin | STM32_FAN_FILTER_Pin | GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  /*Configure GPIO pins : STM32_E_STP_Pin STM32_LIGHT_BAR_Pin STM32_HEAT_BED_Pin STM32_Z_STP_Pin
                           STM32_Z_DIR_Pin STM32_Z_EN_Pin STM32_HEAT_NOZ_E_Pin */
  GPIO_InitStruct.Pin = STM32_E_STP_Pin | STM32_LIGHT_BAR_Pin | STM32_HEAT_BED_Pin | STM32_Z_STP_Pin
                        | STM32_Z_DIR_Pin | STM32_Z_EN_Pin | STM32_HEAT_NOZ_E_Pin | GPIO_PIN_10 | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  /*Configure GPIO pin : STM32_SIG_DOOR_Pin */
  GPIO_InitStruct.Pin = STM32_SIG_DOOR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(STM32_SIG_DOOR_GPIO_Port, &GPIO_InitStruct);
  /*Configure GPIO pins : STM32_E_EN_Pin STM32_B_EN_Pin STM32_TOUCH_MISO_Pin */
  GPIO_InitStruct.Pin = STM32_E_EN_Pin | STM32_B_EN_Pin | STM32_TOUCH_MISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  /*Configure GPIO pins : STM32_SIG_Z_MAX_Pin STM32_SIG_Z_MIN_Pin */
  GPIO_InitStruct.Pin = STM32_SIG_Z_MAX_Pin | STM32_SIG_Z_MIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  /*Configure GPIO pins : STM32_TOUCH_MOSI_Pin STM32_TOUCH_CS_Pin */
  GPIO_InitStruct.Pin = STM32_TOUCH_MOSI_Pin | STM32_TOUCH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  /*Configure GPIO pin : STM32_SIG_X_Pin */
  GPIO_InitStruct.Pin = STM32_SIG_X_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(STM32_SIG_X_GPIO_Port, &GPIO_InitStruct);
  /*Configure GPIO pin : STM32_FAN_EB_MOTOR_Pin */
  GPIO_InitStruct.Pin = STM32_FAN_EB_MOTOR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STM32_FAN_EB_MOTOR_GPIO_Port, &GPIO_InitStruct);
  /*Configure GPIO pins : STM32_X_STP_Pin STM32_X_DIR_Pin STM32_X_EN_Pin STM32_TOUCH_SCK_Pin
                           STM32_E_DIR_Pin STM32_TOUCH_PEN_Pin */
  GPIO_InitStruct.Pin = STM32_X_STP_Pin | STM32_X_DIR_Pin | STM32_X_EN_Pin | STM32_TOUCH_SCK_Pin
                        | STM32_E_DIR_Pin | STM32_TOUCH_PEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
  /*Configure GPIO pin : STM32_SIG_MAT_E0_Pin */
  GPIO_InitStruct.Pin = STM32_SIG_MAT_E0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(STM32_SIG_MAT_E0_GPIO_Port, &GPIO_InitStruct);
  /*Configure GPIO pin : STM32_SIG_MAT_E1_Pin */
  GPIO_InitStruct.Pin = STM32_SIG_MAT_E1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(STM32_SIG_MAT_E1_GPIO_Port, &GPIO_InitStruct);
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

#if defined(USE_APP2)
static void board_disable_spi5(void)
{
  extern SPI_HandleTypeDef hspi5;
  HAL_SPI_DeInit(&hspi5);
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOF_CLK_ENABLE();
  /**SPI5 GPIO Configuration
  PF7     ------> SPI5_SCK
  PF9     ------> SPI5_MOSI
  PF8     ------> SPI5_MISO
  */
  GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);
}

void board_tim2_init(void)
{
  extern TIM_HandleTypeDef htim2;
  HAL_TIM_PWM_DeInit(&htim2);
  HAL_TIM_Base_DeInit(&htim2);
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 83;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /**TIM2 GPIO Configuration
   PA15     ------> TIM2_CH1
   */
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = STM32_FAN_NOZ_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(STM32_FAN_NOZ_B_GPIO_Port, &GPIO_InitStruct);
}

void board_tim3_init(void)
{
  extern TIM_HandleTypeDef htim3;
  HAL_TIM_PWM_DeInit(&htim3);
  HAL_TIM_Base_DeInit(&htim3);
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 83;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
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

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }

  /**TIM3 GPIO Configuration
  PB0     ------> TIM3_CH3
  */
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = STM32_FAN_NOZ_E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(STM32_FAN_NOZ_E_GPIO_Port, &GPIO_InitStruct);
}
#endif

static void board_deinit_spi_cs_pin(void)
{
  HAL_GPIO_DeInit(GD32_X_CS_GPIO_Port, GD32_X_CS_Pin);
  HAL_GPIO_DeInit(GD32_X2_CS_GPIO_Port, GD32_X2_CS_Pin);
  HAL_GPIO_DeInit(GD32_Y_CS_GPIO_Port, GD32_Y_CS_Pin);
  HAL_GPIO_DeInit(GD32_Z_CS_GPIO_Port, GD32_Z_CS_Pin);
  HAL_GPIO_DeInit(GD32_E_CS_GPIO_Port, GD32_E_CS_Pin);
  HAL_GPIO_DeInit(GD32_B_CS_GPIO_Port, GD32_B_CS_Pin);
}

void user_board_init(void)
{
  if (mcu_id == MCU_STM32F429IGT6)
  {
    //stm32 没有step cs pin
    board_deinit_spi_cs_pin();
#if defined(USE_APP2)
    // 没有spi引脚
    board_disable_spi5();
    board_tim2_init(); // PA15 B喷嘴风扇
    board_tim3_init(); // PB0 E喷嘴风扇
#endif
    board_stm32_gpio_init();
    //关闭加热
    HAL_GPIO_WritePin(STM32_HEAT_BED_GPIO_Port, STM32_HEAT_BED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STM32_HEAT_NOZ_E_GPIO_Port, STM32_HEAT_NOZ_E_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STM32_HEAT_NOZ_B_GPIO_Port, STM32_HEAT_NOZ_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STM32_HEAT_CAVITY_GPIO_Port, STM32_HEAT_CAVITY_Pin, GPIO_PIN_RESET);
    //解锁电机
    HAL_GPIO_WritePin(STM32_X_EN_GPIO_Port, STM32_X_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STM32_Y_EN_GPIO_Port, STM32_Y_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STM32_Z_EN_GPIO_Port, STM32_Z_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STM32_X2_EN_GPIO_Port, STM32_X2_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STM32_E_EN_GPIO_Port, STM32_E_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STM32_B_EN_GPIO_Port, STM32_B_EN_Pin, GPIO_PIN_RESET); // 使能Z电机
  }
  else if (mcu_id == MCU_GD32F450IIH6)
  {
#if defined(USE_APP2)
    // A4982 error fixed
    board_disable_spi5();
#endif
    //关闭spi片选
    USER_GPIO_WRITE(GD32, X_CS, GPIO_PIN_RESET);
    USER_GPIO_WRITE(GD32, X2_CS, GPIO_PIN_RESET);
    USER_GPIO_WRITE(GD32, Y_CS, GPIO_PIN_RESET);
    USER_GPIO_WRITE(GD32, Z_CS, GPIO_PIN_RESET);
    USER_GPIO_WRITE(GD32, E_CS, GPIO_PIN_RESET);
    USER_GPIO_WRITE(GD32, B_CS, GPIO_PIN_RESET);
    //关闭加热
    HAL_GPIO_WritePin(GD32_HEAT_BED_GPIO_Port, GD32_HEAT_BED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GD32_HEAT_NOZ_E_GPIO_Port, GD32_HEAT_NOZ_E_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GD32_HEAT_NOZ_B_GPIO_Port, GD32_HEAT_NOZ_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GD32_HEAT_CAVITY_GPIO_Port, GD32_HEAT_CAVITY_Pin, GPIO_PIN_RESET);
    //解锁电机
    HAL_GPIO_WritePin(GD32_X_EN_GPIO_Port, GD32_X_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GD32_Y_EN_GPIO_Port, GD32_Y_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GD32_Z_EN_GPIO_Port, GD32_Z_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GD32_X2_EN_GPIO_Port, GD32_X2_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GD32_E_EN_GPIO_Port, GD32_E_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GD32_B_EN_GPIO_Port, GD32_B_EN_Pin, GPIO_PIN_RESET); // 使能Z电机
    /*
    // GD32 USB HOST 异常处理
    extern USBH_HandleTypeDef  hUSB_Host;
    USBH_LL_Stop(&hUSB_Host);

    for (int i = 0; i < 11; i++)
    {
      hUSB_Host.Pipes[i] &= 0x7FFF;
    }

    USBH_LL_Start(&hUSB_Host);
    */
  }
}

