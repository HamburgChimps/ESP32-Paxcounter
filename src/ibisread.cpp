#if (HAS_IBIS)

#include "globals.h"

// Local logging tag
static const char TAG[] = __FILE__;

TaskHandle_t IbisTask;
HardwareSerial ibisSerial(2); // use UART #2
TinyIBIS::Ibis ibis = TinyIBIS::Ibis();

int ibis_init(void) {
  int ret = 1;
  ESP_LOGI(TAG, "Using serial IBIS");
  ibisSerial.begin(IBIS_SERIAL);
  return ret;
}

void ibis_storeStatus(ibisStatus_t *ibis_store) {
  ibis_store->DS001 = ibis.DS001.value;
  // ibis_store->DS003 = ibis.DS003.value;
  ibis_store->DS036 = ibis.DS036.value;
  // ibis_store->DS010E = ibis.DS010E.value;
}

void ibis_storeTest(ibisStatus_t *ibis_store) {
  ibis_store->DS001 = 999;
  // ibis_store->DS003 = 999;
  ibis_store->DS036 = 9999;
  // ibis_store->DS010E = 0;
}

bool ibis_isValid(void) {
  return ibis.DS001.valid && ibis.DS001.age() <= 60000;
}

void ibis_loop(void *pvParameters) {
  configASSERT(((uint32_t)pvParameters) == 1); // FreeRTOS check

  while (1) {
    while (ibisSerial.available()) {
      ibis.encode(ibisSerial.read());
    }
    delay(2); // yield to CPU
  }
}

#endif // HAS_IBIS