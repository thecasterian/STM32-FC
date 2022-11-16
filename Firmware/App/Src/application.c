#include <stdio.h>
#include "application.h"
#include "main.h"
#include "usbd_cdc_if.h"

void setup(void) {

}

void loop(void) {

}

int _write(int file, char *ptr, int len) {
    int res;

    UNUSED(file);

    if (CDC_Transmit_FS((uint8_t*)ptr, len) != USBD_OK) {
        res = -1;
    } else {
        res = len;
    }

    return res;
}
