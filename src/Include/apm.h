#ifndef __APM_H__
#define __APM_H__

#include "system.h"
#include "bios32.h"
#include "printf.h"
#include "kheap.h"
#include "string.h"
#include "gdt.h"
#include "paging.h"

int check_apm();

int apm_connect_interface();
int apm_disconnect_interface();

int apm_chVersion(uint8_t major, uint8_t minor);
int apm_chPM(uint8_t isPowerManagementEnabled);

int apm_setPowerState(uint8_t state);

void apm_standby();
void apm_suspend();
void apm_shutdown();
void apm_restart();

void init_apm();

#endif