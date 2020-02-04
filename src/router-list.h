#ifndef ROUTE_LIST_H
#define ROUTE_LIST_H

#include <stdlib.h>

#include "http-primitives.h"

typedef struct RouterListNode_ {
    Router *router;
    struct RouterListNode_ *next;
} RouterListNode;

RouterListNode *RouterListNode_new(Router *r);
void RouterListNode_free(RouterListNode *rl_node);

typedef struct {
    RouterListNode *head;
    size_t size;
} RouterList;

RouterList RouterList_new();
void RouterList_free(RouterList *rl);
void RouterList_add(Router *r);

#endif // !ROUTE_LIST_H
