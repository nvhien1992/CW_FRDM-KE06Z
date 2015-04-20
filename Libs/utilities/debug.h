/*
 * debug.h
 *
 *  Created on: Feb 12, 2015
 *      Author: nvhie_000
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>

#define DISABLE_ALL (0)

#if !DISABLE_ALL && NOTIFY_EN
#define NOTIFY(...) printf(__VA_ARGS__)
#else
#define NOTIFY(...) 
#endif

#if !DISABLE_ALL && DEBUG_EN
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...) 
#endif

#endif /* DEBUG_H_ */
