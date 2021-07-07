/*
    This file has numerous functions for a doubly linked list which uses list_manager and
    node_manager to allocate memory statically.
*/

#include "list.h"
#include "node_manager.h"
#include "list_manager.h"

static int   pend(List*, void* pItem, void (*) (List*, Node*));
static void  add_to_empty(List*, Node*);
static void  add_to_start(List*, Node*);
static void  add_to_end(List*, Node*);
static void* first_last(List*, Node*);
static int   assert_n_get_size(List*);
static void* size_1_to_0(List*, Node*);

List* List_create() {
    if (nm_ptr == NULL) {
        init_nodes(&node_manager);
        nm_ptr = &node_manager;
    }
    if (lm_ptr == NULL) {
        init_lists(&list_manager);
        lm_ptr = &list_manager;
    }
    List* l = new_list(lm_ptr);
    if (l == NULL) return NULL;

    l->head = NULL;
    l->tail = NULL;
    l->current = NULL;
    l->current_state = LIST_OOB_BAD;
    l->size = 0;
    return l;
}

int List_count(List* pList) {
    assert(pList != NULL);
    return pList->size;
}

int List_add(List* pList, void* pItem) {
    int size = assert_n_get_size(pList);

    Node* x = new_node(nm_ptr);
    if (x == NULL) return LIST_FAIL;
    x->item = pItem;

    if (size > 0) {
        enum ListOutOfBounds state = pList->current_state;
        switch(state) {
            case LIST_OOB_OK:
                x->prev = pList->current;
                x->next = pList->current->next;
                pList->current->next = x;
                pList->current = x;
                if (x->prev == pList->tail) pList->tail = x;
                break;
            case LIST_OOB_START:
                add_to_start(pList, x);
                break; 
            case LIST_OOB_END:
                add_to_end(pList, x);
                break;
            case LIST_OOB_BAD:
            default:
                return LIST_FAIL;
                break;
        }
    }
    else add_to_empty(pList, x);
    (pList->size)++;
    return LIST_SUCCESS;
}

void* List_remove(List* pList) {
    int size = assert_n_get_size(pList);
    if (size == 0) return NULL;
    Node* temp = pList->current;
    if (temp == NULL) {
        return NULL;
    }

    if (size == 1) return size_1_to_0(pList, temp);

    if (temp == pList->tail) {
        pList->tail = temp->prev;
        pList->tail->next = NULL;
        pList->current_state = LIST_OOB_END;
    }
    if (temp == pList->head) {
        temp->next->prev = NULL;
        pList->head = temp->next;
    }   
    pList->current = temp->next;

    delete_node(nm_ptr, temp);
    (pList->size)--;
    return temp->item;
}

void* List_prev(List* pList) {
    assert(pList != NULL);
    enum ListOutOfBounds state = pList->current_state;
    switch(state) {
        case LIST_OOB_START:
        case LIST_OOB_BAD:
            return NULL;
            break;
        case LIST_OOB_END:
            pList->current = pList->tail;
            pList->current_state = LIST_OOB_OK;
            return pList->tail->item;
            break;
        case LIST_OOB_OK:
            if (pList->current->prev == NULL) {
                pList->current = NULL;
                pList->current_state = LIST_OOB_START;
                return NULL;
            }

            pList->current = pList->current->prev;
            
            return pList->current->item;
            break;
        default:
            return NULL;
            break;
    }
}

void* List_curr(List* pList) {
    assert(pList != NULL);
    Node* ptr = pList->current;
    if (ptr == NULL) return NULL;
    return ptr->item;
}

void* List_first(List* pList) {
    assert(pList != NULL);
    return first_last(pList, pList->head);
}

void* List_last(List* pList) {
    assert(pList != NULL);
    return first_last(pList, pList->tail);
}

static void* first_last(List* pList, Node* first_last) {
    int size = assert_n_get_size(pList);
    if (size == 0) {
        // No need to set current to NULL, as, if the list is empty, this SHOULD already be the case.
        return NULL;
    }
    // We assume here that first_last can't be NULL if size > 0.
    pList->current = first_last;
    pList->current_state = LIST_OOB_OK;
    return first_last->item;
}

void* List_next(List* pList) {
    assert(pList != NULL);
    enum ListOutOfBounds state = pList->current_state;
    switch(state) {
        case LIST_OOB_END:
        case LIST_OOB_BAD:
            return NULL;
            break;
        case LIST_OOB_START:
            pList->current = pList->head;
            // Assume the head is not null.
            pList->current_state = LIST_OOB_OK;
            return pList->current->item;
            break;
        case LIST_OOB_OK:
            if (pList->current == pList->tail) {
                pList->current = NULL;
                pList->current_state = LIST_OOB_END;
                return NULL;
            }
            pList->current = pList->current->next;
            return pList->current->item;
            break;
        default:
            return NULL;
            break;
    }
}

