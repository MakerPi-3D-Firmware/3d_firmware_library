#include "user_common_cpp.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------创建SysConfig.txt----------
// SysConfig.txt存储的信息不能超出512字节，因为读取的时候只读取512字节
// CF0:V1.0 //Boorloader版本
// CF1:V3.0.0 //当前版本
// CF2:20160405  //日期
// CF3:0     //是否已配置机器 1:YES-已设置过机器，不显示配置界面 0:NO-还没设置过机器，显示配置界面
// CF4:0     //机型 0:M14 1:M2030 2:M2041 3:M2048 4:M3145 5:M4141 6:M4040 7:M4141S 8:AMP410W 9:M14R03 10:M2030HY 11:M14S 12:M3145S
// CF5:1     //界面 1:中文图片  2：日文图片
// CF6:0     //断电续打功能 1:打开  0:关闭
// CF7:0     //Z轴最大位置
// CF8:0     //混色功能 1:打开  0:关闭
// CF9:0     //断料检测功能 1:打开  0:关闭
// CF10:0.5  //断料检测模块电压校准值,空载默认值设为0.5V
// CF11:0    //堵料检测功能 1:打开  0:关闭
// CF12:0    //自动调平 Z在xLeft  ，yFront   高度
// CF13:0    //自动调平 Z在xRight ，yFront   高度
// CF14:0    //自动调平 Z在xLeft  ，yBack    高度
// CF15:0    //自动调平 Z在xRight ，yBack    高度
// CF16:0    //自动调平 Z在xMiddle，yMiddle  高度
// CF17:0    //温度调节系数
// CF18:0    //开启自动调平 1:打开  0:关闭
// CF19:0    //开启软硬料   1:打开  0:关闭
// CF20:0    //开启logo     1:打开  0:关闭
// CF21:0    //Logo id
// CF22:0    //定制机型

#define SYS_CONFIG_FILE                          ((TCHAR *)_T("1:/SysConfig.txt"))
#define SYS_CONFIG_CF00_BOOT_VERSION                  ((char *)"CF0:")
#define SYS_CONFIG_CF01_APP2_VERSION                  ((char *)"CF1:")
#define SYS_CONFIG_CF02_DATE                          ((char *)"CF2:")
#define SYS_CONFIG_CF03_MACHINE_SETTING_MASK          ((char *)"CF3:")
#define SYS_CONFIG_CF04_MODEL_ID                      ((char *)"CF4:")
#define SYS_CONFIG_CF05_PICTURE_ID                    ((char *)"CF5:")
#define SYS_CONFIG_CF06_POWEROFF_RECOVERY_MASK        ((char *)"CF6:")
#define SYS_CONFIG_CF07_Z_MAX_POS_VALUE               ((char *)"CF7:")
#define SYS_CONFIG_CF08_COLOR_MIXING_MASK             ((char *)"CF8:")
#define SYS_CONFIG_CF09_MAT_CHECK_MASK                ((char *)"CF9:")
#define SYS_CONFIG_CF10_MAT_CHECK_VOL_VALUE           ((char *)"CF10:")
#define SYS_CONFIG_CF11_BLOCK_DETECT_MASK             ((char *)"CF11:")
#define SYS_CONFIG_CF12_AUTO_BED_LEVEL_Z_LF           ((char *)"CF12:")
#define SYS_CONFIG_CF13_AUTO_BED_LEVEL_Z_RF           ((char *)"CF13:")
#define SYS_CONFIG_CF14_AUTO_BED_LEVEL_Z_LB           ((char *)"CF14:")
#define SYS_CONFIG_CF15_AUTO_BED_LEVEL_Z_RB           ((char *)"CF15:")
#define SYS_CONFIG_CF16_AUTO_BED_LEVEL_Z_MIDDLE       ((char *)"CF16:")
#define SYS_CONFIG_CF17_PID_OUTPUT_FACTOR             ((char *)"CF17:")
#define SYS_CONFIG_CF18_AUTO_BED_LEVEL_MASK           ((char *)"CF18:")
#define SYS_CONFIG_CF19_IS_SOFTFILAMENT               ((char *)"CF19:")
#define SYS_CONFIG_CF20_LOGOorNO                      ((char *)"CF20:")
#define SYS_CONFIG_CF21_LOGOID                        ((char *)"CF21:")
#define SYS_CONFIG_CF22_CUSTOM_MODEL_ID               ((char *)"CF22:")
#define SYS_CONFIG_CF23_CUSTOM_BEZZERSOUND            ((char *)"CF23:")
#define SYS_CONFIG_CF24_Z_OFFSET_ZERO_VALUE           ((char *)"CF24:")
#define SYS_CONFIG_CF25_V5_EXTRUDER_MASK              ((char *)"CF25:")
#define SYS_CONFIG_CF26_ENABLE_CAVITY_TEMP            ((char *)"CF26:")
#define SYS_CONFIG_CF27_TYPE_OF_THERMISTOR            ((char *)"CF27:")
#define SYS_CONFIG_CF28_ENABLE_HIGH_TEMP              ((char *)"CF28:")
#define SYS_CONFIG_CF29_UI_NUMBER                     ((char *)"CF29:")
#define SYS_CONFIG_CF30_IS_2GT                        ((char *)"CF30:")
#define SYS_CONFIG_CF31_IS_MECHANISM_LEVEL            ((char *)"CF31:")
#define SYS_CONFIG_CF32_IS_LASER                      ((char *)"CF32:")
#define SYS_CONFIG_CF33_IS_LASER_MODE                 ((char *)"CF33:")
#define SYS_CONFIG_CF34              ((char *)"CF34:")
#define SYS_CONFIG_CF35              ((char *)"CF35:")
#define SYS_CONFIG_CF36              ((char *)"CF36:")
#define SYS_CONFIG_CF37              ((char *)"CF37:")
#define SYS_CONFIG_CF38              ((char *)"CF38:")
#define SYS_CONFIG_CF39              ((char *)"CF39:")
#define SYS_CONFIG_CF40              ((char *)"CF40:")

