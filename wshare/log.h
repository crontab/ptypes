
#ifndef W_LOG_H
#define W_LOG_H


#include "ptypes.h"
#include "pinet.h"


USING_PTYPES


enum log_severity_t {SYSLOG_FATAL, SYSLOG_ERROR, SYSLOG_WARNING, SYSLOG_INFO};


extern compref<logfile> htlog;	   // perr by default
extern compref<logfile> errlog;	   // perr, until daemonized


void log_init();
void log_done();
void syslog_write(log_severity_t pri, const char* fmt, ...);
void htlog_write(ipaddress ip, string request, int code, large size, string referer);


#endif
