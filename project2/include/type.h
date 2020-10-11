#ifndef __TYPE_H__
#define __TYPE_H__


#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>

#define LEAF_ORDER 32
#define INTERNAL_ORDER 248

typedef uint64_t pagenum_t;

typedef struct inter_record{
    int64_t key;
    pagenum_t pagenum;
}inter_record;


typedef struct record {
    int64_t key;
    char* value;
}record;

extern char* filename;

#endif
