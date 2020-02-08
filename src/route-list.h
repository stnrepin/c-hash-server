#ifndef ROUTE_LIST_H
#define ROUTE_LIST_H

#include <stdlib.h>

#include "http-primitives.h"

typedef struct RouteListNode_ {
    Route *route;
    struct RouteListNode_ *next;
} RouteListNode;

RouteListNode *RouteListNode_new(Route *r);
void RouteListNode_free(RouteListNode *rl_node);

typedef struct {
    RouteListNode *head;
    RouteListNode *last;
    size_t size;
} RouteList;

RouteList *RouteList_new();
void RouteList_free(RouteList *l);
void RouteList_add(RouteList *l, Route *r);

#endif // !ROUTE_LIST_H
