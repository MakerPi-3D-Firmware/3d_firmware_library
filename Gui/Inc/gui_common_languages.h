#ifndef GUI_COMMON_LANGUAGES_H
#define GUI_COMMON_LANGUAGES_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma  diag_suppress 870

#define LCD_HOMING_STR_CN "�����У���ȴ�..."
#define LCD_HOMING_STR_EN "Homing, please wait..."
#define LCD_HOMING_STR_TC "�����У���ȴ�..."

#define LCD_XYZ_MOVING_STR_CN "�ƶ��У���ȴ�..."
#define LCD_XYZ_MOVING_STR_EN "Moving, please wait..."
#define LCD_XYZ_MOVING_STR_TC "�ƶ��У���ȴ�..."

#define LCD_PAUSING_STR_CN "��ͣ�У���ȴ�..."
#define LCD_PAUSING_STR_EN "Pausing, please wait..."
#define LCD_PAUSING_STR_TC "��ͣ�У���ȴ�..."

#define LCD_RESUMING_STR_CN "�ָ��У���ȴ�..."
#define LCD_RESUMING_STR_EN "Resuming, please wait..."
#define LCD_RESUMING_STR_TC "�ָ��У���ȴ�..."

#define LCD_STOPPING_STR_CN "ֹͣ�У���ȴ�..."
#define LCD_STOPPING_STR_EN "Stopping, please wait..."
#define LCD_STOPPING_STR_TC "ֹͣ�У���ȴ�..."

#define LCD_HOME_POS_CAL_STR_CN "�ƶ��У���ȴ�..."
#define LCD_HOME_POS_CAL_STR_EN "Moving, please wait..."
#define LCD_HOME_POS_CAL_STR_TC "�ƶ��У���ȴ�..."

#define LCD_EXT_BED_POS_SET_STR_CN "�ƶ��У���ȴ�..."
#define LCD_EXT_BED_POS_SET_STR_EN "Moving, please wait..."
#define LCD_EXT_BED_POS_SET_STR_TC "�ƶ��У���ȴ�..."

#define LCD_BED_LEVEL_START_STR_CN "�ƶ��У���ȴ�..."
#define LCD_BED_LEVEL_START_STR_EN "Moving, please wait..."
#define LCD_BED_LEVEL_START_STR_TC "�ƶ��У���ȴ�..."

#define LCD_BED_LEVEL_END_STR_CN "�ƶ��У���ȴ�..."
#define LCD_BED_LEVEL_END_STR_EN "Moving, please wait..."
#define LCD_BED_LEVEL_END_STR_TC "�ƶ��У���ȴ�..."

#define LCD_BED_LEVELING_STR_CN "��ƽ�У���ȴ�..."
#define LCD_BED_LEVELING_STR_EN "Leveling, please wait..."
#define LCD_BED_LEVELING_STR_TC "��ƽ�У���ȴ�..."

#define LCD_E0_TO_DATUM_POS_STR_CN "E1�����ƶ�����׼��"
#define LCD_E0_TO_DATUM_POS_STR_EN "E1 is moving to the datum point"
#define LCD_E0_TO_DATUM_POS_STR_TC "E1�����ƶ�����׼��"

#define LCD_E1_TO_DATUM_POS_STR_CN "E2�����ƶ�����׼��"
#define LCD_E1_TO_DATUM_POS_STR_EN "E2 is moving to the datum point"
#define LCD_E1_TO_DATUM_POS_STR_TC "E2�����ƶ�����׼��"

#define LCD_LASER_TO_FOCUS_POS_STR_CN "E2�����ƶ����Խ���"
#define LCD_LASER_TO_FOCUS_POS_STR_EN "E2 is moving to the focus point"
#define LCD_LASER_TO_FOCUS_POS_STR_TC "E2�����ƶ����Խ���"

#define LCD_ENSURE_DATUM_POS_IN_BED_STR_CN "��ȷ����׼����ƽ̨��"
#define LCD_ENSURE_DATUM_POS_IN_BED_STR_EN "Make sure datum point is on platform"
#define LCD_ENSURE_DATUM_POS_IN_BED_STR_TC "��ȷ����׼����ƽ̨��"

#define LCD_LET_E0E1_MOVE_TO_DATUM_POS_STR_CN "��E1��E2�ֱ𿿽���׼��"
#define LCD_LET_E0E1_MOVE_TO_DATUM_POS_STR_EN "Let E1 and E2 approach datum point"
#define LCD_LET_E0E1_MOVE_TO_DATUM_POS_STR_TC "��E1��E2�ֱ𿿽���׼��"

#define LCD_TIPS_E0_Z_CAL_0_STR_CN "���ÿ�ƬУ׼E1��ͷ��ƽ̨��ļ�ࡣ"
#define LCD_TIPS_E0_Z_CAL_0_STR_EN "Please take the card to calibrate"
#define LCD_TIPS_E0_Z_CAL_0_STR_TC "���ÿ�ƬУ׼E1��ͷ��ƽ̨��ļ�ࡣ"

#define LCD_TIPS_E0_Z_CAL_1_STR_CN "��Ƭ�պÿ���ͨ���ļ�ࡣ"
#define LCD_TIPS_E0_Z_CAL_1_STR_EN "E1 Z height.The card can pass."
#define LCD_TIPS_E0_Z_CAL_1_STR_TC "��Ƭ�պÿ���ͨ���ļ�ࡣ"

