#ifndef BOARD_TEST_H
#define BOARD_TEST_H

#ifdef HAS_BOARD_TEST

#ifdef __cplusplus
extern "C" {
#endif

// ²âÊÔ¹Ì¼þ½Ó¿Ú
extern void board_test_display_function(void);
extern void board_test_model_select(void);
extern void board_test_cal_heat_time_gui(void);
extern void board_test_cal_touch_count(void);
extern void board_test_pressure(void);

#ifdef __cplusplus
} //extern "C" {
#endif

class BoardTest
{
public:
  BoardTest();
  bool guiInterface(void);
  void displayInit(void);
  void displayText(void);
  bool touchCheck(void);
  void modelSelect(void);

  void toggleStepStatus(void);
  void toggleFanStatus(void);
  void toggleHeatStatus(void);

  void process(void);

  void runMaxPos(void);
  bool calHeatTimeGui(void);
  void calHeatTime(void);

  uint8_t ERRTouchCount(void);
  void PressureTest(void);

private:
  uint8_t isStepTest;
  uint8_t isFanTest;
  uint8_t isHeatTest;

  char nozzleHeatTimeStr[32];
  char bed50HeatTimeStr[32];
  char bed70HeatTimeStr[32];
  char bed115HeatTimeStr[32];
  int heatStatus;
  uint32_t clockTime;

  uint8_t processOn;

  void stepTest(void);
  void fanTest(void);
  void heatTest(void);

  bool calHeatTimeGuiTouchCheck(void);
};

extern BoardTest boardTest;

#endif

#endif // BOARD_TEST_H

