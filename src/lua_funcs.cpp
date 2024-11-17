#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include <malloc.h>

#include "luatt_context.h"

#include "periphs.h"
#include "util.h"

// Wrapper functions exported to Lua.
//
//These interface our C++ APIs with Lua's calling conventions.

int luaf_dotstar_set_brightness(lua_State *L) {
    int x = 256 * luaL_checknumber(L, 1);
    if (x < 0) x = 0;
    else if (x > 255) x = 255;
    dotstar_set_brightness(x);
    return 0;
}

int luaf_dotstar_set_color(lua_State *L) {
    uint32_t x = luaL_checkinteger(L, 1);
    dotstar_set_color(x);
    return 0;
}

int luaf_read_temperature(lua_State *L) {
    if (!temperature_valid()) {
        // return nil if temp sensor not present
        lua_pushnil(L);
    }
    else {
        float deg_c = temperature_read();
        lua_pushnumber(L, deg_c);
    }
    return 1;
}

int luaf_time_millis(lua_State *L) {
    uint32_t ms = millis();
    lua_pushinteger(L, ms);
    return 1;
}

int luaf_time_micros(lua_State *L) {
    uint32_t us = micros();
    lua_pushinteger(L, us);
    return 1;
}

int luaf_read_keyfob(lua_State *L) {
    lua_pushinteger(L, keyfob_read());
    return 1;
}

int luaf_meminfo(lua_State *L) {
    dbgMemInfo();
    return 0;
}

int luaf_led(lua_State *L) {
    uint32_t x = luaL_checkinteger(L, 1);
    digitalWrite(PIN_LED, x != 0);
    return 0;
}

int luaf_set_output(lua_State *L) {
    uint32_t out_ix = luaL_checkinteger(L, 1);
    uint32_t value = luaL_checkinteger(L, 2);
    set_output(out_ix, value);
    return 0;
}

void lua_setup_funcs(lua_State* L) {
    lua_pushcfunction(L, luaf_read_temperature);
    lua_setglobal(L, "read_temperature");

    const struct luaL_Reg dotstar_table[] = {
        { "set_brightness", luaf_dotstar_set_brightness },
        { "set_color",      luaf_dotstar_set_color },
        { 0, 0 }
    };
    lua_newtable(L);
    luaL_setfuncs(L, dotstar_table, 0);
    lua_setglobal(L, "dotstar");


    const struct luaL_Reg time_table[] = {
        { "millis", luaf_time_millis },
        { "micros", luaf_time_micros },
        { 0, 0 }
    };
    lua_newtable(L);
    luaL_setfuncs(L, time_table, 0);
    lua_setglobal(L, "time");

    lua_pushcfunction(L, luaf_meminfo);
    lua_setglobal(L, "meminfo");

    lua_pushcfunction(L, luaf_read_keyfob);
    lua_setglobal(L, "read_keyfob");

    lua_pushcfunction(L, luaf_led);
    lua_setglobal(L, "led");

    lua_pushcfunction(L, luaf_set_output);
    lua_setglobal(L, "set_output");
}
