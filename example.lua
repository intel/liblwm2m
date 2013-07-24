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

applications_object = {
	id = 2048,
	resources = {
		{ [0] = "ECLO",
		  [1] = "v1.0",
		  [2] = "**datamodel**",
		  [3] = 0 },
		{ [0] = "appname2",
		  [1] = "v2.0",
		  [2] = "**datamodel**",
		  [3] = 1 },
	}
}

--[[
applications_data_eclo_object = {
	id = 4096,
	resources = [
		{ [0] = 28.0,
		  [1] = 0.98 },
		  
		{ [0] = "appname2",
		  [1] = "v2.0",
		  [2] = "datamodelentlvoujson",
		  [3] = 1 },
	]
}
]]



objects = { [3] = device_object , [4] = { id = 4, resources = { [0] = 'salut' } }, [2048] = applications_object }

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
