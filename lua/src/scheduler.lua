local scheduler = {}

scheduler.recent_ms = time.millis()

scheduler.pq = PriorityQueue{
    -- handle timestamp wrap-around
    higherpriority = function(a, b)
        return (a - scheduler.recent_ms) < (b - scheduler.recent_ms)
	end
}

scheduler.interrupts = {}

function scheduler.loop (ints)
    scheduler.recent_ms = time.millis()

    if ints ~= 0 then
        for co, co_ints in pairs(scheduler.interrupts) do
            if ints & co_ints ~= 0 then
                -- mark for immediate execution
                scheduler.pq:update(co, scheduler.recent_ms)
            end
        end
    end

    while not scheduler.pq:empty() do
        local co, t = scheduler.pq:peek()
        local ms = time.millis()
        if t - ms > 0 then
            return t - ms
        end

        co = scheduler.pq:dequeue()
        scheduler.interrupts[co] = nil

        local r, t_inc, co_ints = coroutine.resume(co, ms)
        if r and t_inc then
            scheduler.pq:enqueue(co, time.millis() + math.floor(t_inc))
            if co_ints and co_ints > 0 then
                scheduler.interrupts[co] = co_ints
            end
        else
            coroutine.close(co)
        end
    end
    return 1000
end

function scheduler.start (co, t_inc)
    scheduler.pq:enqueue(co, time.millis() + math.floor(t_inc or 0))
end

function scheduler.wake (co, t_inc)
    scheduler.pq:update(co, time.millis() + math.floor(t_inc or 0))
end

return scheduler