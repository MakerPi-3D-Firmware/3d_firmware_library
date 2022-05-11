#ifndef __PROCESS_COMMAND_H
#define __PROCESS_COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

extern float homing_feedrate[MAX_NUM_AXIS];
void kill(void);
void process_commands(const uint8_t gcodeType, const uint32_t file_pos, const char *gcodeStr, const long layer_count, const long current_layer);
void manage_inactivity(void);

int GetFeedMultiply(void);
void SetFeedMultiply(int FeedMultiplyValue);

uint8_t GetM109HeatingStatus(void);
void SetM109HeatingStatus(uint8_t StatusValue);
float GetCurrentPosition(uint8_t AXIS);
void SetCurrentPosition(uint8_t AXIS, float AxisPos);
void clamp_to_software_endstops(volatile float (&target)[MAX_NUM_AXIS]);

int GetFeedRate(void);
void SetFeedRate(float Value);

//extern uint8_t serialPrintStatus(void);
void stepper_quick_stop(void); ///< 快速停止运动
void process_buffer_line_basic(volatile float *position, const float feed_rate, const int feed_multiply,
                               const int extruder_multiply, const unsigned long sd_pos, bool is_sync_z);
void process_buffer_line(volatile float *position, const float feed_rate, const int feed_multiply,
                         const int extruder_multiply, const unsigned long sd_pos);
void process_buffer_line_normal(volatile float *position, const float feed_rate);
#ifdef __cplusplus
} // extern "C" {
#endif

class GcodeCommand
{
public:
  GcodeCommand();
  void process(const uint8_t gcodeType);
  void set_curr_file_pos(const uint32_t value)
  {
    current_file_pos = value;
  }
private:
  void clearToSend(void);
  inline void GcodeCMD(void);                                         ///< Gcode命令


  volatile uint32_t current_file_pos;
};

extern GcodeCommand gcodeCommand;

void get_coordinates(void);




#endif /* __PROCESS_COMMAND_H */
