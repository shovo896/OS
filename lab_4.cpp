#include <iostream>
#include <vector>
#include <stdexcept>

using namespace std;

// Heap Queue (Max-Heap Priority Queue)
// Higher value = higher priority, dequeued first.
class HeapQueue {
private:
    vector<int> heap; // 0-based: parent = (i-1)/2, children = 2*i+1, 2*i+2

    void heapifyUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (heap[i] <= heap[parent]) break;
            swap(heap[i], heap[parent]);
            i = parent;
        }
    }

    void heapifyDown(int i) {
        int n = heap.size();
        while (true) {
            int largest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            if (left < n && heap[left] > heap[largest]) largest = left;
            if (right < n && heap[right] > heap[largest]) largest = right;
            if (largest == i) break;
            swap(heap[i], heap[largest]);
            i = largest;
        }
    }

public:
    bool isEmpty() const { return heap.empty(); }
    int size() const { return heap.size(); }

    // Enqueue: insert at end, then bubble up. O(log n)
    void enqueue(int value) {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }

    // Dequeue: remove max (root), move last element to root, bubble down. O(log n)
    int dequeue() {
        if (isEmpty()) throw runtime_error("HeapQueue is empty");
        int top = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!isEmpty()) heapifyDown(0);
        return top;
    }

    // Peek: highest priority element without removing it. O(1)
    int peek() const {
        if (isEmpty()) throw runtime_error("HeapQueue is empty");
        return heap[0];
    }

    void display() const {
        cout << "HeapQueue: [";
        for (size_t i = 0; i < heap.size(); ++i) {
            if (i) cout << ", ";
            cout << heap[i];
        }
        cout << "] (size = " << heap.size() << ")" << endl;
    }
};

int main() {
    HeapQueue pq;

    cout << "Enqueue: 10, 20, 5, 30, 15" << endl;
    pq.enqueue(10);
    pq.enqueue(20);
    pq.enqueue(5);
    pq.enqueue(30);
    pq.enqueue(15);
    pq.display();

    cout << "Peek (highest priority): " << pq.peek() << endl;

    cout << "Dequeue order (max first): ";
    while (!pq.isEmpty()) {
        cout << pq.dequeue() << " ";
    }
    cout << endl;

    return 0;
}
