/*
 * File:   time_utils.h
 * Author: zhujun
 */

#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <sys/time.h>

#ifdef __cplusplus
}
#endif

namespace dsblock {

double Now();

}

#endif /* TIME_UTILS_H */
