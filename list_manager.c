/*
    List_manager is an ADT that manages List "heads" to ensure that any valid combination of
    calls to List_free() and List_create() are functional. It operates in almost exactly the same
    manner as Node_manager, except here its data are Lists instead of Nodes.
*/

#include "list_manager.h"

List_manager list_manager;
List_manager* lm_ptr = NULL;

void init_lists(List_manager* list_manager) {
    assert(list_manager != NULL);
    assert(list_manager->lists != NULL);

    List* head = list_manager->free_head = list_manager->lists;
    List* current = head;
    for (int i = 1; i < LIST_MAX_NUM_HEADS; ++i) {
        List* l = current->next_list = head + i;
        current = l;
    }
    current->next_list = NULL;
}

void delete_list(List_manager* list_manager, List* l) {
    assert(list_manager != NULL);
    assert(l != NULL);
    assert(l != list_manager->free_head);

    l->next_list = list_manager->free_head;
    list_manager->free_head = l;
}

List* new_list(List_manager* list_manager) {
    assert(list_manager != NULL);

    if (list_manager->free_head == NULL) return NULL;

    List* new_list = list_manager->free_head;
    list_manager->free_head = new_list->next_list;
    return new_list;
}