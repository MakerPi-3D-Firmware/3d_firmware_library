
#ifndef TEMPERATURE_PIN_H
#define TEMPERATURE_PIN_H

#include "Configuration.h"

namespace sg_grbl
{


#ifdef __cplusplus
  extern "C" {
#endif

  inline void temperature_set_bed_heater_status(bool on)
  {
    USER_HEAT_WRITE(HEAT_PIN_BED_INDEX, on);
  }

  inline void temperature_set_extruders_heater_status(bool on, int extruder)
  {
    bool value = (on ? HIGH : LOW);

    if (0 == extruder)
    {
      USER_HEAT_WRITE(HEAT_PIN_E_INDEX, value);
    }
    else if (1 == extruder)
    {
      USER_HEAT_WRITE(HEAT_PIN_B_INDEX, value);
    }
  }

  inline void temperature_adc_status_switch(int id)
  {
    extern ADC_HandleTypeDef hadc1;
    ADC_ChannelConfTypeDef sConfig;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    HAL_ADC_Stop(&hadc1);

    if (mcu_id == MCU_GD32F450IIH6)
    {
      sConfig.Channel = GD32_ADC_Pin[id].Channel;
      (void)HAL_ADC_ConfigChannel(&hadc1, &sConfig);
      (void)HAL_ADC_Start(&hadc1);
    }
    else if (mcu_id == MCU_STM32F429IGT6)
    {
      sConfig.Channel = STM32_ADC_Pin[id].Channel;
      (void)HAL_ADC_ConfigChannel(&hadc1, &sConfig);
      (void)HAL_ADC_Start(&hadc1);
    }
  }

  inline uint32_t temperature_adc_get_value(int id)
  {
    extern ADC_HandleTypeDef hadc1;
    HAL_ADC_PollForConversion(&hadc1, 50);//等待转换完成，第二个参数是超时时间，单位是ms.

    if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))//判断转换完成标志位是否设置
    {
      if (mcu_id == MCU_GD32F450IIH6)
      {
        if (HAL_ADC_GetValue(&hadc1) < 4092)
          return (HAL_ADC_GetValue(&hadc1)) / 4 - 16;
        else
          return (HAL_ADC_GetValue(&hadc1)) / 4;
      }
      else
      {
        return (HAL_ADC_GetValue(&hadc1));
      }
    }
    else
    {
      return 0;
    }
  }

  inline void temperature_set_cavity_pin_status(bool is_on)
  {
    USER_HEAT_WRITE(HEAT_PIN_CAVITY_INDEX, is_on);
  }

#ifdef __cplusplus
} //extern "C" {
#endif
}
#endif // TEMPERATURE_PIN_H

