package.cpath = package.cpath .. ';/Users/kartben/Repositories/liblwm2m/?.so;/Users/kartben/Repositories/luasocket/src/?.so'
package.path = package.path .. ';/Users/kartben/Repositories/liblwm2m/?.lua;/Users/kartben/Repositories/luasocket/src/?.lua'

lwm2m = require 'lwm2m'
socket = require 'socket'

udp = socket.udp()
udp:setsockname('*', 5683)

device_object = {
    id = 3,
    resources = {
        [0] = "Open Mobile Alliance",    -- manufacturer
        [1] = "Lightweight M2M Client",  -- model number
        [2] = "345000123",               -- serial number
        [3] = "1.0",                     -- firmware version
        [6] = 1,
        [7] = 5,
        [9] = 100,
        [10] = 15,
        [11] = 0,
        [13] = function() return os.time() end,
        [15] = "U"
    }
}

objects = { [3] = device_object , [4] = {} }

client = lwm2m.init(udp:getfd(), 'testlua', objects)
mt = { __index = lwm2m }
setmetatable (client, mt)

repeat
    data, ip, port, msg = udp:receivefrom()
    print(ip)
    print(port)
    print (data)
    if data then
        client:handle(data, ip, port)
    end

until not data
