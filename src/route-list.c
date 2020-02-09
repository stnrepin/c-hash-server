#include "route-list.h"

#include <stdlib.h>

#include "hash-server-error.h"

RouteListNode *RouteListNode_new(Route *r) {
    RouteListNode *node;
    node = malloc(sizeof(RouteListNode));
    if (node == NULL) {
        panic(E_ALLOC);
    }
    node->route = r;
    node->next = NULL;
    return node;
}

void RouteListNode_free(RouteListNode *n) {
    if (n == NULL) {
        return;
    }
    //free(n->route); // Note: RouteList DOES NOT manage routes.
                      // If you allocate it, you should worry about free. 
    free(n);
}

RouteList *RouteList_new() {
    RouteList *l;
    l = malloc(sizeof(RouteList));
    if (l == NULL) {
        panic(E_ALLOC);
    }
    l->head = NULL;
    l->last = NULL;
    l->size = 0;
    return l;
}

void RouteList_free(RouteList *l) {
    RouteListNode *cur, *tmp;
    if (l == NULL || l->size == 0) {
        return;
    }
    cur = l->head;
    while (cur != NULL) {
        tmp = cur->next;
        RouteListNode_free(cur);
        cur = tmp;
    }
    l->size = 0;
}

void RouteList_add(RouteList *l, Route *r) {
    RouteListNode *new_node;
    new_node = RouteListNode_new(r);
    if (l->head == NULL) {
        l->head = new_node;
    }
    if (l->last != NULL) {
        l->last->next = new_node;
    }
    l->last = new_node;
    l->size++;
}
