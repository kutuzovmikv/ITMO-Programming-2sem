#include <memory>
#include <iostream>

template<class T>
class circular_buffer_iterator
        : public std::iterator<std::random_access_iterator_tag, T> {
public:
    circular_buffer_iterator() = delete;

    explicit circular_buffer_iterator(T *p) : ptr_(p) {}

    circular_buffer_iterator(const circular_buffer_iterator<T> &it) : ptr_(it.ptr_) {}

    inline circular_buffer_iterator<T> &operator=(const T &value) {
        ptr_ = *value;
        return *this;
    }

    inline circular_buffer_iterator<T> &operator=(T *ptr) {
        ptr_ = ptr;
        return *this;
    }

    inline circular_buffer_iterator<T> &operator=(const circular_buffer_iterator<T> &it) {
        ptr_ = it.ptr_;
        return *this;
    }

    inline T *operator->() const {
        return *ptr_;
    }

    inline T &operator*() const {
        return *ptr_;
    }

    inline circular_buffer_iterator<T> &operator+=(const int &val) {
        for (int i = 0; i < val; i++)
            ptr_ = ptr_->next;
        return *this;
    }

    inline circular_buffer_iterator<T> &operator-=(const int &val) {
        for (int i = val; i > 0; i--)
            ptr_ = ptr_->prev;
        return *this;
    }

    inline circular_buffer_iterator<T> &operator++() {
        ptr_ = ptr_ + 1;
        return *this;
    }

    inline circular_buffer_iterator<T> &operator--() {
        ptr_ = ptr_ - 1;
        return *this;
    }

    inline circular_buffer_iterator<T> operator+(const int val) {
        circular_buffer_iterator it(*this);
        it += val;
        return it;
    }

    inline circular_buffer_iterator<T> operator-(const int val) {
        circular_buffer_iterator it(*this);
        it -= val;
        return it;
    }

    friend inline int operator+(const circular_buffer_iterator<T> &it1, const circular_buffer_iterator<T> &it2) {
        circular_buffer_iterator<T> it = it1;
        int result1 = 0;
        while (!it.ptr_->prev->is_end) {
            it--;
            result1++;
        }

        it = it2;
        int result2 = 0;
        while (!it.ptr_->prev->is_end) {
            it--;
            result2++;
        }

        return result1 + result2;
    }

    friend inline int operator-(circular_buffer_iterator<T> it1, circular_buffer_iterator<T> it2) {
        circular_buffer_iterator<T> it = it1;
        int result1 = 0;
        while (!it.ptr_->prev->is_end) {
            it--;
            result1++;
        }

        it = it2;
        int result2 = 0;
        while (!it.ptr_->prev->is_end) {
            it--;
            result2++;
        }

        return result1 - result2;
    }

    friend inline circular_buffer_iterator<T> operator+(const circular_buffer_iterator<T> &it, const int &val) {
        return circular_buffer_iterator(it + val);
    }

    friend inline circular_buffer_iterator<T> operator-(const circular_buffer_iterator<T> &it, const int &val) {
        return circular_buffer_iterator(it - val);
    }

    friend inline bool operator==(const circular_buffer_iterator<T> &it1, const circular_buffer_iterator<T> &it2) {
        return it1.ptr_ == it2.ptr_;
    }

    inline bool operator==(T *it) {
        return ptr_ == it;
    }

    inline bool operator!=(const circular_buffer_iterator<T> &it) {
        return ptr_ != it.ptr_;
    }

    inline bool operator!=(T *it) {
        return ptr_ != it;
    }

    friend inline bool operator>(const circular_buffer_iterator<T> &it1, const circular_buffer_iterator<T> &it2) {
        circular_buffer_iterator<T> it = it1;

        while (it != it2) {
            if (it.ptr_->is_end)
                return true;
            it++;
        }
        return false;
    }

    friend inline bool operator>=(const circular_buffer_iterator<T> &it1, const circular_buffer_iterator<T> &it2) {
        return it1 > it2 || it1 == it2;
    }

    friend inline bool operator<(const circular_buffer_iterator<T> &it1, const circular_buffer_iterator<T> &it2) {
        return it2 > it1;
    }

    friend inline bool operator<=(const circular_buffer_iterator<T> &it1, const circular_buffer_iterator<T> &it2) {
        return it2 > it1 || it2 == it1;
    }

private:
    T *ptr_;
};


template<class T, typename Alloc = std::allocator<T>>
class circular_buffer {
private:
    T *buf_;
    Alloc alloc_;
    size_t capacity_ = 10;
    size_t head_ = 0;
    size_t tail_ = 0;
    bool full_ = false;
    T empty_value;
public:
    friend class circular_buffer_iterator<T>;

    typedef circular_buffer_iterator<T> iterator;
    typedef circular_buffer_iterator<const T> const_iterator;

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

    void push_front(const T &item) {
        buf_[head_] = item;

        if (full_) {
            tail_ = (tail_ + 1) % capacity_;
        }

        head_ = (head_ + 1) % capacity_;

        full_ = head_ == tail_;
    }

    void push_back(const T &item) {
        buf_[tail_] = item;

        if (full_) {
            head_ = (head_ - 1) % capacity_;
        }

        tail_ = (tail_ - 1) % capacity_;

        full_ = head_ == tail_;
    }

    void pop_back() {
        if (!empty()) {
            buf_[tail_] = empty_value;

            tail_ = (tail_ + 1) % capacity_;

            if (full_) {
                head_ = (head_ + 1) % capacity_;
                full_ = false;
            }
        }
    }

    void pop_front() {
        if (!empty()) {
            buf_[head_] = empty_value;

            head_ = (head_ - 1) % capacity_;

            if (full_) {
                tail_ = (tail_ - 1) % capacity_;
                full_ = false;
            }
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

    [[nodiscard]] bool empty() const {
        return (!full_ && (head_ == tail_));
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
            if (head_ >= tail_) {
                size = head_ - tail_;
            } else {
                size = capacity_ + head_ - tail_;
            }
        }

        return size;
    }

    [[nodiscard]] T front() const {
        return buf_[head_];
    }

    [[nodiscard]] T back() const {
        return buf_[tail_];
    }

    T operator[](size_t number) const {
        return buf_[(tail_ + number) % capacity_];
    }

    iterator begin() {
        return iterator(buf_ + tail_);
    }

    iterator end() {
        if (full_)
            return iterator(buf_ + (head_ - 1) % capacity_);
        return iterator(buf_ + head_);
    }

    [[nodiscard]] const_iterator begin() const {
        return const_iterator(buf_ + tail_);
    }

    [[nodiscard]] const_iterator end() const {
        return const_iterator(buf_ + head_);
    }
};

int main() {
    circular_buffer<int> cb(3);
    for (int i = 0; i < 3; i++) {
        cb.push_front(i + 10);
    }

    for (int &it : cb) {
        std::cout << it << std::endl;
    }
}