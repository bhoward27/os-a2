/*
    List_manager is an ADT that manages List "heads" to ensure that any valid combination of
    calls to List_free() and List_create() are functional. It operates in almost exactly the same
    manner as Node_manager, except here its data are Lists instead of Nodes.
*/

#ifndef _LIST_MANAGER_H_
#define _LIST_MANAGER_H_
#include "list.h"

typedef struct List_manager_s List_manager;
struct List_manager_s {
    List lists[LIST_MAX_NUM_HEADS];
    List* free_head;
};

extern List_manager list_manager;
extern List_manager* lm_ptr;

void init_lists(List_manager*);
void delete_list(List_manager*, List*);
List* new_list(List_manager*);

#endif