// ewww, ugly, but does the job
#include "base_inc.h"
template <typename Node, Node *Node::* Next = &Node::next, Node *Node::* Prev = &Node::prev>
struct List {
    Node *first, *last;
    Int  len;
};

template <typename Node, Node *Node::* Next = &Node::next, Node *Node::* Prev = &Node::prev>
function void list_push(List<Node, Next, Prev> *list, Node *node) {
    if (list->first == 0) {
        list->first = node;
        list->last  = node;
        node->*Next = 0;
        node->*Prev = 0;
    } else {
        node->*Prev       = list->last;
        list->last->*Next = node;
        list->last        = node;
        node->*Next       = 0;
    }

    list->len += 1;
}

template <typename Node, Node *Node::* Next = &Node::next, Node *Node::* Prev = &Node::prev>
function void list_push_front(List<Node, Next, Prev> *list, Node *node) {
    if (list->first == 0) {
        list->first = node;
        list->last  = node;
        node->*Next = 0;
        node->*Prev = 0;
    } else {
        node->*Next        = list->first;
        list->first->*Prev = node;
        list->first        = node;
        node->*Prev        = 0;
    }

    list->len += 1;
}

template <typename Node, Node *Node::* Next = &Node::next, Node *Node::* Prev = &Node::prev>
function void list_remove(List<Node, Next, Prev> *list, Node *node) {
    if (list->first == list->last) {
        list->first = 0;
        list->last  = 0;
    } else if (list->first == node) {
        list->first = list->first->*Next;
        list->first->*Prev = 0;
    } else if (list->last == node) {
        list->last = list->last->*Prev;
        list->last->*Next = 0;
    } else {
        node->*Next->*Prev = node->*Prev;
        node->*Prev->*Next = node->*Next;
    }
}

template <typename Node, Node *Node::* Next = &Node::next, Node *Node::* Prev = &Node::prev>
function Node *list_pop_front(List<Node, Next, Prev> *list) {
    Node *result = 0;

    if (list->first) {
        result = list->first;
        list_remove(list, result);
    }

    return result;
}

template <typename Node, Node *Node::* Next = &Node::next, Node *Node::* Prev = &Node::prev>
function void list_concat(List<Node, Next, Prev> *list, List<Node, Next, Prev> other) {
    while (other.first) {
        auto node = other.first;
        list_remove(&other, node);
        list_push(list, node);
    }
}
