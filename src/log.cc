/*
 * File:   log.cc
 * Author: zhujun
 */
#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include "log.h"

#define MAXLINE 256

#define LEVELCOUNT 4

namespace dsblock {

static const char* levels[LEVELCOUNT] = {
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG"
};

class DefaultLogger : public Logger {
public:
    DefaultLogger() {
    }

    virtual ~DefaultLogger() {
    }

    virtual void operator()(int type, const char *format, ...) {
        const char* level = "";
        char buf[MAXLINE + 1] = {0};

        if(type >= 0 || type < LEVELCOUNT) {
            level = levels[type];
        }

        va_list	ap;
        va_start(ap, format);
	    vsnprintf(buf, MAXLINE, format, ap);	/* safe */
        printf("%s %s\n", level, buf);
        va_end(ap);
    }
};

Logger* logger_global = new DefaultLogger();

}

