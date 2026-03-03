#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "sys.h"
#include <stdio.h>

#define Log_Info(x, ...) printf("\n############# DEBUG TOP #############\n%s:%d:\n" x \
                                "\n############# DEBUG END #############\n",          \
                                __FUNCTION__,                                         \
                                __LINE__,                                             \
                                ##__VA_ARGS__)

#endif /* __DEBUG_H__ */
