#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

extern volatile bool temp_meas_ready;
extern unsigned char soft_pwm_bed;
extern unsigned char soft_pwm[EXTRUDERS];
extern void updateTemperaturesFromRawValues();

#ifdef __cplusplus
}
#endif

namespace sg_grbl
{
  static unsigned char pid_finish;

  static void _pid_autotune_finish()
  {
    pid_finish = 0x80;
    USER_EchoLog("PID Autotune finished! Put the Kp, Ki and Kd constants into Configuration.h");
  }

  static void _pid_autotune_bad_extruder_num()
  {
    pid_finish = 0x81;
    USER_EchoLog("PID Autotune failed. Bad extruder number.");
  }

  static void _pid_autotune_tmp_too_high()
  {
    pid_finish = 0x82;
    USER_EchoLog("PID Autotune failed! Temperature too high");
  }

  static void _pid_autotune_timeout()
  {
    pid_finish = 0x83;
    USER_EchoLog("PID Autotune failed! Timeout");
  }

  static void _pid_autotune_serial_tmp(int extruder, float input)
  {
    if (extruder < 0)
    {
      USER_EchoLogStr("ok B:%f", input);
      USER_EchoLogStr(" @:%d", soft_pwm_bed);
    }
    else
    {
      USER_EchoLogStr("ok T:%f", input);
      USER_EchoLogStr(" @:%d", soft_pwm[extruder]);
    }

    USER_EchoLogStr("\r\n");
  }

  static void _pid_autotune_serial_pid(long bias, long d, float max, float min, long t_high, long t_low, int cycles)
  {
    USER_EchoLogStr("cycles:%d; bias: %ld", cycles, bias);
    USER_EchoLogStr(" d: %ld", d);
    USER_EchoLogStr(" min: %f", min);
    USER_EchoLogStr(" max: %f\r\n", max);
    USER_EchoLogStr(" pid_finish: %x\r\n", pid_finish);
    float Ku, Tu;
    float Kp, Ki, Kd;

    if (cycles > 2)
    {
      Ku = (4.0f * d) / (PI * (max - min) / 2.0f);
      Tu = ((float)(t_low + t_high) / 1000.0f);
      USER_EchoLogStr(" Ku: %f", Ku);
      USER_EchoLogStr(" Tu: %f", Tu);

      if (ccm_param.t_sys_data_current.enable_v5_extruder)
      {
        if (K5 == ccm_param.t_sys_data_current.model_id)
        {
          Kp = 0.33f * Ku;
          Ki = Kp / Tu;
          Kd = Kp * Tu / 3;
          USER_EchoLogStr(" K5 ");
          USER_EchoLogStr(" Kp: %f", Kp);
          USER_EchoLogStr(" Ki: %f", Ki);
          USER_EchoLogStr(" Kd: %f\r\n", Kd);
          USER_EchoLog("M301 P%f I%f D%f", Kp, Ki, Kd);
        }
        else
        {
          Kp = 0.1f * Ku;
          Ki = Kp / Tu;
          Kd = Kp * Tu / 8;
          USER_EchoLogStr(" K5 ");
          USER_EchoLogStr(" Kp: %f", Kp);
          USER_EchoLogStr(" Ki: %f", Ki);
          USER_EchoLogStr(" Kd: %f\r\n", Kd);
          USER_EchoLog("M301 P%f I%f D%f", Kp, Ki, Kd);
        }
      }
      else
      {
        Kp = 0.6f * Ku;
        Ki = 2 * Kp / Tu;
        Kd = Kp * Tu / 8;
        USER_EchoLogStr(" Clasic PID ");
        USER_EchoLogStr(" Kp: %f", Kp);
        USER_EchoLogStr(" Ki: %f", Ki);
        USER_EchoLogStr(" Kd: %f\r\n", Kd);
        Kp = 0.33f * Ku;
        Ki = Kp / Tu;
        Kd = Kp * Tu / 3;
        USER_EchoLogStr(" Some overshoot ");
        USER_EchoLogStr(" Kp: %f", Kp);
        USER_EchoLogStr(" Ki: %f", Ki);
        USER_EchoLogStr(" Kd: %f\r\n", Kd);
        Kp = 0.2f * Ku;
        Ki = 2 * Kp / Tu;
        Kd = Kp * Tu / 3;
        USER_EchoLogStr(" No overshoot ");
        USER_EchoLogStr(" Kp: %f", Kp);
        USER_EchoLogStr(" Ki: %f", Ki);
        USER_EchoLogStr(" Kd: %f\r\n", Kd);
      }
    }
  }

