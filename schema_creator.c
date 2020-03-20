//
// Created by liujie on 2020/3/18.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "schema_creator.h"

void close_schema(Schema* schema){
    for(int i =0;i<schema->field_count;i++){
        close_field(schema->fields[i]);
    }
    free(schema->fields);
    free(schema);
};

Schema* new_schema(){
    Schema* schema=malloc(sizeof(Schema));
    schema->fields=NULL;
    schema->field_count=0;
    return schema;
}

Field* new_field(){
    Field* field=malloc(sizeof(Field));
    field->name="";
    field->type=INT;
    return field;
}

void close_field(Field* field){
    free(field);
}

int append_field(Schema* schema,Field* field){
    const int n=5;
    if(schema->fields==NULL){
        schema->fields=malloc(sizeof(Field*)*(n));
        schema->fields[n]=NULL;
    }
    if(schema->fields[schema->field_count]==NULL){
        schema->fields=realloc(schema->fields, sizeof(Field)*(schema->field_count+n));
        schema->fields[schema->field_count+n]=NULL;
    }
    schema->fields[schema->field_count]=field;
    schema->field_count=schema->field_count+1;
    return 1;
};

Field* _new_field(char *json_fields){
    static char* local="";
    if(strcmp(local,"") != 0){
        local="";
        memcpy(local,json_fields,strlen(json_fields));
    }else{
        memcpy(local,json_fields,strlen(json_fields));
        //strcat(local,"}");
    }
    printf("%s\n",local);
    Field* field_new=new_field();
    int field=sscanf(json_fields,"{%s ,%d}",field_new->name,&(field_new->type));
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
    if(field!=2){
        field_new->name=NULL;
        field_new->type=-1;
        printf("create field %s err,parse string failed",json_fields);
    };
    printf("field is got value %s\n",field_new->name);
    return field_new;
};

Field** _new_fields(char *json_fields){
    Field** pField=malloc(sizeof(Field**)*10);
    int flag=0;
    char* pattern=strtok(json_fields,";");
    while(pattern!=NULL){
        Field* temp;
        temp=_new_field(pattern);
        pField[flag]=temp;
        flag++;
        pattern=strtok(NULL,";");
    };
    pField[flag+1]=NULL;
    return pField;
};

Schema* _create_schema(Field** pField){
    Schema *schema = new_schema();
    int count=0;
    while(*(pField+count)!=NULL){
        printf("%d index pre add\n",count);
        append_field(schema,*(pField+count));
        printf("%d index suc add\n",count);
        count++;
    }
    free(pField);
    printf("count is %d\n",schema->field_count);
    return schema;
}

Schema* create_schema(char json[]){
    Schema* schema=new_schema();
    char* local=json;
    static char* pattern;
    pattern="";
    if(strcmp(pattern,"")!=0){
        pattern="";
    }
    pattern=strtok(json,";");
    while(pattern!=NULL){
        static Field* temp;
        temp=_new_field(pattern);
        append_field(schema,temp);
        pattern=strtok(NULL,";");
    };
    return schema;
}


