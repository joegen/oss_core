#include <config.h>
#include "ucarp.h"

#ifdef WITH_DMALLOC
# include <dmalloc.h>
#endif

void carp_set_log_callback(on_log_callback_t cb)
{
  on_log_callback = cb;
}

void logfile(const int crit, const char *format, ...)
{
    const char *urgency;    
    va_list va;
    char line[MAX_SYSLOG_LINE];
    
    va_start(va, format);
    vsnprintf(line, sizeof line, format, va);
    switch (crit) {
    case LOG_INFO:
        urgency = "[INFO] ";
        break;
    case LOG_WARNING:
        urgency = "[WARNING] ";
        break;
    case LOG_ERR:
        urgency = "[ERROR] ";
        break;
    case LOG_NOTICE:
        urgency = "[NOTICE] ";
        break;
    case LOG_DEBUG:
        urgency = "[DEBUG] ";
        break;
    default:
        urgency = "";
    }

    if (no_syslog == 0) 
    {

      if (on_log_callback)
      {
        (*on_log_callback)(crit, line);
      }
      else
      {
#ifdef SAVE_DESCRIPTORS
        openlog("ucarp", LOG_PID, syslog_facility);
#endif
        syslog(crit, "%s%s", urgency, line);
#ifdef SAVE_DESCRIPTORS
        closelog();
#endif
      }
    }
#if 0
    if (!on_log_callback && daemonize == 0) {
        switch (crit) {
        case LOG_WARNING:
        case LOG_ERR:
            fprintf(stderr, "%s%s\n", urgency, line);
            break;
        default:
            printf("%s%s\n", urgency, line);
        }
    }
#endif
    va_end(va);
}
