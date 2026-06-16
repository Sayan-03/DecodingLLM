#include <headers/DoubleLinkedList.hpp>

#include <iostream>
#include <string>

Node::Node(std::string value) {
    data = value;
    next = nullptr;
    prev = nullptr;
}


DoubleLinkedList::DoubleLinkedList() {
    head = nullptr;
    tail = nullptr;
}

void DoubleLinkedList::push_back(std::string data) {
    Node* newNode = new Node(data);

    if(head == nullptr) {
        head = newNode;
        tail = newNode;
        return;
    }

    tail->next = newNode;
    newNode->prev = tail;
    tail = newNode;
    return;
}

void DoubleLinkedList::push_front(std::string data) {
    Node* newNode = new Node(data);

    if(head == nullptr) {
        head = newNode;
        tail = newNode;
        return;
    }

    head->prev = newNode;
    newNode->next = head;
    head = newNode;
    return;
}

void DoubleLinkedList::pop_back() {
    if(tail == nullptr) {
        return;
    }

    Node* temp = tail;

    if(tail == head) {
        head = nullptr;
        tail = nullptr;
        delete temp;
        return;
    }

    tail = tail->prev;
    tail->next = nullptr;
    delete temp;
    return;
}

void DoubleLinkedList::pop_front() {
    if(head == nullptr) {
        return;
    }

    Node* temp = head;

    if(head == tail) {
        head = nullptr;
        tail = nullptr;
        delete temp;
        return;
    }

    head = head->next;
    head->prev = nullptr;
    delete temp;
    return;
}

Node* DoubleLinkedList::search(std::string data) {
    Node* curr = head;
    
    while(curr != nullptr) {
        if(curr->data == data) {
            return curr;
        }
        curr = curr->next;
    }

    return nullptr;
}

void DoubleLinkedList::delete_node(std::string data) {
    Node* reqNode = DoubleLinkedList::search(data);

    if(reqNode == nullptr) {
        return;
    }

    else if(reqNode == head) {
        DoubleLinkedList::pop_front();
        return;
    }

    else if(reqNode == tail) {
        DoubleLinkedList::pop_back();
        return;
    }

    reqNode->prev->next = reqNode->next;
    reqNode->next->prev = reqNode->prev;

    delete reqNode;
    return;
}

void DoubleLinkedList::display_forward() {
    Node* current = head;

    while (current != nullptr) {
        std::cout << current->data << " <-> ";
        current = current->next;
    }

    std::cout << "NULL" << std::endl;
    return;
}

void DoubleLinkedList::display_backward() {
    Node* current = tail;

    while (current != nullptr) {
        std::cout << current->data << " <-> ";
        current = current->prev;
    }

    std::cout << "NULL" << std::endl;
    return;
}

DoubleLinkedList::~DoubleLinkedList() {
    while(head != nullptr) {
        DoubleLinkedList::pop_front();
    }
    
    return;
}