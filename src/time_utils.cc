/*
 * File:   time_utils.cc
 * Author: zhujun
 */
#include "time_utils.h"

namespace dsblock {

double Now() {
    struct timeval tp = {0};
    if (!gettimeofday(&tp, NULL)) {
        return (double)(tp.tv_sec + tp.tv_usec / 1000000.00);
    }
    else {
        return (double)time(0);
    }
}

}  // namespace dsblock
