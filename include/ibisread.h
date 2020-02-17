#ifndef _IBISREAD_H
#define _IBISREAD_H

#include "TinyIBIS.h"
#include <RtcDateTime.h>
#include "timekeeper.h"



extern TinyIBIS::Ibis ibis;
extern TaskHandle_t IbisTask;

int ibis_init(void);
// void ibis_storeTelegram(gpsStatus_t *gps_store);
void ibis_loop(void *pvParameters);

#endif