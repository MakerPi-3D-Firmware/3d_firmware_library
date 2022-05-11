#ifndef USER_TASK_H
#define USER_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

//接口定义值，不应随便改变
#define BackZeroValue                 0  //回零
#define DisableStepValue              1  //解锁电机
#define PreHeatPLAValue               2  //预热PLA
#define CoolDownValue                 3  //冷切
#define FeedFilamentValue             4  //进料
#define StopFeedFilamentValue         5  //退出进料
#define BackFilamentValue             6  //退料
#define StopBackFilamentValue         7  //退出退料
#define OpenSDCardValue               8  //打开SD卡
#define OpenDirValue                  9  //打开目录
#define BackLastDirValue             10  //返回上层目录
#define NextPageValue                11  //下一页
#define LastPageValue                12  //上一页
#define FilePrintValue               13  //选中文件确定打印
#define PausePrintValue              14  //暂停打印
#define ResumePrintValue             15  //继续打印
#define StopPrintValue               16  //停止打印
#define OpenBeep                     17  //打开蜂鸣器
#define CloseBeep                    18  //关闭蜂鸣器
#define SysErrValue                  19  //系统出错
#define PrintSetValue_M14            20  //M14机型打印设置界面的确定键
#define PreHeatABSValue              22  //预热ABS
#define MoveXYZValue                 23  //移动光轴确定键
#define ConfirmChangeFilamentValue   24  //中途换料确定键
#define ConfirmLoadFilamentValue     25  //中途换料确认进料
#define ConfirmChangeModelValue      26  //选择机型后确定
#define ConfirmChangePictureValue    27  //选择图片后确定
#define ConfirmChangeFunctionValue   28  //选择功能后确定
#define PrintSetValue_NotM14_Left    29  //非M14机型左方点击 打印设置界面的确定键
#define PrintSetValue_NotM14_Right   30  //非M14机型右方点击 打印设置界面的确定键
#define ConfirmPowerOffRecover       31  //断电续打确认
#define CancelPowerOffRecover        32  //断电续打取消
#define CalculateZMaxPos             33  //计算Z轴最大位置
#define MatCheckCalibrateValue       34  //断料检测模块校准
#define PauseToResumeNozzleTemp      35  //断料状态下去换料，先把暂停打印降低的温度恢复
#define StepTestValue                36  //电机测试
#define FanTestValue                 37  //风扇测试
#define HeatTestValue                38  //加热测试
#define RunMaxPos                    39  //最大行程
#define CalHeatTime                  40  //加熱計時
#define ConfirmChangelogoValue       48  //确定logo设置 
#define PreHeatBedValue              53  //预热热床
#define PrintSetValue_Cavity         54  //预热热床
#define StartCalZZero                55  //计算Z零点
#define CancelCalZZero               56  //计算Z零点
#define StartCalBedLevel             57  //校准Z平台
#define ResetEBValue                 58  //重置eb值
#define SwitchLaser                  59  //切换激光模式指令
#define LaserMoveZUp                 60  //移动激光头Z
#define IdexMoveX                    61  //idex移动X
#define HomePosCalibration           62  //零点校准
#define ExtBedPosSet                 63  //喷嘴热床位置设置
#define BedLevelStart                64  //喷嘴热床位置设置
#define BedLevelEnd                  65  //喷嘴热床位置设置
#define E0ToDatumPos                 66  //喷嘴热床位置设置
#define E1ToDatumPos                 67  //喷嘴热床位置设置
#define LaserFocusPos                68  //移动激光头对焦点
#define HomeE1PosCalibration         69  //零点校准
extern void respond_gui_task_loop(void);
extern void start_default_task_init(void);
extern void start_default_task_loop(void);
extern void gui_task_loop(void);
extern void print_task_loop(void);
extern void read_udisk_task_loop(void);
extern void ref_data_task_loop(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif // USER_TASK_H

