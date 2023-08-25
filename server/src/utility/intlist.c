#include "intlist.h"

#include <stdlib.h>

static void
util_int_list_allocate_more (UtilIntList *lst)
{
  lst->allocated = lst->allocated * 2;
  lst->data = realloc (lst->data, lst->allocated);
}

UtilIntList*
util_int_list_new (int default_allocate)
{
  UtilIntList *lst= calloc (1, sizeof (UtilIntList));

  lst->allocated = default_allocate;
  lst->size = 0;

  lst->data = calloc (sizeof (int), default_allocate);

  return lst;
}

void
util_int_list_add (UtilIntList *lst, int item)
{
  if (lst->allocated <= (lst->size - 1)) {
    util_int_list_allocate_more (lst);
  }

  lst->data[lst->size] = item;
  lst->size++;
}

void
util_int_list_delete (UtilIntList *lst, int item)
{
  for (int i = 0; i < lst->size; ++i) {
    if (lst->data[i] == item) {
      item = lst->data[lst->size-1];
      lst->size--;
      break;
    }
  }
}
