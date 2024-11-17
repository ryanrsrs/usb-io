#include <Arduino.h>
#include "Adafruit_TinyUSB.h"

#include "periphs.h"
#include "util.h"


// Enable %f in printf.
asm(".global _printf_float");

#if 1
void serial_printf(const char* fmt, ...) {
    if (!Serial) return;
    static char buf[256];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    if (n < 0) n = 0;
    if (n >= (int)sizeof(buf)) n = (int)sizeof(buf) - 1;
    va_end(ap);
    Serial.print(buf);
}
#endif

const char* esc_string(const char* src, int count) {
    static char buf[256];
    memset(buf, 0, sizeof(buf));

    size_t i = 0;
    while (count > 0 && i < sizeof(buf) - 1) {
        int ch = *src++;
        count--;

        if (isprint(ch)) {
            buf[i++] = ch;
        }
        else {
            i += snprintf(buf + i, sizeof(buf) - i, "\\x%02x", ch);
        }
    }
    if (i >= sizeof(buf)) i = sizeof(buf) - 1;
    buf[i] = 0;
    return buf;
}

char* log_lines[256] = { 0 };
int log_lines_i = 0;
bool log_was_printed = false;
uint32_t log_start_ms = 0;

void log_printf(const char* fmt, ...) {
return;
    if (log_start_ms == 0) {
        log_start_ms = millis();
    }

    int lines_max = sizeof(log_lines) / sizeof(log_lines[0]);
    if (log_lines_i >= lines_max) return;

    static char buf[256];

    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    if (n < 0) n = 0;
    if (n >= (int)sizeof(buf)) n = (int)sizeof(buf) - 1;
    buf[n] = 0;

    va_end(ap);
    log_lines[log_lines_i++] = strdup(buf);
}

void log_reset() {
    int i;
    for (i = 0; i < log_lines_i; i++) {
        free(log_lines[i]);
        log_lines[i] = 0;
    }
    log_lines_i = 0;
    log_was_printed = false;
    log_start_ms = millis();
}

void print_log_after(uint32_t ms) {
    if (log_was_printed || log_lines_i == 0) {
        return;
    }
    if (log_start_ms == 0) {
        log_start_ms = millis();
    }
    if (millis() - log_start_ms < ms) {
        return;
    }

    Serial.write("\r\n\r\n");
    int i;
    for (i = 0; i < log_lines_i; i++) {
        char* line = log_lines[i];
        Serial.write(line);
        Serial.write("\r\n");
        free(line);
    }
    log_lines_i = 0;
    Serial.write("\r\n\r\n");
    log_was_printed = true;
}

extern "C" {
int dbgHeapTotal(void);
int dbgHeapUsed(void);
int dbgStackTotal(void);
int dbgStackUsed(void);
}


int freeMemory() {
    return dbgHeapUsed();
}


#if 0
void fatal_error(const char* file, int line, const char* msg) {
    bool connected = false;
    while (1) {
        if (Serial) {
            if (!connected) {
                delay(10);
                serial_printf("%s:%i %s\r\n", file, line, msg);
                Serial.flush();
                connected = true;
            }
        }
        else {
            connected = false;
        }

        uint32_t ms = millis();
        digitalWrite(PIN_LED, ms % 200 < 10);
        yield();
    }
}

void ryan_assert(int cond, const char* file, int line) {
    if (!cond) fatal_error(file, line, "FreeRTOS assert");
}
#endif
