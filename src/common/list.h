#pragma once

#include <common/defines.h>

// ListNode represents one node on a circular list.
typedef struct ListNode {
    struct ListNode *prev, *next;
} ListNode;

// initialize a sigle node circular list.
void init_list_node(ListNode *node);

// merge the list containing `node1` and the list containing `node2`
// into one list. It guarantees `node1->next == node2`. Both lists can be
// empty. This function will return the merged list.
ListNode *merge_list(ListNode *node1, ListNode *node2);

// remove `node` from the list, and then `node` becomes a single
// node list. It usually returns `node->prev`. If `node` is
// the last one in the list, it will return NULL.
ListNode *detach_from_list(ListNode *node);
