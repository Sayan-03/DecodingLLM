#pragma once

#include <string>

class Node{
    public:
        std::string data;
        Node* next;
        Node* prev;

        // Constructor
        Node(std::string value);
};

class DoubleLinkedList{
    public:
        Node* head;
        Node* tail;

        // Constructor
        DoubleLinkedList();

        // APIs
        void push_back(std::string data);
        void push_front(std::string data);
        void pop_back();
        void pop_front();
        Node* search(std::string data);
        void delete_node(std::string data);
        void display_forward();
        void display_backward();

        // Destructor
        ~DoubleLinkedList();
};