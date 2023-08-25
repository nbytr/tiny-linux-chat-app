#pragma once

typedef struct
{
  int* data;
  int size;
  int allocated;
} UtilIntList;

UtilIntList*
util_int_list_new (int default_allocate);

void
util_int_list_add (UtilIntList *lst, int item);

void
util_int_list_delete (UtilIntList *lst, int item);
