#ifndef _IBISREAD_H
#define _IBISREAD_H

#include "TinyIBIS.h"
#include <RtcDateTime.h>
#include "timekeeper.h"



extern TinyIBIS::Ibis ibis;
extern TaskHandle_t IbisTask;

int ibis_init(void);
bool ibis_isValid(void);
void ibis_storeStatus(ibisStatus_t *ibis_store);
void ibis_storeTest(ibisStatus_t *ibis_store);
void ibis_loop(void *pvParameters);

#endif