int List_insert(List* pList, void* pItem) {
    int size = assert_n_get_size(pList);

    Node* x = new_node(nm_ptr);
    if (x == NULL) return LIST_FAIL;
    x->item = pItem;

    if (size > 0) {
        enum ListOutOfBounds state = pList->current_state;
        switch(state) {
            case LIST_OOB_OK:
                x->next = pList->current;
                x->prev = pList->current->prev;
                pList->current->prev->next = x;
                pList->current->prev = x;
                pList->current = x;
                if (pList->current == pList->head) pList->head = x;
                break;
            case LIST_OOB_START:
                add_to_start(pList, x);
                break;  
            case LIST_OOB_END:
                add_to_end(pList, x);
                break;
            case LIST_OOB_BAD:
            default:
                return LIST_FAIL;
                break;
        }
    }
    else add_to_empty(pList, x);
    (pList->size)++;
    return LIST_SUCCESS;
}

int List_append(List* pList, void* pItem) {
    return pend(pList, pItem, add_to_end);
}

int List_prepend(List* pList, void* pItem) {
    return pend(pList, pItem, add_to_start);
}

static int pend(List* pList, void* pItem, void (*not_empty_func) (List*, Node*)) {
    int size = assert_n_get_size(pList);

    Node* x = new_node(nm_ptr);
    if (x == NULL) return LIST_FAIL;
    x->item = pItem;

    if (size > 0) not_empty_func(pList, x);
    else add_to_empty(pList, x);
    (pList->size)++;
    return LIST_SUCCESS;
}

static void add_to_empty(List* pList, Node* x) {
    pList->head = x;
    pList->tail = x;
    x->next = NULL;
    x->prev = NULL;
    pList->current = x;
    pList->current_state = LIST_OOB_OK;
}

static void add_to_start(List* pList, Node* x) {
    x->prev = NULL;
    x->next = pList->head;
    pList->head->prev = x;
    pList->head = x;
    pList->current = x;
    pList->current_state = LIST_OOB_OK;
}

static void add_to_end(List* pList, Node* x) {
    x->prev = pList->tail;
    x->next = NULL;
    pList->tail->next = x;
    pList->tail = x;
    pList->current = x;
    pList->current_state = LIST_OOB_OK;
}

static int assert_n_get_size(List* pList) {
    int size = List_count(pList);

    // If size < 0, the value has been corrupted, perhaps due to overflow, so abort.
    assert(size >= 0);
    return size;
}

static void* size_1_to_0(List* pList, Node* temp) {
    pList->head = NULL;
    pList->tail = NULL;
    pList->current = NULL;
    pList->current_state = LIST_OOB_BAD;
    delete_node(nm_ptr, temp);
    pList->size = 0;
    return temp->item;
}  

void* List_trim(List* pList) {
    int size = assert_n_get_size(pList);
    if (size == 0) return NULL;

    Node* old_tail = pList->tail;
    if (size == 1) return size_1_to_0(pList, old_tail);

    pList->tail = old_tail->prev;
    pList->tail->next = NULL;
    pList->current_state = LIST_OOB_OK;  
    pList->current = pList->tail;

    delete_node(nm_ptr, old_tail);
    (pList->size)--;
    return old_tail->item;
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {
    assert(pList != NULL);
    assert(pComparator != NULL);

    Node* cursor;
    enum ListOutOfBounds state = pList->current_state;
    switch (state) {
        case LIST_OOB_OK:
            cursor = pList->current;
            break;
        case LIST_OOB_START: // We assume here that state != LIST_OOB_START whenever size == 0.
            cursor = pList->head;
            break;
        case LIST_OOB_END:
        case LIST_OOB_BAD:
        default:
            return NULL;
            break;
    }
    while (cursor && !pComparator(cursor->item, pComparisonArg)) cursor = cursor->next;

    // If a match was found.
    if (cursor) {
        pList->current = cursor;
        pList->current_state = LIST_OOB_OK;
        return cursor->item;
    }
    pList->current = NULL;
    pList->current_state = LIST_OOB_END;
    return NULL;
}

void List_free(List* pList, FREE_FN pItemFreeFn) {
    assert(pList != NULL);
    assert(pItemFreeFn != NULL);

    Node* cursor = pList->head;
    while (cursor) {
        Node* next = cursor->next;
        pItemFreeFn(cursor->item);
        delete_node(nm_ptr, cursor);
        (pList->size)--;
        cursor = next;
    }
    delete_list(lm_ptr, pList);
}

void List_concat(List* pList1, List* pList2) {
    int size1 = assert_n_get_size(pList1);
    int size2 = assert_n_get_size(pList2);

    if (size2 == 0) {
        delete_list(lm_ptr, pList2);
        return;
    }

    if (size1 == 0) {
        pList1->head = pList2->head;
        pList1->tail = pList2->tail;
    }
    else {
        pList2->head->prev = pList1->tail;
        pList1->tail->next = pList2->head;
        pList1->tail = pList2->tail;
    }
    pList1->size = size1 + size2;
    delete_list(lm_ptr, pList2);
}