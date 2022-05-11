#ifndef USER_DEBUG_H
#define USER_DEBUG_H

#define USER_DEBUG_LEVEL 2

#if defined(USE_BOOT)
  #define DEBUG_NAME "BOOT ==>> "
#elif defined(USE_APP1)
  #define DEBUG_NAME "APP1 ==>> "
#elif defined(USE_APP2)
  #define DEBUG_NAME "APP2 ==>> "
#endif

#if (USER_DEBUG_LEVEL > 0)
  #define  USER_EchoLog(...)     printf("ECHO: "); printf(DEBUG_NAME); printf(__VA_ARGS__); printf("\r\n");
  #define  USER_EchoLogStart()   printf("ECHO: "); printf(DEBUG_NAME);
  #define  USER_EchoLogStr(...)     printf(__VA_ARGS__);
#else
  #define  USER_EchoLog(...)
  #define  USER_EchoLogStart()
  #define  USER_EchoLogStr(...)
#endif

#if (USER_DEBUG_LEVEL > 1)
  #define  USER_ErrLog(...)     printf("ERROR: "); printf(DEBUG_NAME); printf(__VA_ARGS__); printf("\r\n");
  #define  USER_ErrLogStart()   printf("ERROR: "); printf(DEBUG_NAME);
  #define  USER_ErrLogStr(...)     printf(__VA_ARGS__);
#else
  #define  USER_ErrLog(...)
  #define  USER_ErrLogStart()
  #define  USER_ErrLogStr(...)
#endif

#if (USER_DEBUG_LEVEL > 2)
  #define  USER_DbgLog(...)     printf("DEBUG: "); printf(DEBUG_NAME); printf(__VA_ARGS__); printf("\r\n");
  #define  USER_DbgLogStart()   printf("DEBUG: "); printf(DEBUG_NAME);
  #define  USER_DbgLogStr(...)     printf(__VA_ARGS__);
#else
  #define  USER_DbgLog(...)
  #define  USER_DbgLogStart()
  #define  USER_DbgLogStr(...)
#endif

#endif // USER_DEBUG_H

