//
// Created by liujie on 2020/5/31.
//

#ifndef MY_DB_JSON_H
#define MY_DB_JSON_H
#include <stdlib.h>

typedef enum{
    VALID_JSON,
    SYNTAX_ERROR,
    KEY_NOT_STRING,
    INVALID_VALUE,
    UNKNOWN_ERROR
}ValidateResult;

union value_data{
    int num;
    char* str;
};

typedef struct k_v{
    char* key;
    union value_data value;
}KV;

KV* new_kv();

void free_kv(KV*);

ValidateResult validate_all(char*);

ValidateResult validate_key(char*);

ValidateResult validate_value(char*);


ValidateResult parse(char* str,KV* k_v);



#endif //MY_DB_JSON_H
