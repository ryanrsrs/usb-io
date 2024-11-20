#include <Arduino.h>
#include "Adafruit_TinyUSB.h"

#include "luatt_context.h"

struct lua_State* LUA = 0;

void Lua_Reset() {
    if (LUA) {
        lua_close(LUA);
    }

    LUA = luaL_newstate();
    luaL_openlibs(LUA);
    Reset_Peripherals();
    Lua_Setup_Funcs();
}

int Lua_Loop(uint32_t interrupt_flags) {
    int max_sleep = 5000;
    if (!LUA) return max_sleep;

    Serial.set_mux_token("sched");

    int r = lua_getglobal(LUA, "scheduler");
    if (r != LUA_TTABLE) {
        lua_pop(LUA, lua_gettop(LUA));
        return max_sleep;
    }

    r = lua_getfield(LUA, -1, "loop");
    if (r != LUA_TFUNCTION) {
        lua_pop(LUA, lua_gettop(LUA));
        return max_sleep;
    }

    lua_remove(LUA, -2);

    lua_pushinteger(LUA, interrupt_flags);

    r = lua_pcall(LUA, 1, 1, 0);
    if (r != LUA_OK) {
        const char* err_str = lua_tostring(LUA, lua_gettop(LUA));
        printf("error|%s:%i,%i,%s\n", __FILE__, __LINE__, r, err_str);
        lua_pop(LUA, 1);
        return max_sleep;
    }
    if (lua_gettop(LUA) > 0) {
        uint32_t ms = lua_tointeger(LUA, -1);
        lua_pop(LUA, 1);
        return ms;
    }
    return max_sleep;
}