#include <memory>
#include <iostream>
#include <algorithm>

template<class T>
class circular_buffer_iterator
        : public std::iterator<std::random_access_iterator_tag, T> {
public:
    explicit circular_buffer_iterator(T *p, size_t pos, size_t capacity) : ptr_(p), pos_(pos), capacity_(capacity) {}

    circular_buffer_iterator(const circular_buffer_iterator<T> &it) : ptr_(it.ptr_), pos_(it.pos_),
                                                                      capacity_(it.capacity_) {}

    inline bool operator!=(const circular_buffer_iterator<T> &it) {
        return ptr_ + pos_ != it.ptr_ + it.pos_;
    }

    inline circular_buffer_iterator<T> &operator++() {
        pos_ = (pos_ + 1) % capacity_;
        return *this;
    }

    inline T &operator*() const {
        return *(ptr_ + pos_);
    }

    inline T* operator -> () const {
        return *(ptr_ + pos_);
    }

    inline circular_buffer_iterator<T>& operator = (const T& value) {
        ptr_ = *value;
        return *this;
    }

    inline circular_buffer_iterator<T>& operator = (T* ptr) {
        ptr_ = ptr;
        return *this;
    }

    inline circular_buffer_iterator<T>& operator = (const circular_buffer_iterator<T>& it) {
        ptr_ = it.ptr_;
        capacity_ = it.capacity_;
        pos_ = it.pos_;
        return *this;
    }

private:
    T *ptr_;
    size_t capacity_;
    size_t pos_;
};


template<class T, class Allocator = std::allocator<T>>
class circular_buffer {
private:
    T *buf_;
    Allocator alloc_;
    using traits_ = std::allocator_traits<decltype(alloc_)>;
    size_t capacity_ = 10;
    size_t start_ = 0;
    size_t end_ = 1;
    bool full_ = false;
    bool empty_ = true;
public:
    friend class circular_buffer_iterator<T>;

    typedef circular_buffer_iterator<T> iterator;
    typedef circular_buffer_iterator<const T> const_iterator;

    circular_buffer() {
        buf_ = traits_::allocate(alloc_, capacity_);
    }

    explicit circular_buffer(size_t size) : capacity_{size} {
        if (size < 2)
            throw;
        buf_ = traits_::allocate(alloc_, capacity_);
    }

    ~circular_buffer() {
        for (auto it = buf_; it != buf_ + capacity_; ++it)
            traits_::destroy(alloc_, it);
        traits_::deallocate(alloc_, buf_, capacity_);
    }

    void push_front(const T &item) {

        if (empty()) {
            traits_::construct(alloc_, buf_ + start_, item);
            empty_ = false;
            return;
        }

        empty_ = false;
        start_ = (start_ - 1) % capacity_;

        traits_::construct(alloc_, buf_ + start_, item);

        if (full_) {
            end_ = (end_ + 1) % capacity_;
        }

        full_ = start_ == end_;
    }

    void push_back(const T &item) {
        empty_ = false;
        traits_::construct(alloc_, buf_ + end_, item);

        if (full_) {
            start_ = (start_ + 1) % capacity_;
        }

        end_ = (end_ + 1) % capacity_;

        full_ = start_ == end_;
    }

    void pop_back() {
        if (empty()) {
            return;
        }

        traits_::destroy(alloc_, buf_ + end_);


        if (start_ == end_)
            start_ = (start_ + 1) % capacity_;

        end_ = (end_ - 1) % capacity_;


        full_ = false;
    }

    void pop_front() {
        if (empty()) {
            return;
        }

        traits_::destroy(alloc_, buf_ + start_);

        if (start_ == end_) {
            end_ = (end_ - 1) % capacity_;
        }

        start_ = (start_ + 1) % capacity_;

        full_ = false;
    }

