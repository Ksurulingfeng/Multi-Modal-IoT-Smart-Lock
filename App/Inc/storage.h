#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "sys.h"
#include "task_headfile.h"

bool add_fingerprint(uint16_t page_id);
bool delete_fingerprint(uint16_t page_id);
bool add_card(uint16_t page_id);
bool delete_card(uint16_t page_id);
bool set_password(const char *password);

#endif /* __STORAGE_H__ */