//
// Created by liujie on 2020/5/31.
//
#include <string.h>
#include "json.h"

KV* new_kv(){
    KV* kv;
    kv=malloc(sizeof(KV));
    kv->key=NULL;
}

void free_kv(KV* kv){
    free(kv->key);
    free(kv->value.str);
    free(kv);
};

ValidateResult validate_all(char* all){
    if(all[0]!='{' || all[strlen(all)-1]!='}'){
        return SYNTAX_ERROR;
    }
    return VALID_JSON;
}

ValidateResult validate_key(char* key){
    if(key[0] > 64 && key[0] < 123){
        return VALID_JSON;
    }else{
        return KEY_NOT_STRING;
    }
}

ValidateResult validate_value(char* value){
    return VALID_JSON;
}

ValidateResult parse(char* str,KV* k_v){
    if(validate_all(str)!=VALID_JSON){
        return validate_all(str);
    };
    char key[strlen(str)],value[strlen(str)];
    int flag=0;
    for(int i=1,j=0;i<strlen(str)-1;i++){
        if(str[i]==':'){
            flag=1;
            continue;
        }
        if(flag==0){
            key[i-1]=str[i];
        }else{
            value[j]=str[i];
            j++;
        }
    }
    key[strlen(key)]='\0';
    value[strlen(value)]='\0';
    if(validate_key(key)!=VALID_JSON){
        return validate_key(key);
    }
    k_v->key=malloc(sizeof(strlen(key)));
    strcpy(k_v->key,key);
    if(validate_value(value)!=VALID_JSON){
        return validate_value(value);
    }
    k_v->value.str=malloc(sizeof(strlen(value)));
    strcpy(k_v->value.str,value);
    return VALID_JSON;
};
