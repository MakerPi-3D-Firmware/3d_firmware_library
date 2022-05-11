#include "user_common_cpp.h"
#include "../../Grbl/Inc/gcodebufferhandle.h"

#ifdef __cplusplus
extern "C" {
#endif

static const int gcode_buffer_size = 96;                      /*!< gcode数组大小 */

// GcodeBufHandle variables
static uint32_t ForceChksum_RetryCnt;                         /*!< 无$强制校验重试计数 */
static uint32_t ForceChksum_ErrorCnt;                         /*!< 无$强制校验错误计数 */
static uint32_t check_unknown_cmd_retry_count;                /*!< 未知指令校验重试计数 */
static uint32_t check_unknown_cmd_error_count;                /*!< 未知指令校验错误计数 */
static uint32_t checksum_error_counts;                        /*!< 已知指令校验重试计数 */
static uint32_t checksum_retry_counts;                        /*!< 已知指令校验错误计数 */

static volatile uint32_t cmd_buf_pos;                         /*!< 当前指令位置 */
static volatile uint32_t leave_file_size;                     /*!< 剩余文件大小 */

static bool is_comment_mode;                                  /*!< 是否为注释 */
static uint8_t command_buffer_count;                          /*!< 指令数组计数 */
static bool is_force_verify;                                  /*!< 是否开启强制校验 */

void setForceVerify(bool value)
{
  gcodeBufHandle.setForceVerify(value);
}

void resetCmdBuf(void)
{
  gcodeBufHandle.reset();
}

void set_leave_file_size(uint32_t FileSize)
{
  gcodeBufHandle.setLeaveFileSize(FileSize);
}

uint8_t GetGcodeFromBuf(const char sd_char, const uint32_t file_position, bool isColorMix, volatile uint32_t &file_size, unsigned int &sd_buffersize)
{
  return gcodeBufHandle.getFromGcode(sd_char, file_position, isColorMix, file_size, sd_buffersize);
}


#ifdef __cplusplus
} //extern "C" {
#endif

// ParseGcodeBufHandle variables
static char GcodeBuffer[gcode_buffer_size] = {0};             /*!< gcode数组 */
static char *strchr_pointer;                                  /*!< just a pointer to find chars in the cmd string like X, Y, Z, E, etc */
ParseGcodeBufHandle parseGcodeBufHandle;

// GcodeBufHandle variables
GcodeBufHandle gcodeBufHandle;

/**
 *
 */
ParseGcodeBufHandle::ParseGcodeBufHandle()
{
  strchr_pointer = NULL;
  memset(GcodeBuffer, 0, gcode_buffer_size);
}

/**
 * [ParseGcodeBufHandle::codeValue description]
 * @return [description]
 */
float ParseGcodeBufHandle::codeValue()
{
  return strtof(strchr_pointer + 1, NULL);
}

/**
 * [ParseGcodeBufHandle::codeValueLong description]
 * @return [description]
 */
long ParseGcodeBufHandle::codeValueLong()
{
  return (strtol(strchr_pointer + 1, NULL, 10));
}

bool ParseGcodeBufHandle::firstCodeSeen(const char code)
{
  if (GcodeBuffer[0] == code)
  {
    strchr_pointer = strchr(GcodeBuffer, code);
    return true;
  }
  else
  {
    return false;
  }
}

bool ParseGcodeBufHandle::codeSeen(const char code)
{
  strchr_pointer = strchr(GcodeBuffer, code);

  if (strchr_pointer != NULL)
    return true;
  else
    return false;
}

bool ParseGcodeBufHandle::codeSeenStr(const char *code)
{
  char *strstr_pointer = strstr(GcodeBuffer, code);

  if (strstr_pointer != NULL)
    return true;
  else
    return false;
}

void ParseGcodeBufHandle::setGcodeBuffer(const char *gcode_buffer)
{
  memset(GcodeBuffer, 0, sizeof(char) * gcode_buffer_size); // 清空字符串
  size_t buf_len = (strlen(gcode_buffer) <= gcode_buffer_size ? strlen(gcode_buffer) : gcode_buffer_size); // 字符串长度
  strncpy(GcodeBuffer, gcode_buffer, buf_len); // 拷贝字符串
  //  USER_EchoLog("%s\r\n", GcodeBuffer);
  // 修复联机打印第一层没变化，cura生成的gcode多了空格，导致校准异常跳出该指令
  // 软件修复后，可删除该逻辑
  char *pStr = strstr(GcodeBuffer, " E ");

  if (NULL != pStr)
  {
    strcpy(pStr + 2, pStr + 3);
    buf_len--;
  }
}

char *ParseGcodeBufHandle::getGcodeBuf()
{
  return GcodeBuffer;
}

///////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 */
GcodeBufHandle::GcodeBufHandle()
{
  reset();
}

/**
 * [GcodeBufHandle::reset description]
 */
