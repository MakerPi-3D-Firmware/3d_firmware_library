#ifndef GUI_COMMON_LANGUAGES_H
#define GUI_COMMON_LANGUAGES_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma  diag_suppress 870

#define LCD_HOMING_STR_CN "归零中，请等待..."
#define LCD_HOMING_STR_EN "Homing, please wait..."
#define LCD_HOMING_STR_TC "归零中，请等待..."

#define LCD_XYZ_MOVING_STR_CN "移动中，请等待..."
#define LCD_XYZ_MOVING_STR_EN "Moving, please wait..."
#define LCD_XYZ_MOVING_STR_TC "移动中，请等待..."

#define LCD_PAUSING_STR_CN "暂停中，请等待..."
#define LCD_PAUSING_STR_EN "Pausing, please wait..."
#define LCD_PAUSING_STR_TC "暂停中，请等待..."

#define LCD_RESUMING_STR_CN "恢复中，请等待..."
#define LCD_RESUMING_STR_EN "Resuming, please wait..."
#define LCD_RESUMING_STR_TC "恢复中，请等待..."

#define LCD_STOPPING_STR_CN "停止中，请等待..."
#define LCD_STOPPING_STR_EN "Stopping, please wait..."
#define LCD_STOPPING_STR_TC "停止中，请等待..."

#define LCD_HOME_POS_CAL_STR_CN "移动中，请等待..."
#define LCD_HOME_POS_CAL_STR_EN "Moving, please wait..."
#define LCD_HOME_POS_CAL_STR_TC "移动中，请等待..."

#define LCD_EXT_BED_POS_SET_STR_CN "移动中，请等待..."
#define LCD_EXT_BED_POS_SET_STR_EN "Moving, please wait..."
#define LCD_EXT_BED_POS_SET_STR_TC "移动中，请等待..."

#define LCD_BED_LEVEL_START_STR_CN "移动中，请等待..."
#define LCD_BED_LEVEL_START_STR_EN "Moving, please wait..."
#define LCD_BED_LEVEL_START_STR_TC "移动中，请等待..."

#define LCD_BED_LEVEL_END_STR_CN "移动中，请等待..."
#define LCD_BED_LEVEL_END_STR_EN "Moving, please wait..."
#define LCD_BED_LEVEL_END_STR_TC "移动中，请等待..."

#define LCD_BED_LEVELING_STR_CN "调平中，请等待..."
#define LCD_BED_LEVELING_STR_EN "Leveling, please wait..."
#define LCD_BED_LEVELING_STR_TC "调平中，请等待..."

#define LCD_E0_TO_DATUM_POS_STR_CN "E1正在移动到基准点"
#define LCD_E0_TO_DATUM_POS_STR_EN "E1 is moving to the datum point"
#define LCD_E0_TO_DATUM_POS_STR_TC "E1正在移动到基准点"

#define LCD_E1_TO_DATUM_POS_STR_CN "E2正在移动到基准点"
#define LCD_E1_TO_DATUM_POS_STR_EN "E2 is moving to the datum point"
#define LCD_E1_TO_DATUM_POS_STR_TC "E2正在移动到基准点"

#define LCD_LASER_TO_FOCUS_POS_STR_CN "E2正在移动到对焦点"
#define LCD_LASER_TO_FOCUS_POS_STR_EN "E2 is moving to the focus point"
#define LCD_LASER_TO_FOCUS_POS_STR_TC "E2正在移动到对焦点"

#define LCD_ENSURE_DATUM_POS_IN_BED_STR_CN "请确保基准点在平台上"
#define LCD_ENSURE_DATUM_POS_IN_BED_STR_EN "Make sure datum point is on platform"
#define LCD_ENSURE_DATUM_POS_IN_BED_STR_TC "请确保基准点在平台上"

#define LCD_LET_E0E1_MOVE_TO_DATUM_POS_STR_CN "让E1、E2分别靠近基准点"
#define LCD_LET_E0E1_MOVE_TO_DATUM_POS_STR_EN "Let E1 and E2 approach datum point"
#define LCD_LET_E0E1_MOVE_TO_DATUM_POS_STR_TC "让E1、E2分别靠近基准点"

#define LCD_TIPS_E0_Z_CAL_0_STR_CN "请拿卡片校准E1喷头与平台间的间距。"
#define LCD_TIPS_E0_Z_CAL_0_STR_EN "Please take the card to calibrate"
#define LCD_TIPS_E0_Z_CAL_0_STR_TC "请拿卡片校准E1喷头与平台间的间距。"

#define LCD_TIPS_E0_Z_CAL_1_STR_CN "卡片刚好可以通过的间距。"
#define LCD_TIPS_E0_Z_CAL_1_STR_EN "E1 Z height.The card can pass."
#define LCD_TIPS_E0_Z_CAL_1_STR_TC "卡片刚好可以通过的间距。"

#define LCD_TIPS_E1_Z_CAL_0_STR_CN "请拿卡片校准E2喷头与平台间的间距。"
#define LCD_TIPS_E1_Z_CAL_0_STR_EN "Please take the card to calibrate"
#define LCD_TIPS_E1_Z_CAL_0_STR_TC "请拿卡片校准E1喷头与平台间的间距。"

