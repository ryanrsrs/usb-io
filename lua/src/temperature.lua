local function publish_temperature ()
    while true do
        local temp = read_temperature()
        if temp == nil then
            -- error reading temp sensor
            return
        end
        local data = { temperature = temp }
        MQ.Publish("usb-io/temperature", json.encode(data))
        coroutine.yield(15 * 1000)
    end
end

scheduler.start(coroutine.create(publish_temperature))
