#ifndef UTIL_H
#define UTIL_H

// serial_printf() is in lua_shell.cpp
void serial_printf(const char* fmt, ...);

void log_printf(const char* fmt, ...);
void log_reset();
void print_log_after(uint32_t ms);
const char* esc_string(const char* src, int count);

int freeMemory();


#if 0

#define FATAL_ERROR(msg) fatal_error(__FILE__, __LINE__, (msg))

void fatal_error(const char* file, int line, const char* msg);

bool serial_is_connected(); // actually in usb_shell.h/cpp
#endif

#endif