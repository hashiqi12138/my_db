//
// Created by liujie on 2020/3/18.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "schema_creator.h"

Field* new_field(){
    Field* field=malloc(sizeof(Field));
    field->name=malloc(sizeof(char));
    field->type=INT;
    return field;
}

void close_field(Field* field){
    free(field->name);
    free(field);
}

Field* create_field(char* json_str){

};


void close_schema(Schema* schema){
    for(int i =0;i<schema->field_count;i++){
        free(schema->fields[i].name);
        //printf("free %s\n",*&(schema->fields[i]));
        //close_field(&(schema->fields[i]));
    }
    free(schema->fields);
    free(schema);
};

Schema* new_schema(){
    Schema* schema=malloc(sizeof(Schema));
    schema->fields=NULL;
    schema->field_count=0;
    schema->name="schema1";
    return schema;
}


AppendResult append_field(Schema* schema,Field* field){
    const int n=5;
    if(schema->fields==NULL){
        schema->fields=malloc(sizeof(Field)*(MAX_FIELDS_COUNT+1));
        schema->fields[n-1].name=NULL;
    }
    if(schema->fields[schema->field_count].name==NULL){
        return APPEND_FULL_ERROR;
//        printf("reall\n");
//        schema->fields[schema->field_count-1].name="";
//        schema->fields=realloc(schema->fields, sizeof(Field)*(schema->field_count+n));
//        schema->fields[schema->field_count+n-1].name=NULL;
    }
    schema->fields[schema->field_count]=*field;
    schema->field_count=schema->field_count+1;
    return APPEND_SUCCESS;
};

Field* _new_field(char *json_fields){
    char* local="";
    strcpy(local,json_fields);
    Field* field_new=new_field();
    char *part1="";
    part1=strtok(local,",");
    char *part2="";
    part2=strtok(NULL,",");
    printf("part1 %s ,part2 %s\n",part1,part2);
    char* part1_1="";
    memcpy(part1_1,part1,strlen(part1));
    char* part_1_1_1=strtok(part1_1,"{");
    char* part2_1="";
    strcpy(part2_1,part2);
    char* part2_1_1=strtok(part2,"}");
    int real=(int)part2_1_1[0]-48;
    printf("part1-1 %s part2-1 %c\n",part1_1,part2_1_1[0]);

    int field=sscanf(json_fields,"%s , %s",field_new->name,part2);
    printf("%s\n",json_fields);
    printf("%s ,%s\n",field_new->name,part2);
    switch (field_new->type){
        case 0:
            field_new->type=INT;
            break;
        case 1:
            field_new->type=STRING;
            break;
        case 2:
            field_new->type=FLOAT;
            break;
        default:
            field_new->type=-1;
    };
    //printf("%s\n",field_new->name);
    return field_new;
};

//create s1 {name,1};{name,2}
Schema* create_schema(char *json){
    char* local="";
    Schema* schema=new_schema();
    strcpy(local,json);
    //printf("local %s\n",local);
    char* pattern=strtok(local,";");
    //printf("pattern %s\n",pattern);
    while(pattern!=NULL){
        Field* temp=_new_field(pattern);
        if(temp->type==-1){
            return schema;
        }
        switch (append_field(schema,temp)){
            case APPEND_SUCCESS:
                break;
            case APPEND_FULL_ERROR:
                close_field(temp);
                printf("Too many fields, rest of the fields will be abandoned!\n");
                return schema;

        };
        pattern=strtok(NULL,";");
    };
   // printf("%s\n",schema->fields[0].name);
    return schema;
}

void print_schema(Schema* schema){
    for(int i=0;i<schema->field_count;i++){
        char* type="";
        switch (schema->fields[i].type){
            case INT:
                type="INT(40)";
                break;
            case STRING:
                type="STRING(50)";
                break;
            case FLOAT:
                type="FLOAT(30)";
                break;
        }
        printf("Field  (name) %s  (type)  %s \n",schema->fields[i].name,type);
    }
};

