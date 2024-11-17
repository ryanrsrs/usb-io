#ifndef LUA_FUNCS_H
#define LUA_FUNCS_H

// Populate new lua_State with our C function entrypoints.
//
// Add lua-accessible functions, assign values and tables to global vars, etc.

struct lua_State;
void lua_setup_funcs(lua_State* L);

#endif