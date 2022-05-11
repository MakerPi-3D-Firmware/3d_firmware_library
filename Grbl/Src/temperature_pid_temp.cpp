#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"
#include "../../Grbl/Inc/temperature_pid_temp.h"
#include "../../Grbl/Inc/temperature_pid_output_factor.h"

namespace sg_grbl
{


#ifdef __cplusplus
  extern "C" {
#endif

#ifdef PIDTEMP

  //===========================================================================
  //=============================variables private ============================
  //===========================================================================

  static float Kp = DEFAULT_Kp;
  static float Ki = (DEFAULT_Ki *PID_dT);
  static float Kd = (DEFAULT_Kd / PID_dT);
#ifdef PID_ADD_EXTRUSION_RATE
  static float Kc = DEFAULT_Kc;
#endif

  //static cannot be external:
  static float temp_iState[EXTRUDERS] = { 0.0f };
  static float temp_dState[EXTRUDERS] = { 0.0f };
  static float pTerm[EXTRUDERS] = { 0.0f };
  static float iTerm[EXTRUDERS] = { 0.0f };
  static float dTerm[EXTRUDERS] = { 0.0f };
  static float temp_iState_min[EXTRUDERS] = { 0.0f };
  static float temp_iState_max[EXTRUDERS] = { 0.0f };
  static bool pid_reset[EXTRUDERS] = { false };


  //===========================================================================
  //=============================functions public  ============================
  //===========================================================================

  void temp_pid_extruder_set_kp(const float &p)
  {
    Kp = p;
  }

  void temp_pid_extruder_set_ki(const float &i)
  {
    Ki = i;// * PID_dT;  // scalePID_i
  }

  void temp_pid_extruder_set_kd(const float &d)
  {
    Kd = d;// / PID_dT;  // scalePID_d
  }

  float temp_pid_extruder_get_kp(void)
  {
    return Kp;
  }

  float temp_pid_extruder_get_ki(void)
  {
    return Ki;// * PID_dT;  // scalePID_i
  }

  float temp_pid_extruder_get_kd(void)
  {
    return Kd;// / PID_dT;  // scalePID_d
  }

  void temp_pid_extruder_set_kc(const float &c)
  {
#ifdef PID_ADD_EXTRUSION_RATE
    Kc = c;
#endif // #ifdef PID_ADD_EXTRUSION_RATE
  }

  void temp_pid_extruder_update(void)
  {
    for (int16_t e = 0; e < EXTRUDERS; e++)
    {
      temp_iState_max[e] = PID_INTEGRAL_DRIVE_MAX / Ki;
    }

    //    USER_EchoLog("PID %f %f %f %f", Kp, Ki, Kd, PID_dT);
  }

  void temp_pid_extruder_init(void)
  {
    if (ccm_param.t_sys_data_current.enable_v5_extruder)
    {
      if (K5 == ccm_param.t_sys_data_current.model_id)
      {
        //        30.719964
        //        Kp =  10.708734f;
        //        Ki =  0.348592f;
        //        Kd = 109.65743f;
        Kp =  11.708734f;
        Ki =  0.381144f;
        Kd = 119.897295f;
      }
      else if (K600 == ccm_param.t_sys_data_current.model_id || F400TP == ccm_param.t_sys_data_current.model_id)
      {
        //        Kp =  6.6319176f;
        //        Ki = 0.1372503f;
        //        Kd = 40.05668017f;
        //         M301 P5.963432 I0.118699 D37.450352
        Kp =  4.835416f;
        Ki = 0.144393f;
        Kd = 53.976139f;
      }
      else if (F300TP == ccm_param.t_sys_data_current.model_id)
      {
        //        Kp =  6.6319176f;
        //        Ki = 0.1372503f;
        //        Kd = 40.05668017f;
        //         M301 P5.963432 I0.118699 D37.450352
        Kp =  4.835416f;
        Ki = 0.144393f;
        Kd = 53.976139f;
      }
      else
      {
        //        M301 P6.238534 I0.389908 D99.816551
        Kp =  6.238534f;
        Ki = 0.389908f;
        Kd = 99.816551f;
      }

#ifdef PID_ADD_EXTRUSION_RATE
      Kc = DEFAULT_Kc;
#endif//PID_ADD_EXTRUSION_RATE
    }
    else if (1 == ccm_param.t_sys_data_current.enable_color_mixing)
    {
      Kp = 28.2659f;
      Ki = 0.7010f;
      Kd = 284.9204f;
#ifdef PID_ADD_EXTRUSION_RATE
      Kc = DEFAULT_Kc;
#endif//PID_ADD_EXTRUSION_RATE

      if (K600 == ccm_param.t_sys_data_current.model_id || F400TP == ccm_param.t_sys_data_current.model_id)
      {
        //        Kp =  6.6319176f;
        //        Ki = 0.1372503f;
        //        Kd = 40.05668017f;
        //         M301 P5.963432 I0.118699 D37.450352
        Kp =  4.835416f;
        Ki = 0.144393f;
        Kd = 53.976139f;
      }

      if (F300TP == ccm_param.t_sys_data_current.model_id)
      {
        //        Kp =  6.6319176f;
        //        Ki = 0.1372503f;
        //        Kd = 40.05668017f;
        //         M301 P5.963432 I0.118699 D37.450352
        Kp =  4.835416f;
        Ki = 0.144393f;
        Kd = 53.976139f;
      }

      if (P3_Pro_Mini == ccm_param.t_sys_data_current.model_id || P3_Pro == ccm_param.t_sys_data_current.model_id)
      {
        //        Kp =  6.6319176f;
        //        Ki = 0.1372503f;
        //        Kd = 40.05668017f;
        //         M301 P5.963432 I0.118699 D37.450352
        // P0.364016 I0.006533 D2.535465
        if (flash_param_t.extruder_type == EXTRUDER_TYPE_DUAL)
        {
          Kp = 0.364016f;
          Ki = 0.006533f;
          Kd = 2.535465f;
        }
        else
        {
          Kp =  12.0f;
          Ki = 0.56f;
          Kd = 48.7f;
        }
      }
    }
    else
    {
      Kp = DEFAULT_Kp;
      Ki = (DEFAULT_Ki * PID_dT);
      Kd = (DEFAULT_Kd / PID_dT);
#ifdef PID_ADD_EXTRUSION_RATE
      Kc = DEFAULT_Kc;
#endif//PID_ADD_EXTRUSION_RATE
    }

    // Finish init of mult extruder arrays
    for (int16_t e = 0; e < EXTRUDERS; e++)
    {
      temp_iState_min[e] = 0.0f;
      temp_iState_max[e] = PID_INTEGRAL_DRIVE_MAX / Ki;
    }

    //USER_EchoLog("PID %f %f %f %f", Kp, Ki, Kd, PID_dT);
    // call updatePID (similar to when we have processed M301)
  }

  void temp_pid_extruder_calc_pid_output(const int &extruder, const float &pid_input, const float &pid_error, float &pid_output)
  {
    if (pid_reset[extruder])
    {
      temp_iState[extruder] = 0.0f;
      temp_dState[extruder] = pid_input;
      pid_reset[extruder] = false;
    }

    //    // 系数小于1会影响PID加热速度
    //    if(1 == ccm_param.t_sys_data_current.enable_v5_extruder)
    //    {
    //      pTerm[extruder] = ccm_param.t_sys_data_current.pid_output_factor * Kp * pid_error;
    //    }
    //    else
    {
      pTerm[extruder] = Kp * pid_error;
    }
    temp_iState[extruder] += pid_error;
    temp_iState[extruder] = constrain(temp_iState[extruder], temp_iState_min[extruder], temp_iState_max[extruder]);
    iTerm[extruder] = Ki * temp_iState[extruder];
    //K1 defined in Configuration.h in the PID settings
#define K2 (1.0f-K1)
    dTerm[extruder] = (Kd * (pid_input - temp_dState[extruder])) * K2 + (K1 * dTerm[extruder]);
    pid_output = constrain(pTerm[extruder] + iTerm[extruder] - dTerm[extruder], 0.0f, (float)PID_MAX);
    temp_dState[extruder] = pid_input;
  }

  void temp_pid_extruder_set_reset(const int &extruder, const bool &value)
  {
    pid_reset[extruder] = value;
  }

#else
  void temp_pid_set_kp(const float &p) {}
  void temp_pid_set_ki(const float &i) {}
  void temp_pid_set_kd(const float &d) {}
  void temp_pid_set_kc(const float &c);
  void temp_pid_init(void) {}
  void temp_pid_update(void) {}
  void temp_pid_set_reset(const int &extruder, const bool &value) {}
  void temp_pid_set_d_state(const int &extruder, const bool &value) {}
  void temp_pid_calc_pid_output(const int &extruder, const float &pid_input, const float &pid_error, float &pid_output) {}
#endif //PIDTEMP

#ifdef PIDTEMPBED
  void temp_pid_bed_update(void)
  {
    temp_iState_max_bed = PID_INTEGRAL_DRIVE_MAX / bedKi;
  }

  void temp_pid_bed_init(void)
  {
    for (int16_t e = 0; e < EXTRUDERS; e++)
    {
      temp_iState_min_bed = 0.0;
      temp_iState_max_bed = PID_INTEGRAL_DRIVE_MAX / bedKi;
    }

    temp_pid_bed_update();
  }
#else
  void temp_pid_bed_update(void) {}
  void temp_pid_bed_init(void) {}
#endif // #ifdef PIDTEMPBED

  void temp_pid_init(void)
  {
    temp_pid_extruder_init();
    temp_pid_bed_init();
    temp_init_pid_output_factor_param();
  }
#ifdef __cplusplus
} // extern "C" {
#endif

}



