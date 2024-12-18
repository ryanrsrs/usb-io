#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <Adafruit_DotStar.h>
#include <Adafruit_PCT2075.h>

#include <luatt.h>

#include <periphs.h>

#define I2C_PCT2075 0x48
Adafruit_PCT2075 Temperature;
bool Temperature_Valid;

#define PIN_DOTSTAR_CLK 6
#define PIN_DOTSTAR_DAT 8
Adafruit_DotStar Dotstar(1, PIN_DOTSTAR_DAT, PIN_DOTSTAR_CLK, DOTSTAR_BGR);

static void pin_config(uint8_t pin, uint8_t mode, uint8_t value=0)
{
    pinMode(pin, mode);
    if (mode == OUTPUT) digitalWrite(pin, value);
}

void periphs_setup()
{
    pin_config(PIN_LED, OUTPUT, 0);
    pin_config(PIN_SW, INPUT_PULLUP);

    pin_config(PIN_BUZZER, OUTPUT, 0);

    pin_config(PIN_D0, INPUT);
    pin_config(PIN_D1, INPUT);
    pin_config(PIN_D2, INPUT);
    pin_config(PIN_D3, INPUT);
    pin_config(PIN_D4, INPUT);

    pin_config(PIN_VT, INPUT);

    pin_config(PIN_OUT0, OUTPUT, 0);
    pin_config(PIN_OUT1, OUTPUT, 0);
    pin_config(PIN_OUT2, OUTPUT, 0);
    pin_config(PIN_OUT3, OUTPUT, 0);
    pin_config(PIN_OUT4, OUTPUT, 0);
    pin_config(PIN_OUT5, OUTPUT, 0);

    Temperature_Valid = Temperature.begin(I2C_PCT2075);
    Dotstar.begin();
}

void periphs_reset()
{
    digitalWrite(PIN_LED, 0);
    digitalWrite(PIN_BUZZER, 0);

    digitalWrite(PIN_OUT0, 0);
    digitalWrite(PIN_OUT1, 0);
    digitalWrite(PIN_OUT2, 0);
    digitalWrite(PIN_OUT3, 0);
    digitalWrite(PIN_OUT4, 0);
    digitalWrite(PIN_OUT5, 0);

    Dotstar.setBrightness(255);
    Dotstar.setPixelColor(0, 0);
    Dotstar.show();
}

void dotstar_set_brightness(uint8_t br)
{
    Dotstar.setBrightness(br);
    Dotstar.show();
}

void dotstar_set_color(uint32_t color)
{
    Dotstar.setPixelColor(0, color);
    Dotstar.show();
}

bool temperature_valid()
{
    return Temperature_Valid;
}

float temperature_read()
{
    if (!Temperature_Valid) return -99.0f;
    return Temperature.getTemperature();
}

void keyfob_attach_interrupt(void (*func)())
{
    attachInterrupt(digitalPinToInterrupt(PIN_VT), func, CHANGE);
}

uint32_t keyfob_read()
{
    int x = 0;
    if (digitalRead(PIN_D0)) x |= 1; // left
    if (digitalRead(PIN_D1)) x |= 2; // down
    if (digitalRead(PIN_D2)) x |= 4; // right
    if (digitalRead(PIN_D3)) x |= 8; // up
    if (digitalRead(PIN_D4)) x |= 16; // center
    if (digitalRead(PIN_VT)) x |= 256; // valid transmission
    return x;
}

void set_output(int out_ix, bool value)
{
    const int outputs[] = { PIN_OUT0, PIN_OUT1, PIN_OUT2, PIN_OUT3, PIN_OUT4, PIN_OUT5 };
    const int n_outputs = sizeof(outputs) / sizeof(outputs[0]);
    if (out_ix < 0 || out_ix >= n_outputs) {
        printf("error|%s:%i,set_output(%i) bad index.\n", __FILE__, __LINE__, out_ix);
        return;
    }
    digitalWrite(outputs[out_ix], value);
}

void set_outputs(uint8_t values)
{
    digitalWrite(PIN_OUT0, (values & 0x01) > 0);
    digitalWrite(PIN_OUT1, (values & 0x02) > 0);
    digitalWrite(PIN_OUT2, (values & 0x04) > 0);
    digitalWrite(PIN_OUT3, (values & 0x08) > 0);
    digitalWrite(PIN_OUT4, (values & 0x10) > 0);
    digitalWrite(PIN_OUT5, (values & 0x20) > 0);
}

static int lf_read_temperature(lua_State *L) {
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

static int lf_read_keyfob(lua_State *L) {
    lua_pushinteger(L, keyfob_read());
    return 1;
}

static int lf_set_output(lua_State *L) {
    uint32_t out_ix = luaL_checkinteger(L, 1);
    uint32_t value = luaL_checkinteger(L, 2);
    set_output(out_ix, value);
    return 0;
}

void setup_lua_periphs(lua_State* L)
{
    luatt_setup_funcs_dotstar(L, &Dotstar);
    luatt_setup_funcs_red_led(L, PIN_LED);

    lua_pushcfunction(L, lf_read_temperature);
    lua_setglobal(L, "read_temperature");

    lua_pushcfunction(L, lf_read_keyfob);
    lua_setglobal(L, "read_keyfob");

    lua_pushcfunction(L, lf_set_output);
    lua_setglobal(L, "set_output");
}
