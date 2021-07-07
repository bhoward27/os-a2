/*
    Node_manager is an ADT that handles the allocation and deallocation of Nodes.
    When a node is deleted, it is added to the front of the node manager's list.
    When a new node is created, it is taken off of the front of the node manager's list and 
    returned.
*/

#ifndef _NODE_MANAGER_H_
#define _NODE_MANAGER_H_
#include "list.h"

typedef struct Node_manager_s Node_manager;
struct Node_manager_s {
    Node nodes[LIST_MAX_NUM_NODES];
    Node* free_head;
};

extern Node_manager node_manager;
extern Node_manager* nm_ptr;

void init_nodes(Node_manager*);
void delete_node(Node_manager*, Node*);
Node* new_node(Node_manager*);

#endif