/*
 * File:   utils.h
 * Author: zhujun
 */

#ifndef UTILS_H
#define	UTILS_H

#define DISABLE_ASSIGN_AND_COPY(TypeName)       \
        TypeName(const TypeName&);              \
        TypeName& operator=(const TypeName&)

//Re-runs fn until it doesn't cause EINTR.
#define NO_INTR(fn)   do {} while ((fn) < 0 && errno == EINTR)

#endif	/* UTILS_H */
