#ifndef PERIPHS_H
#define PERIPHS_H

#define PIN_LED 3

#define PIN_SW 4
#define PIN_BUZZER 14

#define PIN_D0 24
#define PIN_D1 23
#define PIN_D2 2
#define PIN_D3 25
#define PIN_D4 19

#define PIN_VT 18

#define PIN_OUT0 7
#define PIN_OUT1 9
#define PIN_OUT2 10
#define PIN_OUT3 13
#define PIN_OUT4 12
#define PIN_OUT5 11

// Configure periphs at startup.
void periphs_setup();

// Restore periphs to initial state.
// This is called when we wipe the Lua state, e.g. !reset
void periphs_reset();


// Set the brightness and color of the RGB LED.
void dotstar_set_brightness(uint8_t br);
void dotstar_set_color(uint32_t color);

// True if PCT2075 I2C temp sensor exists.
bool temperature_valid();

// Read board temp sensor in deg C.
float temperature_read();


// Read keyfob rx from Linx KH3 receiver.
uint32_t keyfob_read();

// Attach func to keyfob "valid rx" interrupt.
void keyfob_attach_interrupt(void (*func)());


// Set usb-io relay output.
void set_output(int out_ix, bool value);

// Set all 6 outputs at once.
void set_outputs(uint8_t values);

struct lua_State;
void setup_lua_periphs(lua_State* L);

#endif