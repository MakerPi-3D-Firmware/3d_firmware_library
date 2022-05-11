#include "user_common.h"

#ifdef ENABLE_IWDG

extern IWDG_HandleTypeDef hiwdg;
//判断是否是看门狗的复位
static bool _user_iwdg_is_reset(void)
{
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
  {
    __HAL_RCC_CLEAR_RESET_FLAGS();
    return true;
  }

  return false;
}

__WEAK void user_iwdg_show_err_info(void)
{
}

void user_iwdg_init(void)
{
  // 触发看门狗，显示报警页面
  if (_user_iwdg_is_reset())
  {
    user_iwdg_show_err_info();
  }

  //开启独立看门狗
  __HAL_IWDG_START(&hiwdg);
}



//3s喂一次狗，5s没有喂狗会复位重启
void user_iwdg_refresh(void)
{
  static unsigned long RefreshWatchDogTimeOut = 0;

  if (RefreshWatchDogTimeOut < xTaskGetTickCount())
  {
    (void)HAL_IWDG_Refresh(&hiwdg);
    RefreshWatchDogTimeOut = xTaskGetTickCount() + 3000;
  }
}

#endif