    void resize(const size_t &length) {
        // если меньше - задестроить элементы за размером, такой же ниче, если больше - добавляем дефолт, либо впихиваем больше

        if (length < capacity_) {

            for (auto it = buf_ + length; it != buf_ + capacity_; ++it)
                traits_::destroy(alloc_, it);

            capacity_ = length;
            if (start_ > length)
                start_ -= length;
            if (end_ > length)
                end_ -= length;

        } else if (length > capacity_) {
            empty_ = true;

            T *new_buf = traits_::allocate(alloc_, length);
            for (auto it = buf_; it != buf_ + capacity_; ++it)
                traits_::construct(alloc_, new_buf, *it);

            for (auto it = buf_; it != buf_ + capacity_; ++it)
                traits_::destroy(alloc_, it);
            traits_::deallocate(alloc_, buf_, capacity_);

            buf_ = new_buf;
        }
    }

    void resize(const size_t &length, const T &value) {
        // если меньше - задестроить элементы за размером, такой же ниче, если больше - добавляем дефолт, либо впихиваем больше

        if (length < capacity_) {

            for (auto it = buf_ + length; it != buf_ + capacity_; ++it)
                traits_::destroy(alloc_, it);

            if (start_ > length)
                start_ -= length;
            if (end_ > length)
                end_ -= length;

        } else if (length > capacity_) {
            empty_ = false;
            T *new_buf = traits_::allocate(alloc_, length);
            for (auto it = buf_; it != buf_ + capacity_; ++it)
                traits_::construct(alloc_, new_buf, *it);
            for (auto it = buf_ + capacity_; it != buf_ + length; ++it)
                traits_::construct(alloc_, new_buf, value);

            for (auto it = buf_; it != buf_ + capacity_; ++it)
                traits_::destroy(alloc_, it);
            traits_::deallocate(alloc_, buf_, capacity_);

            buf_ = new_buf;
        }

        capacity_ = length;
    }

    void reserve(const size_t &length) {
        if (length > capacity_) {
            empty_ = true;
            capacity_ = length;

            T *new_buf = traits_::allocate(alloc_, length);
            for (auto it = buf_; it != buf_ + capacity_; ++it)
                traits_::construct(alloc_, new_buf, *it);

            for (auto it = buf_; it != buf_ + capacity_; ++it)
                traits_::destroy(alloc_, it);
            traits_::deallocate(alloc_, buf_, capacity_);

            buf_ = new_buf;
        }
    }

    [[nodiscard]] bool empty() const {
        return empty_;
    }

    [[nodiscard]] bool full() const {
        return full_;
    }

    [[nodiscard]] size_t capacity() const {
        return capacity_;
    }

    [[nodiscard]] size_t size() const {
        size_t size = capacity_;

        if (!full_) {
            if (end_ >= start_) {
                size = end_ - start_;
            } else {
                size = capacity_ + end_ - start_;
            }
        }

        return size;
    }

    [[nodiscard]] T front() const {
        return buf_[start_];
    }

    [[nodiscard]] T back() const {
        if (full_)
            return buf_[end_];
        else
            return buf_[end_ - 1];
    }

    T operator[](const size_t &number) const {
        return buf_[(start_ + number) % capacity_];
    }

    iterator begin() {
        if (start_ == end_)
            return iterator(buf_, (start_ + 1) % capacity_, capacity_);
        return iterator(buf_, start_, capacity_);
    }

    iterator end() {
        return iterator(buf_, end_, capacity_);
    }

    [[nodiscard]] const_iterator begin() const {
        return const_iterator(buf_, start_, capacity_);
    }

    [[nodiscard]] const_iterator end() const {
        return const_iterator(buf_, start_, capacity_);
    }
};

int main() {
    circular_buffer<int> buffer(4);
    buffer.push_front(1);
    buffer.push_front(2);
    buffer.push_back(3);
    buffer.push_back(4);
    buffer.resize(2);

    for (const auto &it : buffer) {
        std::cout << it << " ";
    }
}