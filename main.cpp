#include "msgp.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

void TestMsgpack1() {
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    msgpack_object* root = mp_create_map();
    string key("name");
    string val("jack");
    mp_set_string(root, key.c_str(), key.size(), val.c_str(), val.size());
    val = "rose";
    mp_set_string(root, key.c_str(), key.size(), val.c_str(), val.size());

    key = "format";
    msgpack_object* format = mp_set_array(root, key.c_str(), key.size());
    format = mp_set_map(root, key.c_str(), key.size());
    key = "type";
    val = "rect";
    mp_set_string(format, key.c_str(), key.size(), val.c_str(), val.size());
    key = "width";
    mp_set_int64(format, key.c_str(), key.size(), 1920);
    key = "height";
    mp_set_int64(format, key.c_str(), key.size(), 1080);
    key = "interface";
    mp_set_bool(format, key.c_str(), key.size(), false);

    msgpack_pack_object(&pk, *root);
    printf("\nsbuf size=%d\n", int(sbuf.size));

    msgpack_zone mempool;
    msgpack_zone_init(&mempool, 2048);

    msgpack_object deserialized;
    msgpack_unpack(sbuf.data, sbuf.size, NULL, &mempool, &deserialized);

    /* print the deserialized object. */
    msgpack_object_print(stdout, deserialized);
    puts("");

    { //get items
        key = "name";
        uint32_t len = 0;
        const char* val = mp_map_get_string(&deserialized, key.c_str(), key.size(), &len);
        printf("%s=%s\n", key.c_str(), string(val, len).c_str());

        key = "format";
        msgpack_object* format = mp_map_get_item(&deserialized, key.c_str(), key.size());
        if(format != NULL) {
            key = "type";
            len = 0;
            val = mp_map_get_string(format, key.c_str(), key.size(), &len);
            printf("%s=%s\n", key.c_str(), string(val, len).c_str());

            key = "width";
            printf("%s=%ld\n", key.c_str(), mp_map_get_int64(format, key.c_str(), key.size()));

            key = "height";
            printf("%s=%ld\n", key.c_str(), mp_map_get_int64(format, key.c_str(), key.size()));

            key = "interface";
            printf("%s=%d\n", key.c_str(), mp_map_get_bool(format, key.c_str(), key.size()));
        }
    }
    mp_destroy(root);
    root = NULL;

    msgpack_zone_destroy(&mempool);
    msgpack_sbuffer_destroy(&sbuf);
}

