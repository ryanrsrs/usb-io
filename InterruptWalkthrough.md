# Interrupt handling in Lua

Just because we're cooperatively-scheduled, doesn't mean we don't need hardware interrupt support. Luatt supports classic top-half/bottom-half interrupt handlers, with the bottom-half written in Lua.

## Bottom-half handler in C++
Use FreeRTOS task notifications to wake up the main task.
```
void on_keyfob_int() {
    // use notification value bits as "interrupt channels"
    // channel 0 is the keyfob rx
    int chan = 0;
    xTaskNotifyFromISR(Main_Task, 1 << chan, eSetBits, 0);
}
```
https://github.com/ryanrsrs/usb-io/blob/main/src/main.cpp#L29


## Main event loop
The Arduino runtime calls loop() continuously.

When nothing is running, the main task blocks in wait_ms(). The timeout value comes from the Lua scheduler, and is the soonest time a sleeping thread wants to wake. Notifications from interrupts can wake us early.
```
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
```
https://github.com/ryanrsrs/usb-io/blob/main/src/main.cpp#L56

## Lua_Loop()
Look up the scheduler's Lua entrypoint and call it. Pass in interrupt flags from the task notification value.
```
int Lua_Loop(uint32_t interrupt_flags) {
    int max_sleep = 5000;
    if (!LUA) return max_sleep;

    fp_set_token("sched");

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
        fp_lua_fprintf(0, "error|%s:%i,%i,%s\n", __FILE__, __LINE__, r, err_str);
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
```
https://github.com/ryanrsrs/usb-io/blob/main/lib/luatt/luatt_context.cpp#L19

## Scheduler
Implemented with a priotity queue in pure Lua.

Correctly handles 2^32 millisecond wrap-around.
```
-- Called by the main Arduino loop.
-- The C code looks up "scheduler.loop" in the Lua global scope and calls it.
-- Returns the number of millseconds the system should sleep.
function scheduler.loop (ints)
    scheduler.recent_ms = time.millis()

    -- If we have pending interrupts, wake any threads waiting on them.
    if ints ~= 0 then
        for co, co_ints in pairs(scheduler.interrupts) do
            if ints & co_ints ~= 0 then
                -- mark for immediate execution
                scheduler.pq:update(co, scheduler.recent_ms)
            end
        end
    end
    -- TODO: save interrupt bits that nobody is waiting on so they
    -- don't get lost. Needed once my interrupt handlers
    -- get more complex.

    -- Now run any threads that are ready.
    while not scheduler.pq:empty() do
        -- Find next thread to run according to wakeup time.
        local co, t = scheduler.pq:peek()
        local ms = time.millis()
        if t - ms > 0 then
            -- no threads ready to run
            return t - ms
        end

        co = scheduler.pq:dequeue()
        scheduler.interrupts[co] = nil

        -- Run thread coroutine.
        local r, t_inc, co_ints = coroutine.resume(co, ms)
        if r and t_inc then
            -- coroutine wants to sleep
            scheduler.pq:enqueue(co, time.millis() + math.floor(t_inc))
            if co_ints and co_ints > 0 then
                -- coroutine is listening for interrupts
                scheduler.interrupts[co] = co_ints
            end
        else
            -- coroutine has exited
            coroutine.close(co)
        end
    end
    -- All threads have exited, so just sleep for a second.
    -- We'll be called early if a new thread is created.
    return 1000
end
```
https://github.com/ryanrsrs/usb-io/blob/main/lua/src/scheduler.lua#L14

## Interrupt handler bottom-half in Lua
Read received button state and fire off a message to the MQTT broker.
```
local function keyfob (ints)
    local last_valid = false
    while true do
        -- sleep on interrupt channel 0
        coroutine.yield(10000, 0x01)

        -- interrupt handler bottom-half
        local x = read_keyfob()
        local valid = x & 256 > 0
        if valid ~= last_valid then
            last_valid = valid

            -- green LED during valid rx
            dotstar.set_color(valid and 0x00ff00 or 0)

            local data = { buttons = (x & 255) }
            MQ.Publish("usb-io/keyfob", json.encode(data))
        end
    end
end

scheduler.start(coroutine.create(keyfob))
```
https://github.com/ryanrsrs/usb-io/blob/main/lua/src/keyfob.lua