void GcodeBufHandle::reset(void)
{
  ForceChksum_RetryCnt = 0;
  ForceChksum_ErrorCnt = 0;
  check_unknown_cmd_retry_count = 0;
  check_unknown_cmd_error_count = 0;
  checksum_error_counts = 0;
  checksum_retry_counts = 0;
  cmd_buf_pos = 0;
  leave_file_size = 0;
  is_comment_mode = false;
  memset((char *)ccm_param.command_buffer, 0, gcode_buffer_size);
  command_buffer_count = 0;
  is_force_verify = false;
}
/*20170807堵料修复*/
void GcodeBufHandle::setLeaveFileSize(uint32_t FileSize)
{
  leave_file_size = FileSize;
}

void GcodeBufHandle::setForceVerify(bool isForceVerify)
{
  is_force_verify = isForceVerify;
}

static void Judge_Color(void)
{
  if (ccm_param.t_sys.enable_color_buf)
  {
    if (strstr((char *)ccm_param.command_buffer, "G428 R6"))
    {
      ccm_param.t_sys.enable_color_buf = false;
    }
  }
  else if (!ccm_param.t_sys.enable_color_buf)
  {
    if (strstr((char *)ccm_param.command_buffer, "G428 R7"))
    {
      ccm_param.t_sys.enable_color_buf = true;
    }
  }
}

static bool get_content_from_gcode(void)
{
  if (ccm_param.command_buffer[0] == ';')
  {
    char *layer_p = strstr((char *)&ccm_param.command_buffer[0], ";LAYER:");

    if (layer_p != NULL)
    {
      ccm_param.current_layer = (strtol(layer_p + 7, NULL, 10));
    }

    // Cura Version >= 2.0
    layer_p = strstr((char *)&ccm_param.command_buffer[0], ";LAYER_COUNT:");

    if (layer_p != NULL)
    {
      ccm_param.layer_count = (strtol(layer_p + 13, NULL, 10));
    }

    // Cura Version < 2.0
    layer_p = strstr((char *)&ccm_param.command_buffer[0], ";Layer count:");

    if (layer_p != NULL)
    {
      ccm_param.layer_count = (strtol(layer_p + 13, NULL, 10));
    }

    memset((char *)ccm_param.command_buffer, 0, gcode_buffer_size); // 丢弃该行已读字符
    command_buffer_count = 0;
    is_comment_mode = false;
    return true;
  }

  return false;
}

static void fix_gcode_error(void)
{
  if (ccm_param.layer_count > 1 && ccm_param.current_layer < 1)
  {
    // 修复联机打印第一层没变化，cura生成的gcode多了空格，导致校准异常跳出该指令
    // 软件修复后，可删除该逻辑
    char *pStr = strstr((char *)ccm_param.command_buffer, " E ");

    if (NULL != pStr)
    {
      strcpy(pStr + 2, pStr + 3);
      command_buffer_count--;
    }
  }
}

bool GcodeBufHandle::getFromGcode(const char sd_char, const uint32_t file_position, bool isColorMix, volatile uint32_t &file_size, unsigned int &sd_buffersize)
{
  if ((sd_char == '\r')  || (sd_char == '\n'))
  {
    ccm_param.command_buffer[command_buffer_count] = 0; // 添加字符串结束符
    fix_gcode_error();

    if (get_content_from_gcode())
    {
      return 0;
    }

    if (!command_buffer_count)                // 空行或直接分号的注释行，跳过
    {
      is_comment_mode = false;
      return 0;
    }
    else if (is_comment_mode)       //有带分号注释的Gcode命令行
    {
      Judge_Color();
      updateCmdBuf((char *)&ccm_param.command_buffer[0], isColorMix);                  // 更新当前buffer
      user_send_file_cmd((char *)ccm_param.command_buffer, file_position, ccm_param.layer_count, ccm_param.current_layer);
      command_buffer_count = 0; //clear buffer
      is_comment_mode = false;
      return 1;
    }
    else       //不带分号注释的Gcode命令行
    {
      //当前命令行校验
      if (verifyCmdBuf((char *)&ccm_param.command_buffer[0], is_force_verify, isColorMix, file_size, sd_buffersize) == false)
      {
        memset((char *)ccm_param.command_buffer, 0, gcode_buffer_size); // 丢弃该行已读字符
        command_buffer_count = 0;
        is_comment_mode = false;
        return 0;
      }

      user_send_file_cmd((char *)ccm_param.command_buffer, file_position, ccm_param.layer_count, ccm_param.current_layer);
      command_buffer_count = 0; //clear buffer
      is_comment_mode = false;
      cmd_buf_pos = sd_buffersize;
      leave_file_size = file_size;
      return 1;
    }
  }
  else
  {
    if (command_buffer_count == 0) //不过滤;开头字符串，识别行数
    {
      if (sd_char == '(')
        is_comment_mode = true;  //检测到分号}
    }
    else
    {
      if (sd_char == ';' || sd_char == '(')
        is_comment_mode = true;  //检测到分号
    }

    if (!is_comment_mode)   //存放数据，过滤掉分号后的数据
    {
      if (command_buffer_count >= gcode_buffer_size)
        ccm_param.command_buffer[gcode_buffer_size - 1] = '\r';  //超过缓冲区，截断数据，以防溢出
      else
        ccm_param.command_buffer[command_buffer_count++] = sd_char;
    }
  }

  return 0;
}