static SysConfigOperation sysConfigOperation;

void sysconfig_init(void)
{
  SysConfig sysConfig;
  sysConfig.init();
}

void SaveSelectedModel(void) //保存已选择的机型
{
  sysConfigOperation.ChangeModelInfo();
  sysConfigOperation.saveInfo(true);
}

void SaveSelectedPicture(void) //保存已选择的图片
{
  sysConfigOperation.ChangePictureInfo();
  sysConfigOperation.saveInfo(true);
}

void SaveSelectedFunction(void) //保存已选择的功能
{
  sysConfigOperation.ChangeFunctionInfo();
  sysConfigOperation.saveInfo(true);
}

void SaveSelectedlogo(void)
{
  sysConfigOperation.ChangelogoInfo();
  sysConfigOperation.saveInfo(true);
}

void SaveLaser(void) //保存soundValua；
{
  sysConfigOperation.readInfo();
  sysConfigOperation.ChangeLaser();
  sysConfigOperation.saveInfo(false);
}

static char *sysconfig_find_key_value(const char *key)
{
  char *value;                                                // 用于保存需要返回查找信息的位置
  value = strstr((char *)sys_data, key);                              // 返回key后面数据的指针位置

  if (value != NULL)                                                 // 判断是否查找到匹配信息
    value = value + strlen(key);                                     // 计算需要查找信息后面数据的首地址

  return value;                                                      // 返回数据首地址
}

static void sysconfig_get_uint8_t(char *str_index, uint8_t &value)
{
  static char *search_str;
  search_str = sysconfig_find_key_value(str_index);

  if (search_str != NULL)
  {
    value = (unsigned char)(*search_str - '0');
  }
}

static void sysconfig_get_float(char *str_index, float &value)
{
  static char *search_str;
  search_str = sysconfig_find_key_value(str_index);

  if (search_str != NULL)
  {
    value = (float)atof(search_str);
  }
}

static void sysconfig_get_int(char *str_index, int &value)
{
  static char *search_str;
  search_str = sysconfig_find_key_value(SYS_CONFIG_CF04_MODEL_ID);

  if (search_str != NULL)
  {
    value = atoi(search_str);
  }
}

static void sysconfig_get_str0(char *str_index, char *value)
{
  static char *StrInfoHeadPtr;
  static char *StrInfoEndPtr;
  int length = 0;
  StrInfoHeadPtr = sysconfig_find_key_value(str_index);

  /**********判断是否有匹配数据************/
  if (StrInfoHeadPtr != NULL)
  {
    /***********有匹配数据情况************/
    StrInfoEndPtr = strchr(StrInfoHeadPtr, '\r');
    length = StrInfoEndPtr - StrInfoHeadPtr;
    (void)strncpy((char *)value, StrInfoHeadPtr, (unsigned int)length);
    value[length] = 0;
    char *Ptr = strstr((char *)value, " ");

    if (Ptr != NULL)
    {
      *Ptr = 0;
    }
  }
}

