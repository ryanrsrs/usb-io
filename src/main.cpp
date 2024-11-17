#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "luatt_context.h"
#include "luatt_loader.h"

#include "periphs.h"
#include "lua_funcs.h"

#include "util.h"

Luatt_Loader Loader;

void Reset_Peripherals() {
    periphs_reset();
}

void Lua_Setup_Funcs() {
    lua_setup_funcs(LUA);
}

static TaskHandle_t Main_Task = 0;

void on_keyfob_int() {
    // use notification value bits as "interrupt channels"
    // channel 0 is the keyfob rx
    int chan = 0;
    xTaskNotifyFromISR(Main_Task, 1 << chan, eSetBits, 0);
}

void setup() {
    periphs_setup();

    Serial.begin(115200);

    // Wait 5 secs, for easy crash recovery.
    // If you connect at 1200 buad, it'll jump to the bootloader.
    digitalWrite(PIN_LED, 1);
    delay(5000);
    digitalWrite(PIN_LED, 0);

    // Interrupt-driven lua routine
    Main_Task = xTaskGetCurrentTaskHandle();
    keyfob_attach_interrupt(on_keyfob_int);
    xTaskNotifyStateClear(0);

    // Initialize Lua state.
    Lua_Reset();
}

// Sleep for ms, with early wakeup for interrupts.
//
// Here's where the microcontroller spends most of its time sleeping.
//
// The nRF52840 @ 64 MHz will idle at 2 mA out of the box, with no extra
// work shutting down periphs or turning off clocks.
uint32_t wait_ms(uint32_t ms) {
    uint32_t ticks = ms2tick(ms);
    uint32_t value = 0;
    xTaskNotifyWait(0, ULONG_MAX, &value, ticks);
    return value;
}

void loop() {
    static uint32_t sleep_ms = 0;
    sleep_ms -= millis();
    if (sleep_ms > INT_MAX) sleep_ms = 0;
    uint32_t int_bits = wait_ms(sleep_ms);

    // Call Lua scheduler to run background threads.
    // Returns number of milliseconds to sleep before
    // next thread needs to run.
    uint32_t ms1 = Lua_Loop(int_bits);

    // Current time.
    uint32_t ms0 = millis();

    // Call Loader to handle USB serial comms with luatt.py
    // Returns how long it wants to sleep (sleeps longer when idle).
    uint32_t ms2 = Loader.Loop();

    // Take minimum and sleep that long on the next loop iteration.
    int ms_min = ms1 < ms2 ? ms1 : ms2;
    sleep_ms = ms0 + ms_min;
}
