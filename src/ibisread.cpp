#if (HAS_IBIS)

#include "globals.h"

// Local logging tag
static const char TAG[] = __FILE__;

TaskHandle_t IbisTask;
HardwareSerial ibisSerial(2); // use UART #2


int ibis_init(void) {
  int ret = 1;
  ESP_LOGI(TAG, "Using serial IBIS");
  ibisSerial.begin(IBIS_SERIAL);
  return ret;
}

void ibis_loop(void *pvParameters) {
  configASSERT(((uint32_t)pvParameters) == 1); // FreeRTOS check

  while (1) {
    // ESP_LOGI(TAG, "IBIS LOOP");
    delay(2); // yield to CPU
  }
}

#endif // HAS_IBIS