void GcodeBufHandle::updateCmdBuf(char *curr_buf, bool isDecrypt)
{
  if (!ccm_param.t_sys.enable_color_buf)
  {
    return;
  }

  char decryptBuf_tmp[gcode_buffer_size] = {0};
  int pos = 0;

  while (curr_buf[pos] != '$')
  {
    // 扫描到校验字符$，退出
    if (pos + 1 < gcode_buffer_size && curr_buf[pos] == ' ' && curr_buf[pos + 1] == '$')
      break;

    // 解码当前字符
    decryptBuf_tmp[pos] = isDecrypt ? user_decryption_code(curr_buf[pos], 20, pos) : curr_buf[pos];

    // 计数超出最大值，退出
    if (++pos == gcode_buffer_size)
      break;
  }

  // 添加字符串结束符
  if (pos < gcode_buffer_size)
    decryptBuf_tmp[pos] = 0;

  command_buffer_count = pos;
  // 将cmdbuffer_temp复制到curr_buf
  memcpy(curr_buf, decryptBuf_tmp, gcode_buffer_size);
}

//仅cmdbuffer校验用
uint8_t GcodeBufHandle::getCheckSum(const char *buf)
{
  // 初始化校验值
  uint8_t chksum = buf[0];

  for (int i = 1; i < gcode_buffer_size; i++)
  {
    // 扫描到校验字符$，退出
    if (i + 1 < gcode_buffer_size && buf[i] == ' ' && buf[i + 1] == '$')
      break;

    chksum ^= buf[i];
  }

  return chksum;
}
void GcodeBufHandle::verifyCmdBufCount(int loopCnt, int &currCnt, uint32_t &retryCnt, uint32_t &ErrorCnt, volatile uint32_t &file_size, unsigned int &sd_buffersize)
{
  // 校验次数小于loopCnt，重新读取指令
  if (currCnt < loopCnt)
  {
    sd_buffersize = cmd_buf_pos;       // 设置pos

    if (leave_file_size) //20170830防止file_size被改为0
      file_size = leave_file_size;

    retryCnt++;                        // 重读计数
    currCnt++;                         // 校验次数加一
  }
  // 校验次数等于loopCnt，指令错误，跳过该指令
  else if (currCnt == loopCnt)
  {
    ErrorCnt++;                        // 错误计数
    currCnt = 0;                       // 校验次数重置为0
  }

  command_buffer_count = 0;            // 清空行字符计数
}
bool GcodeBufHandle::verifyKnownCmd(bool isKnownCmd, bool isChkSumCorrect, volatile uint32_t &file_size, unsigned int &sd_buffersize)
{
  //known command
  static int unknown_cmd_counts = 0;
  static int chksum_loop_cnt = 0;

  // 已知指令
  if (isKnownCmd)
  {
    if (!isChkSumCorrect)
    {
      verifyCmdBufCount(20, chksum_loop_cnt, checksum_retry_counts, checksum_error_counts, file_size, sd_buffersize);
      return false;//return;
    }
    else
    {
      chksum_loop_cnt = 0;
    }

    unknown_cmd_counts = 0;
  }
  else
  {
    //unknown command
    verifyCmdBufCount(3, unknown_cmd_counts, check_unknown_cmd_retry_count, check_unknown_cmd_error_count, file_size, sd_buffersize);
    return false;
  }

  return true;
}
bool GcodeBufHandle::verifyCmdBuf(char *curr_cmd, bool force_verify, bool isColorMix, volatile uint32_t &file_size, unsigned int &sd_buffersize)
{
  // 定义强制校验次数
  static int verifyForceCount = 0;
  // 获取美元符号位置
  char *find_dollar_char = strchr(curr_cmd, '$');

  // M305 S1时，开启强制模式，指令没有校验符号$，直接跳过该指令
  if (find_dollar_char == NULL && force_verify)
  {
    verifyCmdBufCount(3, verifyForceCount, ForceChksum_RetryCnt, ForceChksum_ErrorCnt, file_size, sd_buffersize);
    return false;
  }
  else
  {
    verifyForceCount = 0; // 校验次数重置
  }

  if (find_dollar_char != NULL)
  {
    uint8_t get_buf_chksum = (uint8_t)(strtod(find_dollar_char + 1, NULL));             // 获取命令行附加校验值
    uint8_t cal_buf_chksum = getCheckSum(curr_cmd);                                     // 计算命令行校验值
    bool isChkSumCorrect = (get_buf_chksum == cal_buf_chksum || (cal_buf_chksum^' ') == get_buf_chksum) ? true : false;           // 比较并计算校验结果
    updateCmdBuf(curr_cmd, isColorMix);                                                 // 解码到当前buffer
    bool isKnownCmd = (curr_cmd[0] == 'G' || curr_cmd[0] == 'M' || curr_cmd[0] == 'T'); // 是否为已知指令
    return verifyKnownCmd(isKnownCmd, isChkSumCorrect, file_size, sd_buffersize);
  }

  return true;
}