static void sysconfig_set_char(char *str_index, char value)
{
  char *Ptr;
  Ptr = strstr((char *)sys_data, str_index);
  Ptr = Ptr + strlen(str_index);
  *Ptr = (char)value + '0';
}

static void sysconfig_set_int(char *str_index, int value)
{
  char *Ptr;
  char StrBuf1[10];
  char StrBuf2[10];//机型字符串存储空间  共10字节

  //机型字符串存储空间初始化为空白符
  for (int i = 0; i < 10; i++)
  {
    StrBuf2[i] = ' ';
  }

  (void)snprintf(StrBuf1, sizeof(StrBuf1), "%s%d", str_index, value);
  (void)memcpy(StrBuf2, StrBuf1, strlen(StrBuf1));
  Ptr = strstr((char *)sys_data, str_index);
  (void)memcpy(Ptr, StrBuf2, sizeof(StrBuf2));
}


#ifdef __cplusplus
} //extern "C"
#endif

SysConfigOperation::SysConfigOperation() {}

void SysConfigOperation::readInfo(void)
{
  sys_read_info(SYS_CONFIG_FILE);                                                  //从SD卡读取配置信息
}

void SysConfigOperation::saveInfo(bool isFunction)
{
  if (isFunction)
    changeMachineSettingMark();                            // 设备是否已经配置的标志位在 InfoBuf里的位置 置一

  sys_write_info(SYS_CONFIG_FILE);                                         // 将修改后的  InfoBuf 数组数据写进SD卡
}

void SysConfigOperation::changeMachineSettingMark(void)
{
  char *Ptr;
  Ptr = strstr((char *)sys_data, SYS_CONFIG_CF03_MACHINE_SETTING_MASK); //CF3:0           1--YES   0--NO
  Ptr = Ptr + strlen(SYS_CONFIG_CF03_MACHINE_SETTING_MASK);               //计算设备是否已经配置情况在数组InfoBuf里的位置
  *Ptr = '1'; //给CF3:后面的数据置一，表示已配置机器，重新上电不再显示配置界面
}

void SysConfigOperation::ChangeModelInfo(void)
{
  sysconfig_set_int(SYS_CONFIG_CF04_MODEL_ID, (int)ccm_param.t_sys_data_current.model_id);
}

void SysConfigOperation::ChangePictureInfo(void)
{
  sysconfig_set_char(SYS_CONFIG_CF05_PICTURE_ID, (char)ccm_param.t_sys_data_current.pic_id);
}

void SysConfigOperation::ChangeFunctionInfo(void)
{
  sysconfig_set_char(SYS_CONFIG_CF08_COLOR_MIXING_MASK, (char)ccm_param.t_sys_data_current.enable_color_mixing);
  sysconfig_set_char(SYS_CONFIG_CF06_POWEROFF_RECOVERY_MASK, (char)ccm_param.t_sys_data_current.enable_powerOff_recovery);
  sysconfig_set_char(SYS_CONFIG_CF09_MAT_CHECK_MASK, (char)ccm_param.t_sys_data_current.enable_material_check);
  sysconfig_set_char(SYS_CONFIG_CF11_BLOCK_DETECT_MASK, (char)ccm_param.t_sys_data_current.enable_block_detect);
  sysconfig_set_char(SYS_CONFIG_CF18_AUTO_BED_LEVEL_MASK, (char)ccm_param.t_sys_data_current.enable_bed_level);
  sysconfig_set_char(SYS_CONFIG_CF19_IS_SOFTFILAMENT, (char)ccm_param.t_sys_data_current.enable_soft_filament);
  sysconfig_set_char(SYS_CONFIG_CF25_V5_EXTRUDER_MASK, (char)ccm_param.t_sys_data_current.enable_v5_extruder);
}

void SysConfigOperation::ChangelogoInfo(void)
{
  sysconfig_set_char(SYS_CONFIG_CF21_LOGOID, (char)ccm_param.t_sys_data_current.logo_id);
}

void SysConfigOperation::ChangeLaser(void)
{
  char *Ptr;
  Ptr = strstr((char *)sys_data, SYS_CONFIG_CF32_IS_LASER); //CF23:11表示按键和警报音关闭；22表示按键和警报音开启
  Ptr = Ptr + strlen(SYS_CONFIG_CF32_IS_LASER);
  *Ptr = (char)(ccm_param.t_sys_data_current.IsLaser + '0');//保存的值不能是字符‘0’
}

