#ifndef LINKEDLIST_H
#define LINKEDLIST_H

struct Node {
    string task;
    Node* next;
};

class LinkedList {
private:
    Node* head;
public:
    LinkedList();
    void insert(string task);
    void display();
};

#endif
