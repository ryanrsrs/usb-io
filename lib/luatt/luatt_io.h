#ifndef FAKE_POSIX_H
#define FAKE_POSIX_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t fp_lua_fwrite(const void* buf, size_t size, size_t nitems, FILE* stream);
int fp_lua_fprintf(FILE* stream, const char* format, ...);
int fp_lua_fflush(FILE* stream);
void fp_set_token(const char* token);

#define fwrite(a,b,c,d) fp_lua_fwrite((a),(b),(c),(d))
#define fprintf(...) fp_lua_fprintf(__VA_ARGS__)
#define fflush(s) fp_lua_fflush((s))

#ifdef __cplusplus
}
#endif

#endif