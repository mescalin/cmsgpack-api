#include "msgp.h"
#include <stdlib.h>
#include <string.h>

static msgpack_object* _array_increase(msgpack_object_array* array) {
    array->size += 1;
    array->ptr = (msgpack_object*)realloc(array->ptr, array->size * sizeof(msgpack_object));
    msgpack_object* obj = array->ptr + (array->size - 1);
    memset(obj, 0, sizeof(msgpack_object));
    return obj;
}

static msgpack_object_kv* _map_increase(msgpack_object_map* map) {
    map->size += 1;
    map->ptr = (msgpack_object_kv*)realloc(map->ptr, map->size * sizeof(msgpack_object_kv));
    msgpack_object_kv* kv = map->ptr + (map->size - 1);
    memset(kv, 0, sizeof(msgpack_object_kv));
    return kv;
}

static void _set_str(msgpack_object* obj, const char* str, int str_len) {
    obj->type = MSGPACK_OBJECT_STR;
    obj->via.str.size = str_len;
    obj->via.str.ptr = (const char*)malloc(str_len);
    strncpy((char*)obj->via.str.ptr, str, str_len);
}

static void _destroy(msgpack_object obj) {
    switch(obj.type) {
    case MSGPACK_OBJECT_NIL:
    case MSGPACK_OBJECT_BOOLEAN:
    case MSGPACK_OBJECT_POSITIVE_INTEGER:
    case MSGPACK_OBJECT_NEGATIVE_INTEGER:
    case MSGPACK_OBJECT_DOUBLE:
        break;
    case MSGPACK_OBJECT_STR: {
        free((void*)obj.via.str.ptr);
        break;
    }
    case MSGPACK_OBJECT_BIN: {
        free((void*)obj.via.bin.ptr);
        break;
    }
    case MSGPACK_OBJECT_EXT: {
        free((void*)obj.via.ext.ptr);
        break;
    }
    case MSGPACK_OBJECT_ARRAY: {
        int n = 0;
        for(; n < obj.via.array.size; ++n)
            _destroy(obj.via.array.ptr[n]);
        free(obj.via.array.ptr);
        break;
    }
    case MSGPACK_OBJECT_MAP: {
        int n = 0;
        for(; n < obj.via.map.size; ++n) {
            _destroy(obj.via.map.ptr[n].key);
            _destroy(obj.via.map.ptr[n].val);
        }
        free(obj.via.map.ptr);
        break;
    }
    }
}

uint32_t mp_get_size(msgpack_object* obj) {
    switch(obj->type) {
    case MSGPACK_OBJECT_ARRAY:
        return obj->via.array.size;
    case MSGPACK_OBJECT_MAP:
        return obj->via.map.size;
    case MSGPACK_OBJECT_STR:
        return obj->via.str.size;
    case MSGPACK_OBJECT_BIN:
        return obj->via.bin.size;
    case MSGPACK_OBJECT_EXT:
        return obj->via.ext.size;
    }
    return 0;
}

//--------------------------------------------------- array ---------------------------------------------------
msgpack_object* mp_create_array() {
    msgpack_object* obj = (msgpack_object*)malloc(sizeof(msgpack_object));
    memset(obj, 0, sizeof(msgpack_object));
    obj->type = MSGPACK_OBJECT_ARRAY;
    return obj;
}

void mp_push_nil(msgpack_object* obj) {
    if(obj->type != MSGPACK_OBJECT_ARRAY)
        return;
    msgpack_object* elt = _array_increase(&obj->via.array);
    elt->type = MSGPACK_OBJECT_NIL;
}

void mp_push_bool(msgpack_object* obj, bool b) {
    if(obj->type != MSGPACK_OBJECT_ARRAY)
        return;
    msgpack_object* elt = _array_increase(&obj->via.array);
    elt->type = MSGPACK_OBJECT_BOOLEAN;
    elt->via.boolean = b;
}

void mp_push_int64(msgpack_object* obj, int64_t n) {
    if(obj->type != MSGPACK_OBJECT_ARRAY)
        return;
    msgpack_object* elt = _array_increase(&obj->via.array);
    elt->type = MSGPACK_OBJECT_NEGATIVE_INTEGER;
    elt->via.i64 = n;
}

void mp_push_double(msgpack_object* obj, double d) {
    if(obj->type != MSGPACK_OBJECT_ARRAY)
        return;
    msgpack_object* elt = _array_increase(&obj->via.array);
    elt->type = MSGPACK_OBJECT_DOUBLE;
    elt->via.dec = d;
}