#define LCD_TIPS_E1_Z_CAL_1_STR_CN "卡片刚好可以通过的间距。"
#define LCD_TIPS_E1_Z_CAL_1_STR_EN "E2 Z height.The card can pass."
#define LCD_TIPS_E1_Z_CAL_1_STR_TC "卡片刚好可以通过的间距。"

#define LCD_TIPS_E0E1_COOR_CAL_0_STR_CN "请打印校准模型，设置喷头（E1与E2）"
#define LCD_TIPS_E0E1_COOR_CAL_0_STR_EN "Please print the model and set the"
#define LCD_TIPS_E0E1_COOR_CAL_0_STR_TC "请打印校准模型，设置喷头（E1与E2）"

#define LCD_TIPS_E0E1_COOR_CAL_1_STR_CN "的XY偏移量，使得双色模型对齐"
#define LCD_TIPS_E0E1_COOR_CAL_1_STR_EN "XY offset of the nozzle (E1 and E2)."
#define LCD_TIPS_E0E1_COOR_CAL_1_STR_TC "的XY偏移量，使得双色模型对齐"

#define LCD_ERR_MIN_TEMP_STR_CN "无法检测温度"
#define LCD_ERR_MIN_TEMP_STR_EN "Unable to detect nozzle temp"
#define LCD_ERR_MIN_TEMP_STR_TC "無法檢測溫度"

#define LCD_ERR_MAX_TEMP_STR_CN "温度超出限制"
#define LCD_ERR_MAX_TEMP_STR_EN "Max Nozzle Temp Exceeded"
#define LCD_ERR_MAX_TEMP_STR_TC "溫度超出限制"

#define LCD_ERR_MAX_TEMP_BED_STR_CN "超出热床最大温度限制"
#define LCD_ERR_MAX_TEMP_BED_STR_EN "Max Bed Temp Exceeded"
#define LCD_ERR_MAX_TEMP_BED_STR_TC "超出熱床最大溫度限制"

#define LCD_ERR_HEAT_FAIL_STR_CN "加热失败"
#define LCD_ERR_HEAT_FAIL_STR_EN "Heating Failure"
#define LCD_ERR_HEAT_FAIL_STR_TC "加熱失敗"

#define LCD_ERR_X_MIN_LIMIT_STR_CN "超出X轴最小限位"
#define LCD_ERR_X_MIN_LIMIT_STR_EN "Min X-axis Limit Exceeded"
#define LCD_ERR_X_MIN_LIMIT_STR_TC "超出X軸最小限位"

#define LCD_ERR_Y_MIN_LIMIT_STR_CN "超出Y轴最小限位"
#define LCD_ERR_Y_MIN_LIMIT_STR_EN "Min Y-axis Limit Exceeded"
#define LCD_ERR_Y_MIN_LIMIT_STR_TC "超出Y軸最小限位"

#define LCD_ERR_Z_MIN_LIMIT_STR_CN "超出Z轴最小限位"
#define LCD_ERR_Z_MIN_LIMIT_STR_EN "Min Z-axis Limit Exceeded"
#define LCD_ERR_Z_MIN_LIMIT_STR_TC "超出Z軸最小限位"

#define LCD_ERR_X_MAX_LIMIT_STR_CN "超出X轴最大限位"
#define LCD_ERR_X_MAX_LIMIT_STR_EN "Max X-axis Limit Exceeded"
#define LCD_ERR_X_MAX_LIMIT_STR_TC "超出X軸最大限位"

#define LCD_ERR_Y_MAX_LIMIT_STR_CN "超出Y轴最大限位"
#define LCD_ERR_Y_MAX_LIMIT_STR_EN "Max Y-axis Limit Exceeded"
#define LCD_ERR_Y_MAX_LIMIT_STR_TC "超出Y轴最大限位"

#define LCD_ERR_Z_MAX_LIMIT_STR_CN "超出Z轴最大限位"
#define LCD_ERR_Z_MAX_LIMIT_STR_EN "Max Z-axis Limit Exceeded"
#define LCD_ERR_Z_MAX_LIMIT_STR_TC "超出Z軸最大限位"

#define LCD_ERR_IWDG_RESET_STR_CN "系统重启"
#define LCD_ERR_IWDG_RESET_STR_EN "System Reset"
#define LCD_ERR_IWDG_RESET_STR_TC "系統重啟"

#define LCD_ERR_FATFS_STR_CN "读取文件失败"
#define LCD_ERR_FATFS_STR_EN "Print File Read Failure"
#define LCD_ERR_FATFS_STR_TC "讀取文件失敗"

#define LCD_ERR_THERMISTOR_FALLS_STR_CN "喷嘴测温模块脱落"
#define LCD_ERR_THERMISTOR_FALLS_STR_EN "Nozzle thermistor falls"
#define LCD_ERR_THERMISTOR_FALLS_STR_TC "喷嘴测温模块脱落"


#ifdef __cplusplus
} //extern "C" {
#endif

#endif

