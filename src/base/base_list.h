// ewww, ugly, but does the job
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
