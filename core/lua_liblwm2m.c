#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "liblwm2m.h"

extern lwm2m_object_t * get_lua_object(int objectId, lua_State *L);

/**
 * inits a lwm2m context and returns a Lua object { ctx = lwm2m_context_t_USERDATA , objects = [objlist] }
 */
static int _init(lua_State *L) {
	// lwm2m_context_t * lwm2m_init(int socket, char * endpointName, uint16_t numObject, lwm2m_object_t * objectList[]);
	int socket = luaL_checkint(L, 1);
	const char* endpointName = luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);

	lwm2m_object_t * objArray[10];
	int nbObjects = 0;

	// for each object in "objects" table
	lua_pushvalue(L, 3);
	lua_pushnil(L);
	while (lua_next(L, -2)) {
		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue(L, -2);
		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		int key = lua_tointeger(L, -1);
		luaL_checktype(L, -2, LUA_TTABLE); // value should be the object table

//		lua_pushvalue(L, -2);
//		lua_getfield(L, -3, "id")

		const char *value = lua_tostring(L, -2);

		objArray[nbObjects] = get_lua_object(key, L);
		nbObjects++;

		// pop value + copy of key, leaving original key
		lua_pop(L, 2);
		// stack now contains: -1 => key; -2 => table
	}
	lua_pop(L, 1);

	lwm2m_context_t * lwm2mH = lwm2m_init(socket, endpointName, nbObjects,
			objArray);

	lua_newtable(L);
	lua_pushlightuserdata(L, lwm2mH);
	lua_setfield(L, -2, "ctx");
	lua_pushvalue(L, 3); // objects table is the 3rd parameter of the init() fct
	lua_setfield(L, -2, "objects");

	return 1;
}

static int _handle(lua_State *L) {
	// int lwm2m_handle_packet(lwm2m_context_t * contextP, uint8_t * buffer, int length, int socket, struct sockaddr * fromAddr, socklen_t fromAddrLen);
	lua_getfield(L, 1, "ctx");
	lwm2m_context_t *lwContext = (lwm2m_context_t *) lua_touserdata(L, -1);
	int socket = lwContext->socket;

	char* data = luaL_checkstring(L,2);
	uint8_t * buffer = (uint8_t*) data;
	int length = strlen(data);

	char* ip = luaL_checkstring(L,3);
	int port = luaL_checkint(L,4);

	struct sockaddr_in sa;
	socklen_t addrLen = sizeof(sa);

	printf("Handle for client '%s' \n", lwContext->endpointName);
	printf("Buffer = '%s' of length=%d \n", data, length);

	sa.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(sa.sin_addr));
	sa.sin_port = htons(port);

	// objects table should be put at the top of the stack
	lua_getfield(L, 1, "objects");

	// lua_pushinteger(L, 3);
	// lua_gettable(L, -2);

	// at this point, the element at the top of the lua stack is the 'objects' table

	lwm2m_handle_packet(lwContext, buffer, length, socket,
			(struct sockaddr *) &sa, addrLen);

	return 0;
}

static const struct luaL_Reg mylib[] = { { "init", _init },
		{ "handle", _handle }, { NULL, NULL } /* sentinel */
};

int luaopen_lwm2m(lua_State *L) {
	luaL_newlib(L, mylib); /* register C functions with Lua */
	return 1;
}
