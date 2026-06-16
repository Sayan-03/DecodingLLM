class Node:
    def __init__(self, data):
        self.data = data
        self.next = None
        self.prev = None


class DoubleLinkedList:
    def __init__(self):
        self.head = None
        self.tail = None


    def push_back(self, data):
        new_node = Node(data)

        if self.head is None:
            self.head = new_node
            self.tail = new_node
            return
        
        self.tail.next = new_node
        new_node.prev = self.tail
        self.tail = new_node
        return

    
    def push_front(self, data):
        new_node = Node(data)

        if self.head is None:
            self.head = new_node
            self.tail = new_node
            return
        
        new_node.next = self.head
        self.head.prev = new_node
        self.head = new_node
        return


    def pop_back(self):
        if self.tail is None:
            return None
        
        value = self.tail.data

        if self.head == self.tail:
            self.head = None
            self.tail = None
        else:
            self.tail = self.tail.prev
            self.tail.next = None

        return value


    def pop_front(self):
        if self.head is None:
            return None
        
        value = self.head.data
        
        if self.head == self.tail:
            self.head = None
            self.tail = None
        
        else:
            self.head = self.head.next
            self.head.prev = None
        
        return value


    def search(self, data):
        curr = self.head
        while curr is not None:
            if curr.data == data:
                return curr
            curr = curr.next
        
        return None

    
    def delete(self, data):
        node = self.search(data)
        if node is None:
            return False

        if node == self.head:
            self.pop_front()
            return True
        elif node == self.tail:
            self.pop_back()
            return True
        
        node.prev.next = node.next
        node.next.prev = node.prev
        return True


    def display_forward(self):
        curr = self.head
        while curr is not None:
            print(curr.data, end=" <-> ")
            curr = curr.next
        print("None")
        return


    def display_backward(self):
        curr = self.tail
        while curr is not None:
            print(curr.data, end=" <-> ")
            curr = curr.prev
        print("None")
        return


if __name__ == "__main__":

    '''DEBUG'''
    
    dll = DoubleLinkedList()

    dll.push_back(10)
    dll.push_back(20)
    dll.push_back(30)

    dll.push_front(5)

    dll.display_forward()

    dll.display_backward()

    dll.delete(20)

    dll.display_forward()

    print(dll.pop_front())

    print(dll.pop_back())