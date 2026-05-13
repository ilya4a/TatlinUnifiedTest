
#ifndef TATLINUNIFIEDTEST_MINHEAP_H
#define TATLINUNIFIEDTEST_MINHEAP_H

#include <iostream>
#include <vector>

using namespace std;

template <typename T>
class MinHeap {

    vector<T> array;
    std::function<bool(T, T)> comp;

    int size;
    int capacity;

public:
    MinHeap(int capacity, std::function<bool(T, T)> comp) : comp(comp)  {
        this->size = 0;
        this->capacity = capacity;
        this->array.resize(capacity);
    }

    bool empty() const { return size == 0; }

    void heapify(int i) {
        int smallest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < size && comp(array[left],array[smallest]))
            smallest = left;

        if (right < size && comp(array[right], array[smallest]))
            smallest = right;

        if (smallest != i) {
            swap(array[i], array[smallest]);
            heapify(smallest);
        }
    }

    void buildHeap(const vector<T>& arr) {
        capacity = arr.size();
        size = capacity;
        array = arr;

        for (int i = (size - 1) / 2; i >= 0; i--) {
            heapify(i);
        }
    }

    void insertNode(T value) {
        if (size == capacity) {
            capacity *= 2;
            array.resize(capacity);
        }

        size++;
        int i = size - 1;
        array[i] = value;

        while (i != 0 && comp(array[(i - 1) / 2] , array[i]) ) {
            swap(array[i], array[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }

    T peek() {
        if (size <= 0)
            throw std::runtime_error("MinHeap::peek() on empty heap");
        return array[0];
    }

    T extractMin() {
        if (size <= 0)
            throw std::runtime_error("MinHeap::extractMin() on empty heap");
        if (size == 1) {
            size--;
            return array[0];
        }

        T root = array[0];
        array[0] = array[size - 1];
        size--;
        heapify(0);
        return root;
    }

    void DeleteNode(T key) {
        int index = -1;
        for (int i = 0; i < size; ++i) {
            if (array[i] == key) {
                index = i;
                break;
            }
        }
        if (index == -1) {
            cout << "Key not found" << endl;
            return;
        }

        if (index == size - 1) {
            size--;
            return;
        }

        array[index] = array[size - 1];
        size--;

        heapify(index);
    }

};

#endif // TATLINUNIFIEDTEST_MINHEAP_H
