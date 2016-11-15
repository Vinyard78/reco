/*--------------------------------------------------------------------------*/

#ifndef __PIRIX_LOG_H__
#define __PIRIX_LOG_H__

/*--------------------------------------------------------------------------*/

#include <syslog.h>

/*--------------------------------------------------------------------------*/

#define LOG_MAX_LEN	1024

/*--------------------------------------------------------------------------*/

typedef enum log_method_e
{
  LOG_METHOD_SYSLOG,
  LOG_METHOD_FILE,
  LOG_METHOD_STD,

} log_method_t;

/*--------------------------------------------------------------------------*/

#define PIRIX_LOG_OPEN(method, level, exec_name, str) \
  pirix_log_open((method), (level), (exec_name), (str))

#define PIRIX_LOG(type, section, ...) \
  pirix_log((type), (section), 0, -1, __VA_ARGS__)

#define PIRIX_LOG_WITH_FILE_INFO(type, section, ...) \
  pirix_log((type), (section), __FILE__, __LINE__, __VA_ARGS__)

#define PIRIX_LOG_CLOSE() \
  pirix_log_close()

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  extern "C" {
#endif

/*--------------------------------------------------------------------------*/

void pirix_log_open(log_method_t method, int level, const char *str, int chanel);

void pirix_log(int level, const char *section, const char *file, int line, ...);

void pirix_log_close(void);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  }
#endif

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

