#include "user_common_cpp.h"
#include "../../Grbl/Inc/temperature_timer.h"
#include "../../Grbl/Inc/temperature_pin.h"
#include "../../Grbl/Inc/temperature_pid_output_factor.h"
#include "../../Grbl/Inc/temperature_pid_temp.h"
#ifndef ENABLE_GUI_LVGL
#include "../../Feature/Inc/Alter.h"
#endif
namespace sg_grbl
{


#ifdef __cplusplus
  extern "C" {
#endif
#include "../../Grbl/Inc/thermistortables.h"

  extern ADC_HandleTypeDef hadc1;

  //===========================================================================
  //==============================public variables=============================
  //===========================================================================
  volatile int16_t target_temperature[EXTRUDERS] = { 0 };
  volatile int16_t target_temperature_bak[EXTRUDERS] = { 0 };
  volatile int16_t target_temperature_bed = 0;
  volatile uint32_t current_temperature_raw[EXTRUDERS] = { 0 };
  float current_temperature[EXTRUDERS] = { 0.0f };
  uint32_t current_temperature_bed_raw = 0;
  volatile float current_temperature_bed = 0.0f;

  float current_temperature_cavity = 0.0f;
  uint32_t current_temperature_cavity_raw = 0;
  int16_t target_temperature_cavity = 0;

  //  static bool is_update_pid_output_factor = false;

  static int current_fanspeed = 0;
  static unsigned char detect_pcb_faulse_count = 3; // 检测电路板异常报警计数
  //===========================================================================
  //=============================private variables============================
  //===========================================================================

#if EXTRUDERS > 3
# error Unsupported number of extruders
#elif EXTRUDERS > 2
# define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1, v2, v3 }
#elif EXTRUDERS > 1
# define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1, v2 }
#else
# define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1 }
#endif

#ifndef SOFT_PWM_SCALE
#define SOFT_PWM_SCALE 0
#endif

  static const int16_t heater_axis_mintemp[EXTRUDERS] = ARRAY_BY_EXTRUDERS(HEATER_0_MINTEMP, HEATER_1_MINTEMP, HEATER_2_MINTEMP);
  static int16_t heater_axis_maxtemp[EXTRUDERS] = ARRAY_BY_EXTRUDERS(HEATER_0_MAXTEMP, HEATER_1_MAXTEMP, HEATER_2_MAXTEMP);

  static int16_t bed_minttemp_raw = HEATER_BED_RAW_LO_TEMP;
  static int16_t bed_maxttemp_raw = HEATER_BED_RAW_HI_TEMP;

  volatile bool temp_meas_ready = false;
  unsigned char soft_pwm_bed;

  static unsigned long  previous_millis_bed_heater;
  unsigned char soft_pwm[EXTRUDERS];

  // Init min and max temp with extreme values to prevent false errors during startup
  static int16_t minttemp_raw[EXTRUDERS] = ARRAY_BY_EXTRUDERS(HEATER_0_RAW_LO_TEMP, HEATER_1_RAW_LO_TEMP, HEATER_2_RAW_LO_TEMP);
  static int16_t maxttemp_raw[EXTRUDERS] = ARRAY_BY_EXTRUDERS(HEATER_0_RAW_HI_TEMP, HEATER_1_RAW_HI_TEMP, HEATER_2_RAW_HI_TEMP);
  static int16_t maxttemp_raw_temp[EXTRUDERS] = ARRAY_BY_EXTRUDERS(HEATER_0_RAW_HI_TEMP, HEATER_1_RAW_HI_TEMP, HEATER_2_RAW_HI_TEMP);
  static int16_t minttemp[EXTRUDERS] = ARRAY_BY_EXTRUDERS(0, 0, 0);
  static int16_t maxttemp[EXTRUDERS] = ARRAY_BY_EXTRUDERS(16383, 16383, 16383);

  static void *heater_ttbl_map[EXTRUDERS] = ARRAY_BY_EXTRUDERS((void *)HEATER_0_TEMPTABLE, (void *)HEATER_1_TEMPTABLE, (void *)HEATER_2_TEMPTABLE);
  static uint8_t heater_ttbllen_map[EXTRUDERS] = ARRAY_BY_EXTRUDERS(HEATER_0_TEMPTABLE_LEN, HEATER_1_TEMPTABLE_LEN, HEATER_2_TEMPTABLE_LEN);
  static float analog2temp(uint32_t raw, uint8_t e);
  static float analog2tempBed(uint32_t raw);
  void updateTemperaturesFromRawValues(void);

  static uint8_t temp_error_status = NoError;

  //===========================================================================
  //=============================   functions      ============================
  //===========================================================================

  static inline void temp_extruders_init(void)
  {
    // Finish init of mult extruder arrays
    for (int16_t e = 0; e < EXTRUDERS; e++)
    {
      // populate with the first value
      maxttemp[e] = maxttemp[0];
      minttemp[e] = heater_axis_mintemp[e];

      while (analog2temp(minttemp_raw[e], e) < heater_axis_mintemp[e])
      {
        if (minttemp_raw[e] < maxttemp_raw[e])
          minttemp_raw[e] += OVERSAMPLENR;
        else
          minttemp_raw[e] -= OVERSAMPLENR;
      }

      maxttemp[e] = heater_axis_maxtemp[e];

      while (analog2temp(maxttemp_raw[e], e) > heater_axis_maxtemp[e])
      {
        if (minttemp_raw[e] < maxttemp_raw[e])
          maxttemp_raw[e] -= OVERSAMPLENR;
        else
          maxttemp_raw[e] += OVERSAMPLENR;
      }
    }
  }

  void temperature_set_heater_maxtemp(int axis, int value)
  {
    if (axis < EXTRUDERS && axis >= 0)
    {
      heater_axis_maxtemp[axis] = value;
      maxttemp_raw[axis] = maxttemp_raw_temp[axis];
      temp_extruders_init();
    }
  }

  int temperature_get_heater_maxtemp(int axis)
  {
    if (axis < EXTRUDERS && axis >= 0)
      return heater_axis_maxtemp[axis];
    else
      return -1;
  }

  int temperature_get_error_status(void)
  {
    return temp_error_status;
  }

  void temperature_set_error_status(char Value)
  {
    temp_error_status = Value;
  }
#define TEMP_LIST_SIZE 20

  typedef struct
  {
    char level;
    int temp;
    int error;
    // int temp_raw;//current_temperature_raw;
  } Temp_List;

  uint8_t tempListIndex = 0;
  Temp_List temp_list0[TEMP_LIST_SIZE];

  void clr_tempList()
  {
    tempListIndex = 0;

    //  tempCheckTrigger=false;
    for (int i = 0; i < TEMP_LIST_SIZE; i++)
    {
      temp_list0[i].temp = 0;
      temp_list0[i].level = 0;
      temp_list0[i].error = 0;
    }
  }

  //10个数，去掉最高和最低，求平均
  static int calc_ave(const int *const p, const int size)
  {
    int sum = 0;
    int max = p[0];
    int min = p[0];

    for (int i = 0; i < size; i++)
    {
      sum += p[i];

      if (max < p[i])max = p[i];

      if (min > p[i])min = p[i];
    }

    //  SERIAL_ECHOPGM("1 ");
    sum -= max; //去除最大值
    sum -= min; //去除最大值
    //  SERIAL_ECHOPGM("2 ");
    return sum / (size - 2);
  }

  //20个数，比较前十个与后十个的平均值之差
  static int calc_diff(const int *p, const int size)
  {
    return calc_ave(p + size / 2, size / 2) - calc_ave(p, size / 2);
  }

  /*
  迟滞比较

               |<-------^
        反向触发 |        | 正向触发
             A___v_______>|____B
                tr       tf
    reverse_trigger   forward_trigger

  */

  //suzhiwei 20160409: 迟滞比较器 hysteresis comparator
  bool hysteresis(bool &trigger, const int current, const int  min, const int max)
  {
    //靠近目标，正向触发
    if (!trigger && current < min)
    {
      trigger = true;
    }

    //远离目标，反向触发
    if (trigger && current > max)
    {
      trigger = false;
    }

    return trigger;
  }

  void resetTrigger(bool &trigger)
  {
    trigger = false;
  }

  void temp_list_check(Temp_List p_temp_list[])
  {
    static char error1_cnt = 0;
    //  static char error2_cnt=0;
    Temp_List *temp_list = p_temp_list;
    const int level = temp_list[0].level; //温度变化状态取首个状态为参考
    int level_max = 0;
    int temp_log[TEMP_LIST_SIZE] = {0};

    //  int error_log[TEMP_LIST_SIZE]={0};
    //提取最大的有效温度记录值（温度变化状态必须相同）
    while ((level_max < TEMP_LIST_SIZE) && (temp_list[level_max].level == level))
    {
      temp_log[level_max] = temp_list[level_max].temp;
      //    error_log[level_max]=temp_list[level_max].error;
      level_max++;
    }

    //如果处于相同阶段的温度记录值<规定的有效记录数, 则不判断
    if (level_max < 11)
    {
      return;
    }

    int diff_temp = 0;

    switch (level)
    {
    case 1://升温
      //计算level_max*1s 时间内后半段与前半段的温度（过滤max和min）均差，小于2则认为加热失败
      diff_temp = calc_diff(temp_log, level_max);

      if (diff_temp * TEMP_LIST_SIZE / level_max < 2)
      {
        error1_cnt++;

        //连续2次，触发错误提醒
        if (error1_cnt >= 2)
        {
          //          Stop();// 20s*3=180s=1min
          error1_cnt = 0;

          if (flash_param_t.extruder_type != EXTRUDER_TYPE_LASER && flash_param_t.extruder_type != EXTRUDER_TYPE_DRUG)
          {
            temperature_disable_heater();
            temp_error_status = HeatFailError;
          }
        }
      }
      else
      {
        error1_cnt = 0;
      }

      break;

    case 2:
      break;

    case 3://降温
      break;
    }
  }

  //获取temp_list
  void temp_list_get(int16_t e)
  {
    static unsigned long temp_list_get_peroid[EXTRUDERS] = {0};
    static char tempLevel = 0;
    //    static char tempLevel_old = 0;
    static int target_temperature_old[EXTRUDERS] = {0};
    int tempError = 0 ;

    if (temp_list_get_peroid[e] > MILLIS())
    {
      return;//未到1秒返回
    }
    else
    {
      temp_list_get_peroid[e] = MILLIS() + 1000; //每1s获取数据
    }

    tempError = target_temperature[e] - (int)current_temperature[e]; //与目标温度差距

    if (target_temperature[e] < 50 || abs(tempError) < 2)
    {
      goto label_reset_temp_list; //目标温度小于50度 或 温度差2度 则为非重要检测，复位并返回
    }

    if (target_temperature_old[e] != target_temperature[e])
    {
      target_temperature_old[e] = target_temperature[e];
      goto label_reset_temp_list;//如果改变了目标温度则复位检测
    }

    // target_temperature_old[e] = target_temperature[0]; //更新目标温度
    //根据error是否在>20、-10~20、<-10，将level赋值1~3：升温1 恒温（恒温阶段去掉温差小于2的部分）2 降温3,
    tempLevel = tempError > 20 ? 1 : (tempError > - 10 ? 2 : 3);

    if (tempLevel > 2) //只检测升温1 恒温2
    {
      goto label_reset_temp_list;//降温则复位检测
    }

    if (tempListIndex >= 1 && tempListIndex < 10 && tempLevel != temp_list0[tempListIndex - 1].level)
    {
      goto label_reset_temp_list;//如果温度状态改变，则复位检测
    }

    temp_list0[tempListIndex].level =  tempLevel;
    temp_list0[tempListIndex].error = tempError;
    temp_list0[tempListIndex].temp  = (int)current_temperature[0];
    tempListIndex++;//每秒下一个数组元素

    if (tempListIndex >= TEMP_LIST_SIZE)
    {
      temp_list_check(temp_list0);//每TEMP_LIST_SIZE秒检查一次
      goto label_reset_temp_list;//检测完后复位检测
    }

    //label_ok_temp_list:
    return;
label_reset_temp_list:
    clr_tempList();
    return;
  }

  extern volatile float current_pid_output_factor[EXTRUDERS];
  void cal_pid_output(const int16_t &extruder, const float &pid_input, float &pid_output)
  {
    float pct_of_extruder_tmp_power = 1.0f; // 单色喷嘴温度功率百分比
    float pid_error = target_temperature[extruder] - pid_input;

    // 当前与目标温度之差小于0，重置温度系数参数
    if (pid_error < 0)
      temp_reset_pid_output_factor_param(extruder);

    if (pid_error > (ccm_param.t_sys_data_current.enable_color_mixing ? 45 : 50))
    {
      temp_accumulated_temp_per_second(pid_input, extruder);
      pid_output = BANG_MAX;
      temp_pid_extruder_set_reset(extruder, true);
    }
    else if (pid_error > (ccm_param.t_sys_data_current.enable_color_mixing ? 25 : 32))
    {
      float factor = 1.0f;
      pct_of_extruder_tmp_power = 0.85f;

      if (temp_cal_pid_output_factor(factor, extruder)) // 计算pid_output输出系数
      {
        current_pid_output_factor[extruder] = factor;
        //        is_update_pid_output_factor = true;
      }

      if (current_pid_output_factor[extruder] < 0.000001f)
      {
        current_pid_output_factor[extruder] = 1.0f;
      }

      // V5喷嘴一直开启扇热风扇，可能会影响加热速率，触发加热失败，M41G出现这种情况
      if (1 == ccm_param.t_sys_data_current.enable_v5_extruder)
      {
        pct_of_extruder_tmp_power = 1.0f / current_pid_output_factor[extruder];
      }

      pid_output = (current_fanspeed) ? (BANG_MAX) : (BANG_MAX * pct_of_extruder_tmp_power * current_pid_output_factor[extruder]);
      temp_pid_extruder_set_reset(extruder, true);
    }
    else if ((0 == ccm_param.t_sys_data_current.enable_v5_extruder || K5 == ccm_param.t_sys_data_current.model_id) && pid_error > PID_FUNCTIONAL_RANGE)
    {
      pct_of_extruder_tmp_power = 0.75f;

      if (ccm_param.t_sys_data_current.enable_color_mixing) // 混色喷嘴温度功率百分比
      {
        pct_of_extruder_tmp_power = 0.75f;
      }

      // V5喷嘴一直开启扇热风扇，可能会影响加热速率，触发加热失败，M41G出现这种情况
      if (ccm_param.t_sys_data_current.enable_v5_extruder) // V5喷嘴温度功率百分比
      {
        pct_of_extruder_tmp_power = 1.0f / current_pid_output_factor[extruder];
      }

      pid_output = (current_fanspeed) ? (BANG_MAX) : (BANG_MAX * current_pid_output_factor[extruder] * pct_of_extruder_tmp_power);

      // 混色噴嘴冬天加熱不上，功率為30w，取最大加熱功率
      if (ccm_param.t_sys_data_current.enable_color_mixing && current_pid_output_factor[extruder] == temp_get_pid_output_max_factor(extruder))
      {
        pid_output = BANG_MAX;
      }

      temp_pid_extruder_set_reset(extruder, true);
    }
    /*平滑加热曲线[end]  author:yangh@soongon.com date:20150324*/
    else if (pid_error < -PID_FUNCTIONAL_RANGE - ((ccm_param.t_sys_data_current.enable_v5_extruder && K5 != ccm_param.t_sys_data_current.model_id) ? 5 : 0) || target_temperature[extruder] == 0)
    {
      pid_output = 0;
      temp_pid_extruder_set_reset(extruder, true);
    }
    else
    {
      temp_pid_extruder_calc_pid_output(extruder, pid_input, pid_error, pid_output);
    }
  }

  static void manage_extruders_heater(void)
  {
    float pid_input = 0.0f;
    float pid_output = 0.0f;
    int heater_maxtemp = 0;

    for (int16_t e = 0; e < EXTRUDERS; e++)
    {
      pid_input = current_temperature[e];
      heater_maxtemp = heater_axis_maxtemp[e];

      if (ccm_param.t_sys_data_current.enable_color_mixing)
        heater_maxtemp -= 10;

      // 计算pid_output
      cal_pid_output(e, pid_input, pid_output);

      if (flash_param_t.extruder_type != EXTRUDER_TYPE_LASER && flash_param_t.extruder_type != EXTRUDER_TYPE_DRUG)
      {
        // Check if temperature is within the correct range
        if ((current_temperature[e] > minttemp[e]) && (current_temperature[e] < maxttemp[e]))
        {
          soft_pwm[e] = (unsigned char)((int)pid_output >> 1);
        }
        else
        {
          soft_pwm[e] = 0;
        }
      }
      else
      {
        // Check if temperature is within the correct range
        if (current_temperature[e] < maxttemp[e])
        {
          soft_pwm[e] = (unsigned char)((int)pid_output >> 1);
        }
        else
        {
          soft_pwm[e] = 0;
        }
      }

      temp_list_get(e);
    } // End extruder for loop
  }

  static void manage_bed_heater(void)
  {
#if defined(BED_MAXTEMP) && (TEMP_SENSOR_BED != 0)

    // Check if temperature is within the correct range
    if ((current_temperature_bed > BED_MINTEMP) && (current_temperature_bed < BED_MAXTEMP))
    {
      if (current_temperature_bed >= target_temperature_bed)
      {
        soft_pwm_bed = 0;
      }
      else
      {
        soft_pwm_bed = MAX_BED_POWER >> 1;
      }
    }
    else
    {
      soft_pwm_bed = 0;
      temperature_set_bed_heater_status(false);
    }

#endif // defined(BED_MAXTEMP) && (TEMP_SENSOR_BED != 0)
  }

  static void manage_cavity_heater(void)
  {
    if (current_temperature_cavity >= target_temperature_cavity)
    {
      temperature_set_cavity_pin_status(LOW);
    }
    else
    {
      temperature_set_cavity_pin_status(HIGH);
    }
  }

  void temperature_manage_heater(const int ext_fanspeed)
  {
    if (temp_meas_ready != true)  //better readability
      return;

    current_fanspeed = ext_fanspeed;
    updateTemperaturesFromRawValues();

    if (current_temperature[0] > heater_axis_maxtemp[0]) //当前温度大于最大温度就发出警报，并且不能关闭警报
    {
      if (detect_pcb_faulse_count == 0)
        temp_error_status = DETECT_PCB_FAULSE;

      detect_pcb_faulse_count--;
    }

    // 喷嘴加热控制
    manage_extruders_heater();

    if (MILLIS() - previous_millis_bed_heater < BED_CHECK_INTERVAL)
      return;

    previous_millis_bed_heater = MILLIS();
    // 热床加热控制
    manage_bed_heater();

    // 腔体温度控制
    if (ccm_param.t_sys.enable_cavity_temp)
      manage_cavity_heater();
  }


  // Derived from RepRap FiveD extruder::getTemperature()
  // For hot end temperature measurement.
  static float analog2temp(uint32_t raw, uint8_t e)
  {
    if (e >= EXTRUDERS)
    {
      temperature_disable_heater();
      return 0.0;
    }

    if (heater_ttbl_map[e] != NULL)
    {
      float celsius = 0;
      uint8_t i;
      short(*tt)[][2] = (short(*)[][2])(heater_ttbl_map[e]);

      for (i = 1; i < heater_ttbllen_map[e]; i++)
      {
        if ((*tt)[i][0] > raw)
        {
          celsius = (*tt)[i - 1][1] +
                    (raw - (*tt)[i - 1][0]) *
                    (float)((*tt)[i][1] - (*tt)[i - 1][1]) /
                    (float)((*tt)[i][0] - (*tt)[i - 1][0]);
          break;
        }
      }

      // Overflow: Set to last value in the table
      if (i == heater_ttbllen_map[e]) celsius = (*tt)[i - 1][1];

      return celsius;
    }

    return (raw * ((5.0f * 100.0f) / 1024.0f) / OVERSAMPLENR);
  }

  // Derived from RepRap FiveD extruder::getTemperature()
  // For bed temperature measurement.
  static float analog2tempBed(uint32_t raw)
  {
    float celsius = 0;
    uint8_t i;

    for (i = 1; i < BEDTEMPTABLE_LEN; i++)
    {
      if (BEDTEMPTABLE[i][0] > raw)
      {
        celsius  = BEDTEMPTABLE[i - 1][1] +
                   (raw - BEDTEMPTABLE[i - 1][0]) *
                   (float)(BEDTEMPTABLE[i][1] - BEDTEMPTABLE[i - 1][1]) /
                   (float)(BEDTEMPTABLE[i][0] - BEDTEMPTABLE[i - 1][0]);
        break;
      }
    }

    // Overflow: Set to last value in the table
    if (i == BEDTEMPTABLE_LEN) celsius = BEDTEMPTABLE[i - 1][1];

    return celsius;
  }

  /* Called to get the raw values into the the actual temperatures. The raw values are created in interrupt context,
      and this function is called from normal context as it is too slow to run in interrupts and will block the stepper routine otherwise */
  void updateTemperaturesFromRawValues()
  {
    for (uint8_t e = 0; e < EXTRUDERS; e++)
    {
      current_temperature[e] = analog2temp(current_temperature_raw[e], e);
    }

    current_temperature_bed = analog2tempBed(current_temperature_bed_raw);

    if (ccm_param.t_sys.enable_cavity_temp)
      current_temperature_cavity  = analog2tempBed(current_temperature_cavity_raw);

    TASK_ENTER_CRITICAL();
    temp_meas_ready = false;
    TASK_EXIT_CRITICAL();
  }

  void temperature_init(void)
  {
    temp_extruders_init();
#ifdef BED_MAXTEMP

    while (analog2tempBed(bed_maxttemp_raw) > BED_MAXTEMP)
    {
      if (bed_minttemp_raw < bed_maxttemp_raw)
        bed_maxttemp_raw -= OVERSAMPLENR;
      else
        bed_maxttemp_raw += OVERSAMPLENR;
    }

#endif //BED_MAXTEMP
    temp_pid_init();
    // Interleave temperature interrupt with millies interrupt
    temperature_timer_start();
  }

  void temperature_disable_heater()
  {
    target_temperature_bed = 0.0f;

    if (ccm_param.t_sys.enable_cavity_temp)
      target_temperature_cavity = 0.0f;

    soft_pwm_bed = 0;
    temperature_set_bed_heater_status(false);

    for (int16_t i = 0; i < EXTRUDERS; i++)
    {
      target_temperature[i] = 0.0f;
      soft_pwm[i] = 0;
      temperature_set_extruders_heater_status(false, i);
    }
  }

  void temp_status_switch(unsigned long *raw_temp_0_value, unsigned long &raw_temp_bed_value, unsigned long &raw_temp_cavity_value, unsigned char &temp_count)
  {
    static unsigned char temp_state = 10;

    switch (temp_state)
    {
    case 0: // Prepare TEMP_0
      temperature_adc_status_switch(HEAT_PIN_E_INDEX);
      temp_state = 1;
      break;

    case 1: // Measure TEMP_0
      raw_temp_0_value[0] += temperature_adc_get_value(HEAT_PIN_E_INDEX);
      temp_state = 2;
      break;

    case 2: // Prepare TEMP_BED
      temperature_adc_status_switch(HEAT_PIN_BED_INDEX);
      temp_state = 3;
      break;

    case 3: // Measure TEMP_BED
      raw_temp_bed_value += temperature_adc_get_value(HEAT_PIN_BED_INDEX);
      temp_state = 4;
      break;

    case 4: // Prepare TEMP_1
      if (ccm_param.t_sys.enable_cavity_temp)
        temperature_adc_status_switch(HEAT_PIN_CAVITY_INDEX);

      temp_state = 5;
      break;

    case 5: // Prepare TEMP_1
      if (ccm_param.t_sys.enable_cavity_temp)
        raw_temp_cavity_value += temperature_adc_get_value(HEAT_PIN_CAVITY_INDEX);

      temp_state = 6;
      break;

    case 6: // Prepare TEMP_2
      temperature_adc_status_switch(HEAT_PIN_B_INDEX);
      temp_state = 7;
      break;

    case 7: // Measure TEMP_2
      raw_temp_0_value[1] += temperature_adc_get_value(HEAT_PIN_B_INDEX);
      temp_state = 8;
      break;

    case 8:
      temp_state = 9;
      break;

    case 9:
      temp_state = 0;
      temp_count++;
      break;

    case 10:
      temp_state = 0;
      break;

    default:
      break;
    }
  }

  static void temperature_extruder_temp_check_error(int extruder)
  {
    if ((minttemp_raw[extruder] > maxttemp_raw[extruder] && current_temperature_raw[extruder] <= maxttemp_raw[extruder]) ||
        (minttemp_raw[extruder] <= maxttemp_raw[extruder] && current_temperature_raw[extruder] >= maxttemp_raw[extruder]))
    {
      temperature_disable_heater();
      temp_error_status = MaxTempError;
    }

    if ((minttemp_raw[extruder] > maxttemp_raw[extruder] && current_temperature_raw[extruder] >= minttemp_raw[extruder]) ||
        (minttemp_raw[extruder] <= maxttemp_raw[extruder] && current_temperature_raw[extruder] <= minttemp_raw[extruder]))
    {
      if (flash_param_t.extruder_type != EXTRUDER_TYPE_LASER && flash_param_t.extruder_type != EXTRUDER_TYPE_DRUG)
      {
        temperature_disable_heater();
        temp_error_status = MinTempError;
      }
#ifndef ENABLE_GUI_LVGL
      if (Check_TempMinErr() && !ccm_param.t_sys_data_current.IsLaser)
      {
        temperature_disable_heater();
        temp_error_status = MinTempError;
      }
#endif
    }
  }

  static void temperature_bed_temp_check_error(void)
  {
#if defined(BED_MAXTEMP) && (TEMP_SENSOR_BED != 0)

    if ((bed_minttemp_raw > bed_maxttemp_raw && current_temperature_bed_raw <= bed_maxttemp_raw) ||
        (bed_minttemp_raw <= bed_maxttemp_raw && current_temperature_bed_raw >= bed_maxttemp_raw))
    {
      target_temperature_bed = 0;
      temperature_set_bed_heater_status(false);
      temperature_disable_heater();
      temp_error_status = MaxTempBedError;
    }

#endif // defined(BED_MAXTEMP) && (TEMP_SENSOR_BED != 0)
  }

  void tp_software_pwm(void)
  {
    static unsigned char pwm_count = (1 << SOFT_PWM_SCALE);
    static unsigned char soft_pwm_0;
    static unsigned char soft_pwm_1;
    static unsigned char soft_pwm_b;

    if (flash_param_t.extruder_type != EXTRUDER_TYPE_LASER && flash_param_t.extruder_type != EXTRUDER_TYPE_DRUG)
    {
      if (pwm_count == 0)
      {
        soft_pwm_0 = soft_pwm[0];

        if (soft_pwm_0 > 0)
          temperature_set_extruders_heater_status(true, 0);

        soft_pwm_1 = soft_pwm[1];

        if (soft_pwm_1 > 0)
          temperature_set_extruders_heater_status(true, 1);

        soft_pwm_b = soft_pwm_bed;

        if (soft_pwm_b > 0)
          temperature_set_bed_heater_status(true);
      }

      if (soft_pwm_0 <= pwm_count)
        temperature_set_extruders_heater_status(false, 0);

      if (soft_pwm_1 <= pwm_count)
        temperature_set_extruders_heater_status(false, 1);

      if (soft_pwm_b <= pwm_count)
        temperature_set_bed_heater_status(false);

      pwm_count += (1 << SOFT_PWM_SCALE);
      pwm_count &= 0x7f;
    }
    else
    {
      if (target_temperature[0] > current_temperature[0])
        temperature_set_extruders_heater_status(true, 0);
      else
        temperature_set_extruders_heater_status(false, 0);
    }
  }

  bool temperature_update(void)
  {
    //these variables are only accesible from the ISR, but static, so they don't lose their value
    static unsigned char temp_count = 0;
    static unsigned long raw_temp_0_value[2] = {0};
    static unsigned long raw_temp_bed_value = 0;
    static unsigned long raw_temp_cavity_value = 0;
    tp_software_pwm();
    temp_status_switch(&raw_temp_0_value[0], raw_temp_bed_value, raw_temp_cavity_value, temp_count);

    if (temp_count >= OVERSAMPLENR)  // 10 * 16 * 1/(16000000/64/256)  = 164ms.For stduino 10/2 * 16 * 1/1000 = 80ms
    {
      if (!temp_meas_ready)   //Only update the raw values if they have been read. Else we could be updating them during reading.
      {
        current_temperature_raw[0] = (int16_t)raw_temp_0_value[0];
        current_temperature_raw[1] = (int16_t)raw_temp_0_value[1];
        current_temperature_bed_raw = (int16_t)raw_temp_bed_value;

        if (ccm_param.t_sys.enable_cavity_temp)
          current_temperature_cavity_raw = (int16_t)raw_temp_cavity_value;
      }

      temp_meas_ready = true;
      temp_count = 0;
      raw_temp_0_value[0] = 0;
      raw_temp_0_value[1] = 0;
      raw_temp_bed_value = 0;

      if (ccm_param.t_sys.enable_cavity_temp)
        raw_temp_cavity_value = 0;

      temperature_extruder_temp_check_error(0);

      if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
        temperature_extruder_temp_check_error(1);

      /* No bed MINTEMP error? */
      temperature_bed_temp_check_error();
      return true;
    }
    else
    {
      return false;
    }
  }

  float temperature_get_extruder_current(int extruder)
  {
    return current_temperature[extruder];
  }

  float temperature_get_bed_current(void)
  {
    return current_temperature_bed;
  }

  float temperature_get_extruder_target(int extruder)
  {
    return target_temperature[extruder];
  }

  float temperature_get_bed_target(void)
  {
    return target_temperature_bed;
  }

  int16_t target_temperature_buf;
  void temperature_set_extruder_target(const float celsius, int extruder)
  {
    target_temperature[extruder] = celsius;
    target_temperature_bak[extruder] = celsius;
    target_temperature_buf = celsius;

    if (flash_param_t.extruder_type == EXTRUDER_TYPE_LASER || flash_param_t.extruder_type == EXTRUDER_TYPE_DRUG)
    {
      if (target_temperature[extruder] > 60)
        target_temperature[extruder] = 60;
    }
  }

  void temperature_set_bed_target(const float celsius)
  {
    target_temperature_bed = celsius;
  }

  float temperature_get_cavity_target(void)
  {
    return target_temperature_cavity;
  }

  void temperature_set_cavity_target(const float celsius)
  {
    if (ccm_param.t_sys.enable_cavity_temp)
      target_temperature_cavity = celsius;
  }

  float temperature_get_cavity_current(void)
  {
    return current_temperature_cavity;
  }

  bool temperature_is_extruder_heating(int extruder)
  {
    return target_temperature[extruder] > current_temperature[extruder];
  }

  bool temperature_is_bed_heating(void)
  {
    return target_temperature_bed > current_temperature_bed;
  }

  bool temperature_is_extruder_cooling(int extruder)
  {
    return target_temperature[extruder] < current_temperature[extruder];
  }

  bool temperature_is_bed_cooling(void)
  {
    return target_temperature_bed < current_temperature_bed;
  }

  int temperature_get_extruder_heater_power(int heater)
  {
    return soft_pwm[heater];
  }

  int temperature_get_bed_heater_power(void)
  {
    return soft_pwm_bed;
  }

  //  bool temperature_update_pid_output_factor(void)
  //  {
  //    bool factor = is_update_pid_output_factor;
  //    is_update_pid_output_factor = false;
  //    return factor;
  //  }
#ifdef __cplusplus
}
#endif

}





