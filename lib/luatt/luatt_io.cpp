#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "luatt_io.h"

#if 0
size_t fp_lua_fwrite(const void* buf, size_t size, size_t nitems, FILE* stream) {
    Serial.write((const uint8_t*)buf, size * nitems);
    return nitems;
}

int fp_lua_fprintf(FILE* stream, const char* format, ...) {
    static char buf[256];
    va_list ap;
    va_start(ap, format);
    int n = vsnprintf(buf, sizeof(buf), format, ap);
    if (n < 0) n = 0;
    if (n >= (int)sizeof(buf)) n = (int)sizeof(buf) - 1;
    va_end(ap);
    fp_lua_fwrite(buf, 1, n, 0);
    return n;
}

int fp_lua_fflush(FILE* stream) {
    return 0;
}
#endif

void fp_set_token(const char* token) {
    Serial.set_mux_token(token);
    return;
}
