#ifndef SYSCONFIG_H
#define SYSCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

extern void sysconfig_init(void);

extern void SaveSelectedModel(void);
extern void SaveSelectedPicture(void);
extern void SaveSelectedFunction(void);
extern void SaveSelectedlogo(void);

#ifdef __cplusplus
} //extern "C"
#endif

class SysConfigOperation
{
public:
  SysConfigOperation();
  void readInfo(void);
  void ChangeModelInfo(void);
  void ChangePictureInfo(void);
  void ChangeFunctionInfo(void);
  void ChangelogoInfo(void);

  void ChangeLaser(void);

  void saveInfo(bool isFunction);
private:
  /*save sysconfig info Function*/
  void changeMachineSettingMark(void);
};

class SysConfig
{
public:
  SysConfig();
  void init(void);

private:
  /*get sysconfig info Function*/
  void explainInfo(void);
  void getModelStr(void);
  void getStatusInfoStr(void);

};

#endif // SYSCONFIG_H

