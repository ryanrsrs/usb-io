#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "luatt_io.h"

static struct {
    char token[32];
    char lua_buf[256];
    int lua_buf_i;
} State = { 0 };

static void fp_flush_buffer() {
    if (Serial) {
        Serial.write(State.token);
        Serial.write(State.lua_buf, State.lua_buf_i);
    }
    State.lua_buf_i = 0;
}

size_t fp_lua_fwrite(const void* buf, size_t size, size_t nitems, FILE* stream) {
    const char* src = (const char*)buf;
    int i, n = size * nitems;
    for (i = 0; i < n; i++) {
        char ch = src[i];
        State.lua_buf[State.lua_buf_i++] = ch;
        if (State.lua_buf_i == sizeof(State.lua_buf) || ch == '\n') {
            fp_flush_buffer();
        }
    }
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
    if (State.lua_buf_i) fp_flush_buffer();
    return 0;
}

void fp_set_token(const char* token) {
    if (State.lua_buf_i) fp_flush_buffer();

    if (token == 0) token = "0";
    snprintf(State.token, sizeof(State.token), "%s|", token);
}
