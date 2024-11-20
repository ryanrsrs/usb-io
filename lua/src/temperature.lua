local function publish_temperature ()
    while true do
        local data = { temperature = read_temperature() }
        MQ.Publish("usb-io/temperature", json.encode(data))
        coroutine.yield(60 * 1000)
    end
end

if read_temperature() == nil then
    print("Error: temperature sensor not present.")
else
    scheduler.start(coroutine.create(publish_temperature))
end
