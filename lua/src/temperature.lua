local function publish_temperature ()
    while true do
        local data = { temperature = read_temperature() }
        MQ.Publish("usb-io/temperature", json.encode(data))
        coroutine.yield(15 * 1000)
    end
end

scheduler.start(coroutine.create(publish_temperature))
