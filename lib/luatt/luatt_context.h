#ifndef LUATT_CONTEXT_H
#define LUATT_CONTEXT_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

extern struct lua_State* LUA;

void Lua_Reset();
int Lua_Loop(uint32_t interrupt_flags);

// in main.cpp
void Reset_Peripherals();
void Lua_Setup_Funcs();

#endif