SysConfig::SysConfig()
{
  /*************机器信息配置***********/
  memset((char *)&ccm_param.t_sys_data_current, 0, sizeof(ccm_param.t_sys_data_current));
  ccm_param.t_sys_data_current.model_id = DEFAULT_MODEL;                                           // 默认配置，机型的选择 （在machinecustom.h文件中修改）
  ccm_param.t_sys_data_current.pic_id = PICTURE_IS_CHINESE;                                   // 图片语言选择 ：中文 、日文、英文、韩文、俄文（宏定义在machinecustom.h文件）
  ccm_param.t_sys_data_current.enable_color_mixing = DEFAULT_COLORMIXING;                         // 是否打开混色功能（在machinecustom.h文件中修改）
  ccm_param.t_sys_data_current.enable_powerOff_recovery = DEFAULT_POWEROFFRECOVERY;               // 是否打开断电续打功能（在machinecustom.h文件中修改）
  ccm_param.t_sys_data_current.enable_material_check = DEFAULT_MATCHECK;                          // 是否打开断料检测功能（在machinecustom.h文件中修改）
  ccm_param.t_sys_data_current.pid_output_factor = 1.0f;
  ccm_param.t_sys_data_current.ui_number = LCD_43_UI_TYPE_NONE;
}

void SysConfig::init(void)
{
  sys_read_info(SYS_CONFIG_FILE);                                                  //从SD卡读取配置信息
  explainInfo();                                                     //解析SD卡中的配置信息

  if (!ccm_param.t_sys_data_current.have_set_machine) // 没有设置机器时，混色模式打开，测试固件可以测试B轴电机
  {
    ccm_param.t_sys_data_current.enable_color_mixing = true;
  }

  if (!ccm_param.t_sys_data_current.enable_LOGO_interface)
    ccm_param.t_sys_data_current.logo_id = 0;
}

