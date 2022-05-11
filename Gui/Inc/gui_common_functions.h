#ifndef GUI_COMMON_FUNCTIONS_H
#define GUI_COMMON_FUNCTIONS_H


#ifdef __cplusplus
extern "C" {
#endif

extern void maindisplayF(void);

extern void PictureSetting(void);

extern void ChangeFilamentStatus(void);
extern void ChangeFilamentConfirm(void);
extern void NotHaveMatToChangeFilament(void);

extern void DoorOpenWarning_StartPrint(void);
extern void DoorOpenWarningInfo_NotPrinting(void);
extern void DoorOpenWarningInfo_Printing(void);

extern void filescanF(void);



extern void MoveXYZ(void);

extern void NoUdiskF(void);

extern void PausePrintF(void);
extern void ResumePrintF(void);


extern void prepareF(void);

extern void printconfirmF(void);

extern void printfinishF(void);

extern void PrintSet_M14(void);
extern void PrintSet_NotM14_Right(void);
extern void PrintSet_Cavity(void);
extern void SetLoadFilamentNozzleTemp(void);
extern void SetUnLoadFilamentNozzleTemp(void);
extern void SetPowerOffRecoverNozzleTemp(void);
extern void SetPowerOffRecoverHotBedTemp(void);

extern void PrintSet_NotM14_Left_dual(void);
extern void SetLoadFilamentNozzleTemp_dual(void);
extern void SetUnLoadFilamentNozzleTemp_dual(void);
extern void SetPowerOffRecoverNozzleTemp_dual(void);

extern void MachineSetting(void);

extern void settingF(void);

extern void statusF(void);

extern void stopprintF(void);

extern void loadfilament0F_dual(void);
extern void loadfilament0F(void);
extern void loadfilament1F(void);
extern void loadfilament2F(void);

extern void unloadfilament0F_dual(void);
extern void unloadfilament0F(void);
extern void unloadfilament1F(void);
extern void unloadfilament2F(void);

extern void gui_p3_pro_setting(void);
extern void gui_k5_setting(void);

extern void waiting_for_stopping(const volatile menufunc_t display);
extern void waiting_for_resuming(const volatile menufunc_t display);
extern void waiting_for_pausing(const volatile menufunc_t display);
extern void waiting_for_xyz_moving(const volatile menufunc_t display);
extern void waiting_for_home_pos_cal(const volatile menufunc_t display);
extern void waiting_for_home_e1_pos_cal(const volatile menufunc_t display);
extern void waiting_for_ext_bed_pos_set(const volatile menufunc_t display);
extern void waiting_for_bed_level_start(const volatile menufunc_t display);
extern void waiting_for_bed_level_end(const volatile menufunc_t display);
extern void waiting_for_homing(const volatile menufunc_t display);
extern void waiting_for_bed_leveling(const volatile menufunc_t display);
extern void waiting_for_e0_to_datum_pos(const volatile menufunc_t display);
extern void waiting_for_e1_to_datum_pos(const volatile menufunc_t display);
extern void waiting_for_laser_to_focus_pos(const volatile menufunc_t display);


void tips_for_e0_cal(const volatile menufunc_t pre_display, const volatile menufunc_t next_display);
void tips_for_e1_cal(const volatile menufunc_t pre_display, const volatile menufunc_t next_display);
void tips_for_e0e1_coor_cal(const volatile menufunc_t pre_display, const volatile menufunc_t next_display);

extern void gui_p3_pro_idex_model_select(void);
extern void gui_p3_pro_setting_model_set(void);

extern void gui_p3_pro_mix_model_select(void);

extern void gui_p3_pro_setting_home_pos(void);
extern void gui_p3_pro_setting_ext1_bed(void);
extern void gui_p3_pro_setting_prepare_bedlevel(void);
extern void gui_p3_pro_setting_bedlevel_finish(void);
extern void gui_p3_pro_laser_focus_setting(void);

extern void gui_pic_43_black_set(void);
extern void gui_pic_43_blue_set(void);
extern void gui_pic_43_set(void);


extern void gui_step_disable_info(void);
extern void gui_main_disp_single(void);
extern void gui_main_disp_single_cavity(void);
extern int gui_main_disp(void);

extern void gui_print_set_singel_left(void);

extern void gui_setting_pow_rec(void);

extern void gui_cal_z_height(void);

extern void gui_pow_rec_entry(void);
extern void gui_pow_rec_single(void);
extern void gui_pow_rec_single_dual(void);
extern void gui_pow_rec_ready(void);
int gui_print_disp_single_entry(int status, bool have_chg_filament);

void gui_filament_not_have_entry(void);
void gui_filament_resume_and_stop(void);

#define PIC_INDEX_FILAMENT_NOT_HAVE_ENTRY 62
#define PIC_INDEX_FILAMENT_CHG_AND_STOP 60
#define PIC_INDEX_FILAMENT_HEAT 64
#define PIC_INDEX_FILAMENT_RESUME_AND_STOP 61

#define PIC_INDEX_MAIN_SINGLE 1

#define PIC_INDEX_MAIN_SINGLE_CAVITY 106
#define PIC_INDEX_SET_SINGLE_LEFT 18
#define PIC_INDEX_STEP_DISABLE 63
#define PIC_INDEX_LOAD_UNLOAD_HEAT 8

#define PIC_INDEX_SETTING_POW_REC 46
#define PIC_INDEX_SETTING_POW_REC_NOT_TOUCH 188

#define PIC_INDEX_POW_REC_ENTRY 45
#define PIC_INDEX_POW_REC_SINGLE 49
#define PIC_INDEX_POW_REC_DUAL 187

#define PIC_INDEX_CAL_Z_HEIGHT 47
#define PIC_INDEX_CAL_Z_HEIGHT_FINISH 48

#define PIC_INDEX_PRINT_PAUSE_BTN 2
#define PIC_INDEX_PRINT_RESUME_BTN 3

#ifdef __cplusplus
} //extern "C" {
#endif

#endif


