#ifndef __MSGP_H__
#define __MSGP_H__

#include "msgpack.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t mp_get_size(msgpack_object* obj);

msgpack_object* mp_create_array();
void mp_push_nil(msgpack_object* obj);
void mp_push_bool(msgpack_object* obj, bool b);
void mp_push_int64(msgpack_object* obj, int64_t u);
void mp_push_double(msgpack_object* obj, double d);
void mp_push_string(msgpack_object* obj, const char* str, int str_len);
msgpack_object* mp_push_array(msgpack_object* obj);
msgpack_object* mp_push_map(msgpack_object* obj);
msgpack_object* mp_array_get_item(msgpack_object* obj, uint32_t index);
bool mp_array_get_bool(msgpack_object* obj, uint32_t index);
int64_t mp_array_get_int64(msgpack_object* obj, uint32_t index);
double mp_array_get_double(msgpack_object* obj, uint32_t index);
const char* mp_array_get_string(msgpack_object* obj, uint32_t index, uint32_t* len);

msgpack_object* mp_create_map();
void mp_set_nil(msgpack_object* obj, const char* key, int key_len);
void mp_set_bool(msgpack_object* obj, const char* key, int key_len, bool b);
void mp_set_int64(msgpack_object* obj, const char* key, int key_len, int64_t n);
void mp_set_double(msgpack_object* obj, const char* key, int key_len, double d);
void mp_set_string(msgpack_object* obj, const char* key, int key_len, const char* value, int value_len);
msgpack_object* mp_set_array(msgpack_object* obj, const char* key, int key_len);
msgpack_object* mp_set_map(msgpack_object* obj, const char* key, int key_len);
msgpack_object* mp_map_get_item(msgpack_object* obj, const char* key, uint32_t key_len);
bool mp_map_get_bool(msgpack_object* obj, const char* key, uint32_t key_len);
int64_t mp_map_get_int64(msgpack_object* obj, const char* key, uint32_t key_len);
double mp_map_get_double(msgpack_object* obj, const char* key, uint32_t key_len);
const char* mp_map_get_string(msgpack_object* obj, const char* key, uint32_t key_len, uint32_t* len);

void mp_destroy(msgpack_object* obj);

#ifdef __cplusplus
}
#endif

#endif
