#ifndef GCODEBUFFERHANDLE_H
#define GCODEBUFFERHANDLE_H
#include <string.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t GetGcodeFromBuf(const char sd_char, const uint32_t file_position, bool isColorMix, volatile uint32_t &file_size, unsigned int &sd_buffersize);
extern void resetCmdBuf(void);
extern void setForceVerify(bool value);
/*20170807堵料修复*/
extern void set_leave_file_size(uint32_t FileSize);

#ifdef __cplusplus
} // extern "C" {
#endif


/**
 * 解析gcode指令
 */
class ParseGcodeBufHandle
{
public:
  ParseGcodeBufHandle();
  void setGcodeBuffer(const char *gcode_buffer);    ///< 设置gcode数组
  char *getGcodeBuf();                              ///< 获取设置gcode数组
  bool firstCodeSeen(const char code);                   ///< 查找字符并定位该字符位置
  bool codeSeen(const char code);                   ///< 查找字符并定位该字符位置
  bool codeSeenStr(const char *code);               ///< 查找字符串并定位该字符串位置
  float codeValue();                                ///< 获取查找字符或字符串后面所带的数值，整型
  long codeValueLong();                             ///< 获取查找字符或字符串后面所带的数值，浮点型
};

/**
 * Gcode指令获取
 */
class GcodeBufHandle
{
public:
  GcodeBufHandle();
  bool getFromGcode(const char sd_char, const uint32_t file_position, bool isColorMix, volatile uint32_t &file_size, unsigned int &sd_buffersize);      ///< 获取指令
  void setForceVerify(bool isForceVerify);                                                   ///< 设置是否强制校验
  void reset(void);                                                                          ///< 重置指令
  void setLeaveFileSize(uint32_t FileSize);                                                  ///< 设置file_size，堵料检测20170807
private:
  uint8_t getCheckSum(const char *buf);                                                      ///< 获取校验码
  void verifyCmdBufCount(int loopCnt, int &currCnt, uint32_t &retryCnt, uint32_t &ErrorCnt, volatile uint32_t &file_size, unsigned int &sd_buffersize); ///< 校验计数
  bool verifyCmdBuf(char *curr_cmd, bool force_verify, bool isColorMix, volatile uint32_t &file_size, unsigned int &sd_buffersize);                     ///< 校验指令
  void updateCmdBuf(char *curr_buf, bool isDecrypt);                                         ///< 更新指令
  bool verifyKnownCmd(bool isKnownCmd, bool isChkSumCorrect, volatile uint32_t &file_size, unsigned int &sd_buffersize);                                ///< 校验已知指令
};

extern ParseGcodeBufHandle parseGcodeBufHandle;
extern GcodeBufHandle gcodeBufHandle;
#endif // GCODEBUFFERHANDLE_H


