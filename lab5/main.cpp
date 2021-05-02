#include <memory>
#include <iostream>

template<class T, typename Alloc = std::allocator<T>>
class circular_buffer {
private:
    T *buf_;
    Alloc alloc_;
    size_t capacity_ = 10;
    size_t head_ = 0;
    size_t tail_ = 0;
    bool full_ = false;
public:
    circular_buffer() {
        buf_ = alloc_.allocate(capacity_);
    }

    explicit circular_buffer(size_t size) : capacity_{size * 3 / 2} {
        buf_ = alloc_.allocate(capacity_);
    }

    ~circular_buffer() {
        for (auto it = buf_; it != buf_ + size(); ++it)
            alloc_.destroy(it);
        alloc_.deallocate(buf_, capacity_);
    }

    void push_front(T item) {
        buf_[head_] = item;

        if (full_) {
            tail_ = (tail_ + 1) % capacity_;
        }

        head_ = (head_ + 1) % capacity_;

        full_ = head_ == tail_;
    }

    void push_back(T item) {
        buf_[tail_] = item;

        if (full_) {
            head_ = (head_ - 1) % capacity_;
        }

        tail_ = (tail_ - 1) % capacity_;

        full_ = head_ == tail_;
    }

    void pop_back() {
        if (!empty()) {
            alloc_.destroy(buf_ + tail_);

            tail_ = (tail_ + 1) % capacity_;
        }
    }

    void pop_front() {
        if (!empty()) {
            alloc_.destroy(buf_ + head_);

            head_ = (head_ - 1) % capacity_;
        }
    }

    void resize(size_t length) {
        T *new_buf = alloc_.allocate(length);

        size_t lim;
        if (length < size()) {
            lim = length;
        } else {
            lim = size();
        }

        for (int i = 0; i < lim; i++) {
            new_buf[i] = buf_[i];
        }

        for (auto it = buf_; it != buf_ + size(); ++it)
            alloc_.destroy(it);
        alloc_.deallocate(buf_, capacity_);

        buf_ = new_buf;
    }

    bool empty() const {
        return (!full_ && (head_ == tail_));
    }

    bool full() const {
        return full_;
    }

    size_t capacity() const {
        return capacity_;
    }

    size_t size() const {
        size_t size = capacity_;

        if (!full_) {
            if (head_ >= tail_) {
                size = head_ - tail_;
            } else {
                size = capacity_ + head_ - tail_;
            }
        }

        return size;
    }

    T front() const {
        return buf_[head_];
    }

    T back() const {
        return buf_[tail_];
    }

    T operator[](size_t number) const {
        return buf_[(tail_ + number) % capacity_];
    }
};