void mp_push_string(msgpack_object* obj, const char* str, int str_len) {
    if(obj->type != MSGPACK_OBJECT_ARRAY)
        return;
    msgpack_object* elt = _array_increase(&obj->via.array);
    _set_str(elt, str, str_len);
}

msgpack_object* mp_push_array(msgpack_object* obj) {
    if(obj->type != MSGPACK_OBJECT_ARRAY)
        return;
    msgpack_object* elt = _array_increase(&obj->via.array);
    elt->type = MSGPACK_OBJECT_ARRAY;
    return elt;
}

msgpack_object* mp_push_map(msgpack_object* obj) {
    if(obj->type != MSGPACK_OBJECT_ARRAY)
        return;
    msgpack_object* elt = _array_increase(&obj->via.array);
    elt->type = MSGPACK_OBJECT_MAP;
    return elt;
}

msgpack_object* mp_array_get_item(msgpack_object* obj, uint32_t index) {
    if(obj->type != MSGPACK_OBJECT_ARRAY)
        return NULL;
    if(index < 0 || index > obj->via.array.size)
        return NULL;
    return obj->via.array.ptr + index;
}

bool mp_array_get_bool(msgpack_object* obj, uint32_t index) {
    msgpack_object* elt = mp_array_get_item(obj, index);
    if(elt != NULL && elt->type == MSGPACK_OBJECT_BOOLEAN) {
        return elt->via.boolean;
    }
    return false;
}

int64_t mp_array_get_int64(msgpack_object* obj, uint32_t index) {
    msgpack_object* elt = mp_array_get_item(obj, index);
    if(elt != NULL) {
        if(elt->type == MSGPACK_OBJECT_POSITIVE_INTEGER || elt->type == MSGPACK_OBJECT_NEGATIVE_INTEGER)
            return elt->via.i64;
    }
    return 0;
}

double mp_array_get_double(msgpack_object* obj, uint32_t index) {
    msgpack_object* elt = mp_array_get_item(obj, index);
    if(elt != NULL && elt->type == MSGPACK_OBJECT_DOUBLE) {
        return elt->via.dec;
    }
    return 0;
}

const char* mp_array_get_string(msgpack_object* obj, uint32_t index, uint32_t* len) {
    msgpack_object* elt = mp_array_get_item(obj, index);
    if(elt != NULL && elt->type == MSGPACK_OBJECT_STR) {
        *len = elt->via.str.size;
        return elt->via.str.ptr;
    }
    *len = 0;
    return NULL;
}

//--------------------------------------------------- map ---------------------------------------------------
static msgpack_object_kv* _map_find(msgpack_object_map* map, const char* key, int key_len)  {
    int n = 0;
    for(; n < map->size; ++n) {
        msgpack_object_kv* kv = map->ptr + n;
        if(kv->key.type == MSGPACK_OBJECT_STR) {
            if(strncmp(key, kv->key.via.str.ptr, key_len) == 0)
                return kv;
        }
    }
    return NULL;
}

msgpack_object* mp_create_map() {
    msgpack_object* obj = (msgpack_object*)malloc(sizeof(msgpack_object));
    memset(obj, 0, sizeof(msgpack_object));
    obj->type = MSGPACK_OBJECT_MAP;
    return obj;
}

void mp_set_nil(msgpack_object* obj, const char* key, int key_len) {
    if(obj->type != MSGPACK_OBJECT_MAP)
        return;
    msgpack_object_kv* kv = _map_find(&obj->via.map, key, key_len);
    if(kv != NULL) {
        _destroy(kv->val);
    } else {
        kv = _map_increase(&obj->via.map);
        _set_str(&kv->key, key, key_len);
    }
    kv->val.type = MSGPACK_OBJECT_NIL; //set nil val
}

void mp_set_bool(msgpack_object* obj, const char* key, int key_len, bool b) {
    if(obj->type != MSGPACK_OBJECT_MAP)
        return;
    msgpack_object_kv* kv = _map_find(&obj->via.map, key, key_len);
    if(kv != NULL) {
        _destroy(kv->val);
    } else {
        kv = _map_increase(&obj->via.map);
        _set_str(&kv->key, key, key_len);
    }
    kv->val.type = MSGPACK_OBJECT_BOOLEAN; //set bool val
    kv->val.via.boolean = b;
}

