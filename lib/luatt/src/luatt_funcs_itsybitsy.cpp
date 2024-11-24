#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <Adafruit_DotStar.h>

#include "luatt_context.h"
#include "luatt_funcs_itsybitsy.h"


///////////////////////////////////
// Dotstar LED (single).

static struct {
    Adafruit_DotStar* dotstar;
    bool implicit_show;
} State_dotstar;

static int lf_dotstar_set_brightness(lua_State *L) {
    if (State_dotstar.dotstar == 0) return 0;

    int x = 256 * luaL_checknumber(L, 1);
    if (x < 0) x = 0;
    else if (x > 255) x = 255;

    State_dotstar.dotstar->setBrightness(x);
    if (State_dotstar.implicit_show) {
        State_dotstar.dotstar->show();
    }
    return 0;
}

static int lf_dotstar_set_color(lua_State *L) {
    if (State_dotstar.dotstar == 0) return 0;

    uint32_t x = luaL_checkinteger(L, 1);

    State_dotstar.dotstar->setPixelColor(0, x);
    if (State_dotstar.implicit_show) {
        State_dotstar.dotstar->show();
    }
    return 0;
}

static int lf_dotstar_show(lua_State *L) {
    if (State_dotstar.dotstar == 0) return 0;
    State_dotstar.dotstar->show();
    return 0;
}

void luatt_setup_funcs_dotstar(lua_State* L, Adafruit_DotStar* dotstar, bool implicit_show) {
    State_dotstar.dotstar = dotstar;
    State_dotstar.implicit_show = implicit_show;

    static const struct luaL_Reg dotstar_table[] = {
        { "set_brightness", lf_dotstar_set_brightness },
        { "set_color",      lf_dotstar_set_color },
        { "show",           lf_dotstar_show },
        { 0, 0 }
    };
    lua_newtable(L);
    luaL_setfuncs(L, dotstar_table, 0);
    lua_setglobal(L, "dotstar");
}


///////////////////////////////////
// Red LED.

static struct {
    int pin;
    bool active_low;
} State_red_led = { -1 };

static int lf_set_red_led(lua_State *L) {
    if (State_red_led.pin == -1) return 0;

    uint32_t x = luaL_checkinteger(L, 1);
    if (State_red_led.active_low) x = (x == 0);
    digitalWrite(State_red_led.pin, x != 0);
    return 0;
}

void luatt_setup_funcs_red_led(lua_State* L, int led_pin, bool active_low) {
    State_red_led.pin = led_pin;
    State_red_led.active_low = active_low;

    lua_pushcfunction(L, lf_set_red_led);
    lua_setglobal(L, "set_red_led");
}
