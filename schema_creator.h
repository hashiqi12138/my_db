//
// Created by liujie on 2020/3/18.
//

#ifndef MY_DB_SCHEMA_CREATOR_H
#define MY_DB_SCHEMA_CREATOR_H

#define MAX_FIELDS_COUNT 10

typedef enum{
    INT,
    STRING,
    FLOAT
}FieldType;
typedef struct _field{
    char* name;
    FieldType type;
}Field;

typedef struct _schema{
    int field_count;
    Field* fields;
    char *name;
}Schema;

typedef enum{
    APPEND_SUCCESS,
    APPEND_FULL_ERROR,
}AppendResult;


Schema* create_schema(char[]);

Field* _new_field(char *json_field);

Field* new_field();

void free_field(Field* field);

Field* create_field(char* json_str);

void close_field(Field* field);

Schema* new_schema();

AppendResult append_field(Schema*,Field*);

void close_schema(Schema* schema);

void print_schema(Schema* schema);

#endif //MY_DB_SCHEMA_CREATOR_H