void mp_set_int64(msgpack_object* obj, const char* key, int key_len, int64_t n) {
    if(obj->type != MSGPACK_OBJECT_MAP)
        return;
    msgpack_object_kv* kv = _map_find(&obj->via.map, key, key_len);
    if(kv != NULL) {
        _destroy(kv->val);
    } else {
        kv = _map_increase(&obj->via.map);
        _set_str(&kv->key, key, key_len);
    }
    kv->val.type = MSGPACK_OBJECT_NEGATIVE_INTEGER; //set int val
    kv->val.via.i64 = n;
}

void mp_set_double(msgpack_object* obj, const char* key, int key_len, double d) {
    if(obj->type != MSGPACK_OBJECT_MAP)
        return;
    msgpack_object_kv* kv = _map_find(&obj->via.map, key, key_len);
    if(kv != NULL) {
        _destroy(kv->val);
    } else {
        kv = _map_increase(&obj->via.map);
        _set_str(&kv->key, key, key_len);
    }
    kv->val.type = MSGPACK_OBJECT_DOUBLE; //set double val
    kv->val.via.dec = d;
}

void mp_set_string(msgpack_object* obj, const char* key, int key_len, const char* value, int value_len) {
    if(obj->type != MSGPACK_OBJECT_MAP)
        return;
    msgpack_object_kv* kv = _map_find(&obj->via.map, key, key_len);
    if(kv != NULL) {
        _destroy(kv->val);
    } else {
        kv = _map_increase(&obj->via.map);
        _set_str(&kv->key, key, key_len);
    }
    _set_str(&kv->val, value, value_len); //set str val
}

msgpack_object* mp_set_array(msgpack_object* obj, const char* key, int key_len) {
    if(obj->type != MSGPACK_OBJECT_MAP)
        return NULL;
    msgpack_object_kv* kv = _map_find(&obj->via.map, key, key_len);
    if(kv != NULL) {
        _destroy(kv->val);
    } else {
        kv = _map_increase(&obj->via.map);
        _set_str(&kv->key, key, key_len);
    }
    kv->val.type = MSGPACK_OBJECT_ARRAY; //set array val
    return &kv->val;
}

msgpack_object* mp_set_map(msgpack_object* obj, const char* key, int key_len) {
    if(obj->type != MSGPACK_OBJECT_MAP)
        return NULL;
    msgpack_object_kv* kv = _map_find(&obj->via.map, key, key_len);
    if(kv != NULL) {
        _destroy(kv->val);
    } else {
        kv = _map_increase(&obj->via.map);
        _set_str(&kv->key, key, key_len);
    }
    kv->val.type = MSGPACK_OBJECT_MAP; //set map val
    return &kv->val;
}

msgpack_object* mp_map_get_item(msgpack_object* obj, const char* key, uint32_t key_len) {
    if(obj->type != MSGPACK_OBJECT_MAP)
        return NULL;
    int n = 0;
    for(; n < obj->via.map.size; ++n) {
        msgpack_object_kv* kv = obj->via.map.ptr + n;
        if(kv->key.type == MSGPACK_OBJECT_STR) {
            if(strncmp(key, kv->key.via.str.ptr, key_len) == 0)
                return &kv->val;
        }
    }
    return NULL;
}

bool mp_map_get_bool(msgpack_object* obj, const char* key, uint32_t key_len) {
    msgpack_object* elt = mp_map_get_item(obj, key, key_len);
    if(elt != NULL && elt->type == MSGPACK_OBJECT_BOOLEAN) {
        return elt->via.boolean;
    }
    return false;
}

int64_t mp_map_get_int64(msgpack_object* obj, const char* key, uint32_t key_len) {
    msgpack_object* elt = mp_map_get_item(obj, key, key_len);
    if(elt != NULL) {
        if(elt->type == MSGPACK_OBJECT_POSITIVE_INTEGER || elt->type == MSGPACK_OBJECT_NEGATIVE_INTEGER)
            return elt->via.i64;
    }
    return 0;
}

double mp_map_get_double(msgpack_object* obj, const char* key, uint32_t key_len) {
    msgpack_object* elt = mp_map_get_item(obj, key, key_len);
    if(elt != NULL && elt->type == MSGPACK_OBJECT_DOUBLE) {
        return elt->via.dec;
    }
    return 0;
}

const char* mp_map_get_string(msgpack_object* obj, const char* key, uint32_t key_len, uint32_t* len) {
    msgpack_object* elt = mp_map_get_item(obj, key, key_len);
    if(elt != NULL && elt->type == MSGPACK_OBJECT_STR) {
        *len = elt->via.str.size;
        return elt->via.str.ptr;
    }
    *len = 0;
    return NULL;
}

void mp_destroy(msgpack_object* obj) {
    _destroy(*obj);
    free(obj);
}
