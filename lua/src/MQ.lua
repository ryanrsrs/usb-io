local MQ = {}

local function topic_to_pattern (topic)
    local tail = string.sub(topic, -1) == "#"
    if tail then
        -- stripo wildcard #
        topic = string.sub(topic, 1, -2)
    end
    pat = string.gsub(topic, "([^%w_/+])", "%%%1")
    pat = "^" .. string.gsub(pat, "%+", "[^/]*")
    if not tail then
        pat = pat .. "$"
    end
    return pat
end

MQ.Topics = {}
MQ.WildcardTopics = {}

function MQ.OnMessage (topic, payload)
    print("log: got msg(" .. topic .. ", " .. payload .. ")")
    cb = MQ.Topics[topic]
    if not cb then
        for _, pat_cb in pairs(MQ.WildcardTopics) do
            if string.find(topic, pat_cb[1]) then
                cb = pat_cb[2]
                break
            end
        end
        if not cb then
            -- no callback for this message
            return
        end
    end
    cb(topic, payload)
end

function MQ.Subscribe (topic, callback)
    if string.find(topic, "[+#]") then
        local pat = topic_to_pattern(topic)
        MQ.WildcardTopics[topic] = { pat, callback }
    else
        MQ.Topics[topic] = callback
    end
    print("sub|" .. topic)
end

function MQ.Unsubscribe (topic)
    print("unsub|" .. topic)
    MQ.Topics[topic] = nil
    MQ.WildcardTopics[topic] = nil
end

function MQ.Publish (topic, payload)
    print("pub|" .. topic .. "|" .. payload)
end

return MQ