#ifndef LUATT_CONTEXT_H
#define LUATT_CONTEXT_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

extern struct lua_State* LUA;

typedef void (*luatt_setup_callback)(struct lua_State*);
void Lua_Begin(luatt_setup_callback setup_cb);

void Lua_Reset();
int Lua_Loop(uint32_t interrupt_flags);

#endif