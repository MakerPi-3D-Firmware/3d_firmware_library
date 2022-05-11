#include "user_common_cpp.h"
#ifdef __cplusplus
extern "C" {
#endif

void TIM3_IRQHandler_process(void)
{
  sg_grbl::temperature_update();
}

void TIM4_IRQHandler_process(void)
{
  sg_grbl::st_process();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if ((GPIO_Pin == GPIO_PIN_3) && ccm_param.t_sys_data_current.enable_powerOff_recovery) // PD3
  {
    if (flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG || flash_param_t.extruder_type == EXTRUDER_TYPE_LASER)
    {
      __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
      return;
    }

    if (ccm_param.t_sys.is_print_e1_offset_flag)
    {
      return;
    }

    if ((print_status.is_printing && !ccm_param.t_sys.is_power_off_rec_flag) || ccm_param.t_sys.is_power_off_rec_data_chg)
    {
      if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3) == GPIO_PIN_SET)
      {
        if (!ccm_param.t_sys.is_power_off_rec_flag)
        {
          memmove((planner_running_status_t *)&ccm_param.flash_poweroff_recovery,
                  (planner_running_status_t *)&ccm_param.runningStatus[ccm_param.block_buffer_tail], sizeof(planner_running_status_t));
        }

        ccm_param.flash_poweroff_recovery.flag = 1;
        flash_poweroff_data_save();
        HAL_NVIC_SystemReset();
      }

      __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
    }
  }
}

#ifdef __cplusplus
}
#endif









