//
// Created by liujie on 2020/3/18.
//

#ifndef MY_DB_SCHEMA_CREATOR_H
#define MY_DB_SCHEMA_CREATOR_H

#define MAX_FIELDS_COUNT 5

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
    Field **fields;
}Schema;

Schema* _create_schema(Field** pField);

Schema* create_schema(char[]);

Field* _new_field(char *json_field);

Field* new_field();

void close_field(Field* field);

Field** _new_fields(char *json_fields);

Schema* new_schema();

int append_field(Schema*,Field*);

void close_schema(Schema* schema);

#endif //MY_DB_SCHEMA_CREATOR_H
