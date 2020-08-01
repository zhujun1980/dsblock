/*
 * File:   log.h
 * Author: zhujun
 */

#ifndef LOG_H
#define	LOG_H

#include "utils.h"

namespace dsblock {

class Logger {
public:
    enum Type {
        INVALID = -1,
        ERROR = 0,
        WARNING,
        INFO,
        DEBUG,
    };

    Logger() {
    }

    virtual ~Logger() {
    }

    virtual void operator()(int type, const char *format, ...) = 0;

private:
    DISABLE_ASSIGN_AND_COPY(Logger);
};

extern Logger* logger_global;

#define DoLog logger_global->operator()

}

#endif	/* LOG_H */
