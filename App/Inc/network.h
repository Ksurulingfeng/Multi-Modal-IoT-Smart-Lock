#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "sys.h"
#include "task_headfile.h"

void network_init(void);
bool connect_wifi(void);
bool disconnect_wifi(void);
bool connect_mqtt(void);
bool network_ping(void);
void network_cmd_process(void);

#endif /* __NETWORK_H__ */
