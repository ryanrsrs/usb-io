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
