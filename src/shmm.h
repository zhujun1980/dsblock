/*
 * File:   shmm.h
 * Author: zhujun
 */

#ifndef SHMM_H
#define	SHMM_H

namespace dsblock {

void* CreateMmap(const char* file, size_t segsize);

void* MMap(const char* file, size_t segsize, int fg);

bool MUnMap(void* addr, size_t segsize);

}

#endif	/* SHMM_H */