void SysConfig::explainInfo(void)
{
  // CF0 解析保存固件Boorloader版本
  sysconfig_get_str0(SYS_CONFIG_CF00_BOOT_VERSION, (char *)ccm_param.t_gui.boot_ver_str);
  // CF1 解析保存固件当前版本信息
  sysconfig_get_str0(SYS_CONFIG_CF01_APP2_VERSION, (char *)ccm_param.t_gui.app2_ver_str);
  // CF2 解析保存固件建立日期
  sysconfig_get_str0(SYS_CONFIG_CF02_DATE, (char *)ccm_param.t_gui.build_date_str);
  // CF3 解析是否已配置机器 1:YES-已设置过机器，不显示配置界面 0:NO-还没设置过机器，显示配置界面
  sysconfig_get_uint8_t(SYS_CONFIG_CF03_MACHINE_SETTING_MASK, (uint8_t &)ccm_param.t_sys_data_current.have_set_machine);
  // CF4 解析机型 0:M14 1:M2030 2:M2041 3:M2048 4:M3145 5:M4141 6:M4040 7:M4141S 8:AMP410W 9:M14R03 10:M2030HY 11:M14S 12:M3145S
  sysconfig_get_int(SYS_CONFIG_CF04_MODEL_ID, (int &)ccm_param.t_sys_data_current.model_id);
  // CF5 解析界面语种  1:中文图片  2：日文图片
  sysconfig_get_uint8_t(SYS_CONFIG_CF05_PICTURE_ID, (uint8_t &)ccm_param.t_sys_data_current.pic_id);
  // CF6 解析断电续打功能是否开启
  sysconfig_get_uint8_t(SYS_CONFIG_CF06_POWEROFF_RECOVERY_MASK, (uint8_t &)ccm_param.t_sys_data_current.enable_powerOff_recovery);
  // CF8 解析是否打开混色功能
  sysconfig_get_uint8_t(SYS_CONFIG_CF08_COLOR_MIXING_MASK, (uint8_t &)ccm_param.t_sys_data_current.enable_color_mixing);
  // CF9 解析是否打开断料检测功能
  sysconfig_get_uint8_t(SYS_CONFIG_CF09_MAT_CHECK_MASK, (uint8_t &)ccm_param.t_sys_data_current.enable_material_check);
  // CF10 解析断料检测模块电压校准值,空载默认值设为0.5V
  sysconfig_get_float(SYS_CONFIG_CF10_MAT_CHECK_VOL_VALUE, (float &)ccm_param.t_sys_data_current.material_chk_vol_value);
  // CF11 解析是否打开块检测功能
  sysconfig_get_uint8_t(SYS_CONFIG_CF11_BLOCK_DETECT_MASK, (uint8_t &)ccm_param.t_sys_data_current.enable_block_detect);
  // CF12
  sysconfig_get_float(SYS_CONFIG_CF12_AUTO_BED_LEVEL_Z_LF, (float &)ccm_param.t_sys_data_current.bed_level_z_at_left_front);
  // CF13
  sysconfig_get_float(SYS_CONFIG_CF13_AUTO_BED_LEVEL_Z_RF, (float &)ccm_param.t_sys_data_current.bed_level_z_at_right_front);
  // CF14
  sysconfig_get_float(SYS_CONFIG_CF14_AUTO_BED_LEVEL_Z_LB, (float &)ccm_param.t_sys_data_current.bed_level_z_at_left_back);
  // CF15
  sysconfig_get_float(SYS_CONFIG_CF15_AUTO_BED_LEVEL_Z_RB, (float &)ccm_param.t_sys_data_current.bed_level_z_at_right_back);
  // CF16
  sysconfig_get_float(SYS_CONFIG_CF16_AUTO_BED_LEVEL_Z_MIDDLE, (float &)ccm_param.t_sys_data_current.bed_level_z_at_middle);
  // CF17
  sysconfig_get_float(SYS_CONFIG_CF17_PID_OUTPUT_FACTOR, (float &)ccm_param.t_sys_data_current.pid_output_factor);

  if (ccm_param.t_sys_data_current.pid_output_factor > (1.0f / 0.85f + 0.1f) || ccm_param.t_sys_data_current.pid_output_factor <= 0.0f) //1/0.85+0.1时才不会更改最大系数1/0.85
    ccm_param.t_sys_data_current.pid_output_factor = 1.0f;

  // CF18
  sysconfig_get_uint8_t(SYS_CONFIG_CF18_AUTO_BED_LEVEL_MASK, (uint8_t &)ccm_param.t_sys_data_current.enable_bed_level);
  // CF19 解析是否使用了软料
  sysconfig_get_uint8_t(SYS_CONFIG_CF19_IS_SOFTFILAMENT, (uint8_t &)ccm_param.t_sys_data_current.enable_soft_filament);
  // CF20 解析是否开启开机LOGO
  sysconfig_get_uint8_t(SYS_CONFIG_CF20_LOGOorNO, (uint8_t &)ccm_param.t_sys_data_current.enable_LOGO_interface);
  // CF21 解析SD卡中保存的LOGOid
  sysconfig_get_uint8_t(SYS_CONFIG_CF21_LOGOID, (uint8_t &)ccm_param.t_sys_data_current.logo_id);
  // CF22
  sysconfig_get_int(SYS_CONFIG_CF22_CUSTOM_MODEL_ID, (int &)ccm_param.t_sys_data_current.custom_model_id);
  // CF24
  sysconfig_get_float(SYS_CONFIG_CF24_Z_OFFSET_ZERO_VALUE, (float &)ccm_param.t_sys_data_current.z_offset_value);
  // CF25
  sysconfig_get_uint8_t(SYS_CONFIG_CF25_V5_EXTRUDER_MASK, (uint8_t &)ccm_param.t_sys_data_current.enable_v5_extruder);
  sysconfig_get_uint8_t(SYS_CONFIG_CF26_ENABLE_CAVITY_TEMP, (uint8_t &)ccm_param.t_sys_data_current.enable_cavity_temp);
  ccm_param.t_sys.enable_cavity_temp = ccm_param.t_sys_data_current.enable_cavity_temp;
  sysconfig_get_uint8_t(SYS_CONFIG_CF27_TYPE_OF_THERMISTOR, (uint8_t &)ccm_param.t_sys_data_current.enable_type_of_thermistor);
  sysconfig_get_uint8_t(SYS_CONFIG_CF28_ENABLE_HIGH_TEMP, (uint8_t &)ccm_param.t_sys_data_current.enable_high_temp);
  sysconfig_get_uint8_t(SYS_CONFIG_CF29_UI_NUMBER, (uint8_t &)ccm_param.t_sys_data_current.ui_number);
  sysconfig_get_uint8_t(SYS_CONFIG_CF30_IS_2GT, (uint8_t &)ccm_param.t_sys_data_current.is_2GT);
  sysconfig_get_uint8_t(SYS_CONFIG_CF31_IS_MECHANISM_LEVEL, (uint8_t &)ccm_param.t_sys_data_current.IsMechanismLevel);
  sysconfig_get_uint8_t(SYS_CONFIG_CF32_IS_LASER, (uint8_t &)ccm_param.t_sys_data_current.IsLaser);
  sysconfig_get_uint8_t(SYS_CONFIG_CF33_IS_LASER_MODE, (uint8_t &)ccm_param.t_sys_data_current.IsLaserMode);
  sysconfig_get_uint8_t(SYS_CONFIG_CF34, (uint8_t &)ccm_param.t_sys_data_current.lcd_type);
  ccm_param.t_sys.lcd_type = ccm_param.t_sys_data_current.lcd_type;
  sysconfig_get_uint8_t(SYS_CONFIG_CF35, (uint8_t &)ccm_param.t_sys_data_current.cf35);
  sysconfig_get_uint8_t(SYS_CONFIG_CF36, (uint8_t &)ccm_param.t_sys_data_current.cf36);
  sysconfig_get_uint8_t(SYS_CONFIG_CF37, (uint8_t &)ccm_param.t_sys_data_current.cf37);
  sysconfig_get_uint8_t(SYS_CONFIG_CF38, (uint8_t &)ccm_param.t_sys_data_current.cf38);
  sysconfig_get_uint8_t(SYS_CONFIG_CF39, (uint8_t &)ccm_param.t_sys_data_current.cf39);
  sysconfig_get_uint8_t(SYS_CONFIG_CF40, (uint8_t &)ccm_param.t_sys_data_current.cf40);
  getModelStr();                                                     // 通过从SD卡读取的机型信息编号查找将机型信息编号写进机型数组中
  getStatusInfoStr();                                                // 将从SD卡读取的功能和版本信息分别写进功能和版本数组中
}

