/*
 Copyright (c) 2013, Sierra Wireless

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 Benjamin Cab������������������ <bcabe@sierrawireless.com>

 */

#include "../core/liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#define PRV_TLV_BUFFER_SIZE 64

typedef struct _prv_instance_ {
	struct _prv_instance_ * next;   // matches lwm2m_list_t::next
	uint16_t shortID;               // matches lwm2m_list_t::id
	uint8_t test;
} prv_instance_t;

static uint8_t prv_read(lwm2m_uri_t * uriP, char ** bufferP, int * lengthP,
		lwm2m_object_t * objectP) {
	prv_instance_t * targetP;

	*bufferP = NULL;
	*lengthP = 0;

	lua_State* L = (lua_State*) objectP->userData;

	lua_pushinteger(L, objectP->objID);
	lua_gettable(L, -2);

	printf("prv_read: objID: %d, objInstance: %d, resID: %d \n", uriP->objID,
			uriP->objInstance, uriP->resID);

	lua_getfield(L, -1, "resources");

	if (LWM2M_URI_NOT_DEFINED == uriP->objInstance) {
		// TODO: TLV

		// prv_output_buffer(*bufferP, *lengthP);
		return COAP_205_CONTENT ;
	} else {
		int objInstance = uriP->objInstance;
		int resId = uriP->resID;

		lua_pushinteger(L, resId);
		lua_gettable(L, -2);

		if (lua_isnil(L, -1)) {
			return COAP_404_NOT_FOUND ;
		} else if (lua_isfunction(L, -1)) {
			lua_call(L, 0, 1);
		}

		*bufferP = strdup(lua_tostring(L, -1));
		if (NULL != *bufferP) {
			*lengthP = strlen(*bufferP);
			return COAP_205_CONTENT ;
		}

		return COAP_404_NOT_FOUND ;
	}
}

static uint8_t prv_write(lwm2m_uri_t * uriP, char * buffer, int length,
		lwm2m_object_t * objectP) {
	return COAP_501_NOT_IMPLEMENTED ;
}

static uint8_t prv_create(uint16_t id, char * buffer, int length,
		lwm2m_object_t * objectP) {
	return COAP_501_NOT_IMPLEMENTED ;
}

static uint8_t prv_delete(uint16_t id, lwm2m_object_t * objectP) {
	return COAP_501_NOT_IMPLEMENTED ;
}

lwm2m_object_t * get_lua_object(int objId, lua_State *luaState) {
	lwm2m_object_t * luaObj;

	luaObj = (lwm2m_object_t *) malloc(sizeof(lwm2m_object_t));

	if (NULL != luaObj) {
		int i;
		prv_instance_t * targetP;

		memset(luaObj, 0, sizeof(lwm2m_object_t));

		luaObj->objID = objId;
		luaObj->readFunc = prv_read;
		luaObj->writeFunc = prv_write;
		luaObj->createFunc = prv_create;
		luaObj->deleteFunc = prv_delete;
		luaObj->userData = luaState;
	}

	return luaObj;
}