#define LCD_TIPS_E1_Z_CAL_0_STR_CN "���ÿ�ƬУ׼E2��ͷ��ƽ̨��ļ�ࡣ"
#define LCD_TIPS_E1_Z_CAL_0_STR_EN "Please take the card to calibrate"
#define LCD_TIPS_E1_Z_CAL_0_STR_TC "���ÿ�ƬУ׼E1��ͷ��ƽ̨��ļ�ࡣ"

#define LCD_TIPS_E1_Z_CAL_1_STR_CN "��Ƭ�պÿ���ͨ���ļ�ࡣ"
#define LCD_TIPS_E1_Z_CAL_1_STR_EN "E2 Z height.The card can pass."
#define LCD_TIPS_E1_Z_CAL_1_STR_TC "��Ƭ�պÿ���ͨ���ļ�ࡣ"

#define LCD_TIPS_E0E1_COOR_CAL_0_STR_CN "���ӡУ׼ģ�ͣ�������ͷ��E1��E2��"
#define LCD_TIPS_E0E1_COOR_CAL_0_STR_EN "Please print the model and set the"
#define LCD_TIPS_E0E1_COOR_CAL_0_STR_TC "���ӡУ׼ģ�ͣ�������ͷ��E1��E2��"

#define LCD_TIPS_E0E1_COOR_CAL_1_STR_CN "��XYƫ������ʹ��˫ɫģ�Ͷ���"
#define LCD_TIPS_E0E1_COOR_CAL_1_STR_EN "XY offset of the nozzle (E1 and E2)."
#define LCD_TIPS_E0E1_COOR_CAL_1_STR_TC "��XYƫ������ʹ��˫ɫģ�Ͷ���"

#define LCD_ERR_MIN_TEMP_STR_CN "�޷�����¶�"
#define LCD_ERR_MIN_TEMP_STR_EN "Unable to detect nozzle temp"
#define LCD_ERR_MIN_TEMP_STR_TC "�o���z�y�ض�"

#define LCD_ERR_MAX_TEMP_STR_CN "�¶ȳ�������"
#define LCD_ERR_MAX_TEMP_STR_EN "Max Nozzle Temp Exceeded"
#define LCD_ERR_MAX_TEMP_STR_TC "�ضȳ�������"

#define LCD_ERR_MAX_TEMP_BED_STR_CN "�����ȴ�����¶�����"
#define LCD_ERR_MAX_TEMP_BED_STR_EN "Max Bed Temp Exceeded"
#define LCD_ERR_MAX_TEMP_BED_STR_TC "�����ᴲ���ض�����"

#define LCD_ERR_HEAT_FAIL_STR_CN "����ʧ��"
#define LCD_ERR_HEAT_FAIL_STR_EN "Heating Failure"
#define LCD_ERR_HEAT_FAIL_STR_TC "�ӟ�ʧ��"

#define LCD_ERR_X_MIN_LIMIT_STR_CN "����X����С��λ"
#define LCD_ERR_X_MIN_LIMIT_STR_EN "Min X-axis Limit Exceeded"
#define LCD_ERR_X_MIN_LIMIT_STR_TC "����X�S��С��λ"

#define LCD_ERR_Y_MIN_LIMIT_STR_CN "����Y����С��λ"
#define LCD_ERR_Y_MIN_LIMIT_STR_EN "Min Y-axis Limit Exceeded"
#define LCD_ERR_Y_MIN_LIMIT_STR_TC "����Y�S��С��λ"

#define LCD_ERR_Z_MIN_LIMIT_STR_CN "����Z����С��λ"
#define LCD_ERR_Z_MIN_LIMIT_STR_EN "Min Z-axis Limit Exceeded"
#define LCD_ERR_Z_MIN_LIMIT_STR_TC "����Z�S��С��λ"

#define LCD_ERR_X_MAX_LIMIT_STR_CN "����X�������λ"
#define LCD_ERR_X_MAX_LIMIT_STR_EN "Max X-axis Limit Exceeded"
#define LCD_ERR_X_MAX_LIMIT_STR_TC "����X�S�����λ"

#define LCD_ERR_Y_MAX_LIMIT_STR_CN "����Y�������λ"
#define LCD_ERR_Y_MAX_LIMIT_STR_EN "Max Y-axis Limit Exceeded"
#define LCD_ERR_Y_MAX_LIMIT_STR_TC "����Y�������λ"

#define LCD_ERR_Z_MAX_LIMIT_STR_CN "����Z�������λ"
#define LCD_ERR_Z_MAX_LIMIT_STR_EN "Max Z-axis Limit Exceeded"
#define LCD_ERR_Z_MAX_LIMIT_STR_TC "����Z�S�����λ"

#define LCD_ERR_IWDG_RESET_STR_CN "ϵͳ����"
#define LCD_ERR_IWDG_RESET_STR_EN "System Reset"
#define LCD_ERR_IWDG_RESET_STR_TC "ϵ�y�؆�"

#define LCD_ERR_FATFS_STR_CN "��ȡ�ļ�ʧ��"
#define LCD_ERR_FATFS_STR_EN "Print File Read Failure"
#define LCD_ERR_FATFS_STR_TC "�xȡ�ļ�ʧ��"

#define LCD_ERR_THERMISTOR_FALLS_STR_CN "�������ģ������"
#define LCD_ERR_THERMISTOR_FALLS_STR_EN "Nozzle thermistor falls"
#define LCD_ERR_THERMISTOR_FALLS_STR_TC "�������ģ������"


#ifdef __cplusplus
} //extern "C" {
#endif

#endif