void TestMsgpack2() {
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    msgpack_object* root = mp_create_array();
    string strs[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    for(int n = 0; n < 7; ++n)
        mp_push_string(root, strs[n].c_str(), strs[n].size());
    msgpack_pack_object(&pk, *root);
    printf("\nsbuf size=%d\n", int(sbuf.size));

    msgpack_zone mempool;
    msgpack_zone_init(&mempool, 2048);

    msgpack_object deserialized;
    msgpack_unpack(sbuf.data, sbuf.size, NULL, &mempool, &deserialized);

    /* print the deserialized object. */
    msgpack_object_print(stdout, deserialized);
    puts("");

    {
        for(int n = 0; n < mp_get_size(&deserialized); ++n) {
            uint32_t len = 0;
            const char* val = mp_array_get_string(&deserialized, n, &len);
            printf("array[%d]=%s\n", n, string(val, len).c_str());
        }
    }
    mp_destroy(root);
    root = NULL;

    msgpack_zone_destroy(&mempool);
    msgpack_sbuffer_destroy(&sbuf);
}

void TestMsgpack3() {
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    msgpack_object* root = mp_create_array();
    int xx[3][3] = {{2, -1, 0}, {1, -10, 0}, {0, 0, 1}};
    for(int n = 0; n < 3; ++n) {
        msgpack_object* elt = mp_push_array(root);
        for(int m = 0; m < 3; ++m) {
            mp_push_int64(elt, xx[n][m]);
        }
    }
    msgpack_pack_object(&pk, *root);
    printf("\nsbuf size=%d\n", int(sbuf.size));

    msgpack_zone mempool;
    msgpack_zone_init(&mempool, 2048);

    msgpack_object deserialized;
    msgpack_unpack(sbuf.data, sbuf.size, NULL, &mempool, &deserialized);

    /* print the deserialized object. */
    msgpack_object_print(stdout, deserialized);
    puts("");

    for(int n = 0; n < mp_get_size(&deserialized); ++n) {
        msgpack_object* elt = mp_array_get_item(&deserialized, n);
        for(int m = 0; m < mp_get_size(elt); ++m) {
            printf("array[%d][%d] = %ld\n", n, m, mp_array_get_int64(elt, m));
        }
    }
    mp_destroy(root);
    root = NULL;

    msgpack_zone_destroy(&mempool);
    msgpack_sbuffer_destroy(&sbuf);
}

void TestMsgpack4() {
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    msgpack_object* root = mp_create_map();
    string key = "image";
    msgpack_object* images = mp_set_map(root, key.c_str(), key.size());
    key = "width";
    mp_set_int64(images, key.c_str(), key.size(), 800);
    key = "height";
    mp_set_int64(images, key.c_str(), key.size(), 600);
    key = "title";
    string value = "View from 15th Floor";
    mp_set_string(images, key.c_str(), key.size(), value.c_str(), value.size());

    key = "thumbnail";
    msgpack_object* thumb = mp_set_map(images, key.c_str(), key.size());
    key = "url";
    value = "http:/*www.example.com/image/481989943";
    mp_set_string(thumb, key.c_str(), key.size(), value.c_str(), value.size());
    key = "width";
    mp_set_int64(thumb, key.c_str(), key.size(), 125);
    key = "height";
    value = "100";
    mp_set_string(thumb, key.c_str(), key.size(), value.c_str(), value.size());

    key = "ids";
    msgpack_object* ids = mp_set_array(images, key.c_str(), key.size());
    int xx[] = {116, 943, 234, 38793};
    for(int n = 0; n < 4; ++n)
        mp_push_int64(ids, xx[n]);

    msgpack_pack_object(&pk, *root);
    printf("\nsbuf size=%d\n", int(sbuf.size));

    msgpack_zone mempool;
    msgpack_zone_init(&mempool, 2048);

    msgpack_object deserialized;
    msgpack_unpack(sbuf.data, sbuf.size, NULL, &mempool, &deserialized);

    /* print the deserialized object. */
    msgpack_object_print(stdout, deserialized);
    puts("");

    {
        string key("image");
        msgpack_object* image = mp_map_get_item(&deserialized, key.c_str(), key.size());
        key = "width";
        printf("%s=%ld\n", key.c_str(), mp_map_get_int64(image, key.c_str(), key.size()));
        key = "height";
        printf("%s=%ld\n", key.c_str(), mp_map_get_int64(image, key.c_str(), key.size()));
        key = "title";
        {
            uint32_t len = 0;
            const char* title = mp_map_get_string(image, key.c_str(), key.size(), &len);
            string value(title, len);
            printf("%s=%s\n", key.c_str(), value.c_str());
        }
        key = "ids";
        msgpack_object* ids = mp_map_get_item(image, key.c_str(), key.size());
        for(int n = 0; n < mp_get_size(ids); ++n)
            printf("%s[%d]=%ld\n", key.c_str(), n, mp_array_get_int64(ids, n));
        key = "thumbnail";
        msgpack_object* thumb = mp_map_get_item(image, key.c_str(), key.size());
        key = "url";
        {
            uint32_t len = 0;
            const char* url = mp_map_get_string(thumb, key.c_str(), key.size(), &len);
            string value(url, len);
            printf("%s=%s\n", key.c_str(), value.c_str());
            key = "width";
            printf("%s=%ld\n", key.c_str(), mp_map_get_int64(image, key.c_str(), key.size()));
            key = "height";
            printf("%s=%ld\n", key.c_str(), mp_map_get_int64(image, key.c_str(), key.size()));
        }

    }
    mp_destroy(root);
    root = NULL;

    msgpack_zone_destroy(&mempool);
    msgpack_sbuffer_destroy(&sbuf);
}

void TestMsgpack5() {
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    msgpack_object* root = mp_create_array();
    for(int n = 0; n < 3; ++n) {
        msgpack_object* elt = mp_push_map(root);
        string key = "precision";
        string value = "zip";
        mp_set_string(elt, key.c_str(), key.size(), value.c_str(), value.size());

        key = "Latitude";
        mp_set_double(elt, key.c_str(), key.size(), 37.766800);
        key = "Longitude";
        mp_set_double(elt, key.c_str(), key.size(), -122.395900);
        key = "Address";
        value = "";
        mp_set_string(elt, key.c_str(), key.size(), value.c_str(), value.size());
    }

    msgpack_pack_object(&pk, *root);
    printf("\nsbuf size=%d\n", int(sbuf.size));

    msgpack_zone mempool;
    msgpack_zone_init(&mempool, 2048);

    msgpack_object deserialized;
    msgpack_unpack(sbuf.data, sbuf.size, NULL, &mempool, &deserialized);

    /* print the deserialized object. */
    msgpack_object_print(stdout, deserialized);
    puts("");
    {
        for(int n = 0; n < mp_get_size(&deserialized); ++n) {
            msgpack_object* obj = mp_array_get_item(&deserialized, n);
            string key("precision");
            uint32_t len = 0;
            const char* val = mp_map_get_string(obj, key.c_str(), key.size(), &len);
            {
                string value(val, len);
                printf("%s=%s\n", key.c_str(), value.c_str());
            }
            key = "Address";
            val = mp_map_get_string(obj, key.c_str(), key.size(), &len);
            {
                string value(val, len);
                printf("%s=%s\n", key.c_str(), value.c_str());
            }
            key = "Latitude";
            printf("%s=%f\n", key.c_str(), mp_map_get_double(obj, key.c_str(), key.size()));
            key = "Longitude";
            printf("%s=%f\n", key.c_str(), mp_map_get_double(obj, key.c_str(), key.size()));
        }
    }
    mp_destroy(root);
    root = NULL;

    msgpack_zone_destroy(&mempool);
    msgpack_sbuffer_destroy(&sbuf);
}

int main(int argc, const char** argv) {
    TestMsgpack5();
    TestMsgpack4();
    TestMsgpack3();
    TestMsgpack2();
    TestMsgpack1();
}
