#include "LinkedList.h"
#include <iostream>

LinkedList::LinkedList() {
    head = nullptr;
}

void LinkedList::insert(string task) {
    Node* newNode = new Node{task, head};
    head = newNode;
}

void LinkedList::display() {
    Node* temp = head;
    while (temp) {
        std::cout << temp->task << std::endl;
        temp = temp->next;
    }
}
