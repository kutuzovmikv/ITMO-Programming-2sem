#include <memory>
#include <iostream>

template<class T>
class Circular_Buffer {
private:
    std::unique_ptr<T[]> buffer;

    size_t head = 0;
    size_t tail = 0;
    size_t cur_size = 0;
    T empty_item;
public:
    explicit Circular_Buffer<T>(size_t size)
            : buffer(std::unique_ptr<T[]>(new T[size])), cur_size(size) {};

    void push_back(T item) {
        buffer = level_up(buffer, 1);
        cur_size++;

        buffer[tail] = item;

        tail++;
    }

    void push_front(T item) {
        buffer = level_down(buffer, 1);

        buffer[head] = item;

        cur_size++;
        tail++;
    }

    void resize(size_t length) {
        if (cur_size < length) {
            buffer = level_up(buffer, length - cur_size);
        } else {
            while (cur_size > length) {
                pop_back();
            }
        }
        cur_size = length;
    }

    std::unique_ptr<T[]> level_up(std::unique_ptr<T[]> &arr, size_t length) {
        std::unique_ptr<T[]> new_buffer(new T[cur_size + length]);
        for (int i = 0; i < cur_size; i++) {
            new_buffer[i] = arr[i];
        }
        return new_buffer;
    }

    std::unique_ptr<T[]> level_down(std::unique_ptr<T[]> &arr, size_t length) {
        std::unique_ptr<T[]> new_buffer(new T[cur_size + length]);
        for (int i = 1; i < cur_size + 1; i++)
            new_buffer[i] = arr[i - 1];
        return new_buffer;
    }

    void pop_front() {

        if (is_empty())
            throw std::runtime_error("buffer is empty");

        buffer[head] = empty_item;

        cur_size--;
        head++;
    }

    void pop_back() {

        if (is_empty())
            throw std::runtime_error("buffer is empty");

        buffer[tail - 1] = empty_item;

        cur_size--;
        tail--;
    }

    T front() { return buffer[head]; }

    T back() { return buffer[tail - 1]; }

    bool is_empty() { return head == tail; }

    bool is_full() { return head + 1 == tail; }

    size_t size() {
        return cur_size;
    }

    T operator[](size_t number) {
        return buffer[head + number];
    }
};

int main() {
    Circular_Buffer<uint32_t> cb(1);
    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);
    cb.push_front(4);
    cb.pop_front();

    std::cout << cb.size() << std::endl;

    cb.resize(10);

    for (int i = 0; i < cb.size(); i++) {
        std::cout << cb[i] << " ";
    }

    std::cout << cb.size() << std::endl;
}