/* The MIT License (MIT)
 *
 * Copyright (c) 2023 Stefano Trettel
 *
 * Software repository: MoonSDL2, https://github.com/stetre/moonsdl2
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "internal.h"

SDL_SensorType checksensortypeorint(lua_State *L, int arg)
    {
    if(lua_isinteger(L, arg))
        return lua_tointeger(L, arg);
    return checksensortype(L, arg);
    }

void pushsensortypeorint(lua_State *L, SDL_SensorType type)
    {
    switch(type)
        {
        case SDL_SENSOR_INVALID:
        case SDL_SENSOR_UNKNOWN:
        case SDL_SENSOR_ACCEL:
        case SDL_SENSOR_GYRO: pushsensortype(L, type); return;
        default: break;
        }
    lua_pushinteger(L, type);
    }

static int freesensor(lua_State *L, ud_t *ud)
    {
    sensor_t *sensor = (sensor_t*)ud->handle;
//  freechildren(L, _MT, ud);
    if(!freeuserdata(L, ud, "sensor")) return 0;
    SDL_SensorClose(sensor);
    return 0;
    }

static int newsensor(lua_State *L, sensor_t *sensor)
    {
    ud_t *ud;
    ud = newuserdata(L, sensor, SENSOR_MT, "sensor");
    ud->parent_ud = NULL;
    ud->destructor = freesensor;
    return 1;
    }

static int SensorOpen(lua_State *L)
    {
    int device_index = checkindex(L, 1);
    sensor_t *sensor = SDL_SensorOpen(device_index);
    if(sensor==NULL) GetError(L);
    if(UD(sensor))
        pushsensor(L, sensor);
    else 
        newsensor(L, sensor);
    return 1;
    }

static int SensorFromInstanceID(lua_State *L)
    {
    int instance_id = luaL_checkinteger(L, 1);
    sensor_t *sensor = SDL_SensorFromInstanceID(instance_id);
    if(sensor==NULL) GetError(L);
    if(UD(sensor))
        pushsensor(L, sensor);
    else 
        newsensor(L, sensor);
    return 1;
    }

static int GetName(lua_State *L)
    {
    sensor_t *sensor = checksensor(L, 1, NULL);
    const char *name = SDL_SensorGetName(sensor);
    if(!name) return 0;
    lua_pushstring(L, name);
    return 1;
    }

static int GetType(lua_State *L)
    {
    sensor_t *sensor = checksensor(L, 1, NULL);
    pushsensortypeorint(L, SDL_SensorGetType(sensor));
    return 1;
    }

static int GetNonPortableType(lua_State *L)
    {
    sensor_t *sensor = checksensor(L, 1, NULL);
    pushsensortypeorint(L, SDL_SensorGetNonPortableType(sensor));
    return 1;
    }

static int GetInstanceID(lua_State *L)
    {
    sensor_t *sensor = checksensor(L, 1, NULL);
    SDL_SensorID id = SDL_SensorGetInstanceID(sensor);
    if(id==-1) return 0;
    lua_pushinteger(L, id);
    return 1;
    }

static int GetData(lua_State *L)
    {
    int ec;
    sensor_t *sensor = checksensor(L, 1, NULL);
    int num_values = luaL_checkinteger(L, 2);
    float *data = Malloc(L, sizeof(float)*num_values);
    uint64_t *timestamp = MallocNoErr(L, sizeof(uint64_t)*num_values);
    if(!timestamp) { Free(L, data); return errmemory(L); }
//  ec = SDL_SensorGetData(sensor, data, num_values);
    ec = SDL_SensorGetDataWithTimestamp(sensor, timestamp, data, num_values);
    if(ec)
        { Free(L, data); Free(L, timestamp); GetError(L); }
    lua_newtable(L); // data
    lua_newtable(L); // timestamp
    for(int i=0; i<num_values; i++)
        {
        lua_pushnumber(L, data[i]);
        lua_rawseti(L, -3, i+1);
        lua_pushinteger(L, timestamp[i]); // usec
        lua_rawseti(L, -2, i+1);
        }
    Free(L, data);
    Free(L, timestamp);
    return 2;
    }

static int SensorGetDeviceType(lua_State *L)
    {
    int device_index = checkindex(L, 1);
    pushsensortypeorint(L, SDL_SensorGetDeviceType(device_index));
    return 1;
    }

static int SensorGetDeviceNonPortableType(lua_State *L)
    {
    int device_index = checkindex(L, 1);
    pushsensortypeorint(L, SDL_SensorGetDeviceNonPortableType(device_index));
    return 1;
    }

static int SensorGetDeviceInstanceID(lua_State *L)
    {
    int device_index = checkindex(L, 1);
    SDL_SensorID id = SDL_SensorGetDeviceInstanceID(device_index);
    if(id==-1) return 0;
    lua_pushinteger(L, id);
    return 1;
    }

#if 0
static int LockSensors(lua_State *L)
    {
    (void)L;
    SDL_LockSensors();
    return 0;
    }

static int UnlockSensors(lua_State *L)
    {
    (void)L;
    SDL_UnlockSensors();
    return 0;
    }
#endif

static int NumSensors(lua_State *L)
    {
    lua_pushinteger(L, SDL_NumSensors());
    return 1;
    }

static int SensorUpdate(lua_State *L)
    {
    (void)L;
    SDL_SensorUpdate();
    return 0;
    }

static int SensorGetDeviceName(lua_State *L)
    {
    int device_index = checkindex(L, 1);
    const char *name = SDL_SensorGetDeviceName(device_index);
    if(!name) return 0;
    lua_pushstring(L, name);
    return 1;
    }

DESTROY_FUNC(sensor)

static const struct luaL_Reg Methods[] = 
    {
        { "close", Destroy },
        { "get_name", GetName },
        { "get_type", GetType },
        { "get_non_portable_type", GetNonPortableType },
        { "get_id", GetInstanceID },
        { "get_data", GetData },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Destroy },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "sensor_open", SensorOpen },
        { "sensor_open_from_id", SensorFromInstanceID },
        { "sensor_id", SensorGetDeviceInstanceID },
        { "sensor_type", SensorGetDeviceType },
        { "sensor_non_portable_type", SensorGetDeviceNonPortableType },
//      { "lock_sensors", LockSensors },
//      { "unlock_sensors", UnlockSensors },
        { "num_sensors", NumSensors },
        { "sensor_update", SensorUpdate },
        { "sensor_name", SensorGetDeviceName },
        { NULL, NULL } /* sentinel */
    };

void moonsdl2_open_sensor(lua_State *L)
    {
    udata_define(L, SENSOR_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
#define ADD(what)   do { lua_pushinteger(L, SDL_##what); lua_setfield(L, -2, #what); } while(0)
    ADD(STANDARD_GRAVITY);
    //SDL_SensorType (keep as integer because custom values follow) 
    ADD(SENSOR_INVALID);
    ADD(SENSOR_UNKNOWN);
    ADD(SENSOR_ACCEL);
    ADD(SENSOR_GYRO);
#undef ADD
    }

