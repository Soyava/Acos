#pragma once

#include <utility>

class List {
  public:
    // Non-copyable
    List(const List&) = delete;
    List& operator=(const List&) = delete;

    List(List&& other) : List() {
        Swap(other);
    }

    List& operator=(List&& other) {
        List tmp{std::move(other)};
        Swap(tmp);
        return *this;
    }

    List() {
    }

    ~List() {
        Clear();
    }

    void PushBack(int value);

    void PushFront(int value);

    void PopBack();

    void PopFront();

    int& Back();

    int& Front();

    bool IsEmpty() const;

    void Swap(List& other);

    void Clear();

    // https://en.cppreference.com/w/cpp/container/list/splice
    // Expected behavior:
    // l1 = {1, 2, 3};
    // l1.Splice({4, 5, 6});
    // l1 == {1, 2, 3, 4, 5, 6};
    void Splice(List& other);

    template <class F>
    void ForEachElement(F&& f) const;

  private:
    struct Node {
        Node* prev;
        Node* next;
        int value;

        Node() : prev(this), next(this), value(0) {
        }

        explicit Node(int v) : prev(nullptr), next(nullptr), value(v) {
        }
    };

    void InsertBefore(Node* position, Node* node);
    void RemoveNode(Node* node);

    Node sentinel_{};
};

inline void List::InsertBefore(Node* position, Node* node) {
    node->next = position;
    node->prev = position->prev;
    position->prev->next = node;
    position->prev = node;
}

inline void List::RemoveNode(Node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

inline void List::PushBack(int value) {
    InsertBefore(&sentinel_, new Node(value));
}

inline void List::PushFront(int value) {
    InsertBefore(sentinel_.next, new Node(value));
}

inline void List::PopBack() {
    if (IsEmpty()) {
        return;
    }
    Node* node = sentinel_.prev;
    RemoveNode(node);
    delete node;
}

inline void List::PopFront() {
    if (IsEmpty()) {
        return;
    }
    Node* node = sentinel_.next;
    RemoveNode(node);
    delete node;
}

inline int& List::Back() {
    return sentinel_.prev->value;
}

inline int& List::Front() {
    return sentinel_.next->value;
}

inline bool List::IsEmpty() const {
    return sentinel_.next == &sentinel_;
}

inline void List::Swap(List& other) {
    if (this == &other) {
        return;
    }

    const bool this_empty = IsEmpty();
    const bool other_empty = other.IsEmpty();

    if (this_empty && other_empty) {
        return;
    }

    if (!this_empty && !other_empty) {
        using std::swap;
        swap(sentinel_.next, other.sentinel_.next);
        swap(sentinel_.prev, other.sentinel_.prev);

        sentinel_.next->prev = &sentinel_;
        sentinel_.prev->next = &sentinel_;
        other.sentinel_.next->prev = &other.sentinel_;
        other.sentinel_.prev->next = &other.sentinel_;
        return;
    }

    if (this_empty) {
        sentinel_.next = other.sentinel_.next;
        sentinel_.prev = other.sentinel_.prev;
        sentinel_.next->prev = &sentinel_;
        sentinel_.prev->next = &sentinel_;

        other.sentinel_.next = &other.sentinel_;
        other.sentinel_.prev = &other.sentinel_;
        return;
    }

    other.sentinel_.next = sentinel_.next;
    other.sentinel_.prev = sentinel_.prev;
    other.sentinel_.next->prev = &other.sentinel_;
    other.sentinel_.prev->next = &other.sentinel_;

    sentinel_.next = &sentinel_;
    sentinel_.prev = &sentinel_;
}

inline void List::Clear() {
    Node* current = sentinel_.next;
    while (current != &sentinel_) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    sentinel_.next = &sentinel_;
    sentinel_.prev = &sentinel_;
}

inline void List::Splice(List& other) {
    if (this == &other || other.IsEmpty()) {
        return;
    }

    Node* first = other.sentinel_.next;
    Node* last = other.sentinel_.prev;

    other.sentinel_.next = &other.sentinel_;
    other.sentinel_.prev = &other.sentinel_;

    if (IsEmpty()) {
        sentinel_.next = first;
        sentinel_.prev = last;
        first->prev = &sentinel_;
        last->next = &sentinel_;
        return;
    }

    Node* tail = sentinel_.prev;
    tail->next = first;
    first->prev = tail;
    last->next = &sentinel_;
    sentinel_.prev = last;
}

template <class F>
inline void List::ForEachElement(F&& f) const {
    for (Node* current = sentinel_.next; current != &sentinel_;
         current = current->next) {
        f(current->value);
    }
}