  //M303 Exx Cxx
  //对于喷嘴0,设定温度150,在5个周期后自动调节PID参数,并串口输出PID
  //需要手动记录PID
  void pid_autotune(float temp, int extruder, int ncycles)
  {
    float input = 0.0;
    int cycles = 0;
    bool heating = true;
    unsigned long temp_millis = MILLIS();
    unsigned long t1 = temp_millis;
    unsigned long t2 = temp_millis;
    long t_high = 0;
    long t_low = 0;
    long bias, d;
    float max = 0, min = 10000;
    pid_finish = 0;

    if ((extruder > EXTRUDERS) || (extruder < 0))
    {
      _pid_autotune_bad_extruder_num();
      return;
    }

    USER_EchoLog("PID Autotune start");
    temperature_disable_heater(); // switch off all heaters.

    if (extruder < 0)
    {
      soft_pwm_bed = (MAX_BED_POWER) / 2;
      bias = d = (MAX_BED_POWER) / 2;
    }
    else
    {
      soft_pwm[extruder] = (PID_MAX) / 2;
      bias = d = (PID_MAX) / 2;
    }

    for (;;)
    {
      osDelay(50);

      if (temp_meas_ready == true)  // temp sample ready
      {
        updateTemperaturesFromRawValues();
        input = (extruder < 0) ? temperature_get_bed_current() : temperature_get_extruder_current(extruder);
        max = max(max, input);
        min = min(min, input);

        if (heating == true && input > temp)
        {
          if (MILLIS() - t2 > 5000)
          {
            heating = false;

            if (extruder < 0)
              soft_pwm_bed = (bias - d) >> 1;
            else
              soft_pwm[extruder] = (bias - d) >> 1;

            t1 = MILLIS();
            t_high = t1 - t2;
            max = temp;
          }
        }

        if (heating == false && input < temp)
        {
          if (MILLIS() - t1 > 5000)
          {
            heating = true;
            t2 = MILLIS();
            t_low = t2 - t1;

            if (cycles > 0)
            {
              bias += (d * (t_high - t_low)) / (t_low + t_high);
              bias = constrain(bias, 20, (extruder < 0 ? (MAX_BED_POWER) : (PID_MAX)) - 20);

              if (bias > (extruder < 0 ? (MAX_BED_POWER) : (PID_MAX)) / 2) d = (extruder < 0 ? (MAX_BED_POWER) : (PID_MAX)) - 1 - bias;
              else d = bias;

              _pid_autotune_serial_pid(bias, d, max, min, t_high, t_low, cycles);
            }

            if (extruder < 0)
              soft_pwm_bed = (bias + d) >> 1;
            else
              soft_pwm[extruder] = (bias + d) >> 1;

            cycles++;
            min = temp;
          }
        }
      }

      if (input > (temp + 50))
      {
        _pid_autotune_tmp_too_high();
        return;
      }

      if (MILLIS() - temp_millis > 2000)
      {
        _pid_autotune_serial_tmp(extruder, input);
        temp_millis = MILLIS();
      }

      if (((MILLIS() - t1) + (MILLIS() - t2)) > (10L * 60L * 1000L * 2L))
      {
        _pid_autotune_timeout();
        return;
      }

      if (cycles > ncycles)
      {
        _pid_autotune_finish();
        return;
      }
    }
  }
}