void SysConfig::getModelStr(void)
{
  (void)strcpy((char *)ccm_param.t_gui.model_str, model_name_table[ccm_param.t_sys_data_current.model_id]);                 // 通过从SD卡读取的机型信息编号查找将机型信息编号写进 modelStr 数组中
}

void SysConfig::getStatusInfoStr(void)
{
  (void)strcpy((char *)ccm_param.t_gui.function_str, "Base"); //Base+CM+POR+MC+LED 基础+混色+断电续打+断料检测+LED照明

  if (ccm_param.t_sys_data_current.enable_color_mixing)
    (void)strcat((char *)ccm_param.t_gui.function_str, "+CM");                                // 混色功能

  if (ccm_param.t_sys_data_current.enable_powerOff_recovery)
    (void)strcat((char *)ccm_param.t_gui.function_str, "+POR");                               // 断电续打功能

  if (ccm_param.t_sys_data_current.enable_material_check)
    (void)strcat((char *)ccm_param.t_gui.function_str, "+MC");                                // 断料检测功能

  if (ccm_param.t_sys_data_current.enable_block_detect)
    (void)strcat((char *)ccm_param.t_gui.function_str, "+BD");

  if (BED_LEVEL_PRESSURE_SENSOR == ccm_param.t_sys_data_current.enable_bed_level)
    (void)strcat((char *)ccm_param.t_gui.function_str, "+BL3");

  if (ccm_param.t_sys_data_current.enable_soft_filament)
    (void)strcat((char *)ccm_param.t_gui.function_str, "+SF");                                 //使用了软料齿轮

  if (1 == ccm_param.t_sys_data_current.enable_v5_extruder)
    (void)strcat((char *)ccm_param.t_gui.function_str, "+V5");                                 //使用了软料齿轮

  if (2 == ccm_param.t_sys_data_current.enable_v5_extruder)
    (void)strcat((char *)ccm_param.t_gui.function_str, "+V5.1");                                 //使用了软料齿轮

  /******************版本信息******************/
  (void)snprintf((char *)ccm_param.t_gui.version_str,
                 sizeof(ccm_param.t_gui.version_str),
                 "D%s-%s-%s",
                 ccm_param.t_gui.build_date_str,
                 ccm_param.t_gui.app2_ver_str,
                 ccm_param.t_gui.boot_ver_str); //D20151110-V3.0.0-V2.0
}


