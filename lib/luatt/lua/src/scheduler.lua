local scheduler = {}

scheduler.recent_ms = time.millis()

scheduler.pq = PriorityQueue{
    -- handle timestamp wrap-around
    higherpriority = function(a, b)
        return (a - scheduler.recent_ms) < (b - scheduler.recent_ms)
	end
}

scheduler.interrupts = {}
scheduler.tokens = {}

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
        set_mux_token(scheduler.tokens[co])
        local r, t_inc, co_ints = coroutine.resume(co, ms)
        set_mux_token("sched")

        if r and t_inc then
            -- coroutine wants to sleep
            scheduler.pq:enqueue(co, time.millis() + math.floor(t_inc))
            if co_ints and co_ints > 0 then
                -- coroutine is listening for interrupts
                scheduler.interrupts[co] = co_ints
            end
        else
            -- coroutine has exited
            scheduler.tokens[co] = nil
            coroutine.close(co)
        end
    end
    -- All threads have exited, so just sleep for a second.
    -- We'll be called early if a new thread is created.
    return 1000
end

-- Start a new thread after t_inc milliseconds.
function scheduler.start (co, t_inc)
    scheduler.tokens[co] = get_mux_token()
    scheduler.pq:enqueue(co, time.millis() + math.floor(t_inc or 0))
end

-- Reschedule a thread to run after t_inc milliseconds.
-- Can be used to wake up early.
function scheduler.wake (co, t_inc)
    scheduler.pq:update(co, time.millis() + math.floor(t_inc or 0))
end

return scheduler