#include <iostream>
#include <memory>
#include <algorithm>
#include <cmath>

template<class T, class Allocator = std::allocator<T>>
class circular_buffer {
public:
    class const_iterator : public std::iterator<std::random_access_iterator_tag, T> {
    public:
        explicit const_iterator(T *iter, const size_t &pos, const size_t &cap, const size_t &start) : iter_(iter),
                                                                                                      pos_(pos),
                                                                                                      cap_(cap),
                                                                                                      start_(start) {};

        ~const_iterator() = default;

        const T &operator*() const {
            return *(iter_ + pos_);
        }

        const_iterator operator++() {
            pos_ = (pos_ + 1) % cap_;
            return *this;
        }

        const_iterator operator--() {
            pos_ = (pos_ - 1) % cap_;
            return *this;
        }

        const_iterator &operator+=(int value) {
            pos_ = (pos_ + value) % cap_;
            return *this;
        }

        const_iterator &operator-=(int value) {
            pos_ = (pos_ - value) % cap_;
            return *this;
        }

        const_iterator operator+(int value) const {
            iterator tmp = *this;
            tmp.pos_ = (tmp.pos_ + value) % cap_;
            return tmp;
        }

        const_iterator operator-(int value) const {
            iterator tmp = *this;
            tmp.pos_ = (tmp.pos_ - value) % cap_;
            return tmp;
        }

        using difference_type = typename std::iterator<std::random_access_iterator_tag, T>::difference_type;

        difference_type operator-(const const_iterator &obj) const {
            return (pos_ - obj.pos_) % cap_;
        }

        bool operator==(const const_iterator &it) const {
            return (start_ + pos_) % cap_ == (it.start_ + it.pos_) % cap_;
        }

        bool operator!=(const const_iterator &it) const {
            return (start_ + pos_) % cap_ != (it.start_ + it.pos_) % cap_;
        }

        bool operator<(const const_iterator &it) const {
            return (start_ + pos_) % cap_ < (it.start_ + it.pos_) % cap_;
        }

        bool operator<=(const const_iterator &it) const {
            return (start_ + pos_) % cap_ <= (it.start_ + it.pos_) % cap_;
        }

        bool operator>(const const_iterator &it) const {
            return (start_ + pos_) % cap_ > (it.start_ + it.pos_) % cap_;
        }

        bool operator>=(const const_iterator &it) const {
            return (start_ + pos_) % cap_ >= (it.start_ + it.pos_) % cap_;
        }

    private:
        T *iter_;
        size_t pos_;
        size_t cap_;
        size_t start_;
    };

    class iterator : public std::iterator<std::random_access_iterator_tag, T> {
    public:
        explicit iterator(T *iter, const size_t &pos, const size_t &cap, const size_t &start) : iter_(iter),
                                                                                                pos_(pos), cap_(cap),
                                                                                                start_(start) {};

        ~iterator() = default;

        T &operator*() const {
            return *(iter_ + pos_);
        }

        iterator operator++() {
            pos_ = (pos_ + 1) % cap_;
            return *this;
        }

        iterator operator--() {
            pos_ = (pos_ - 1) % cap_;
            return *this;
        }

        iterator &operator+=(int value) {
            pos_ = (pos_ + value) % cap_;
            return *this;
        }

        iterator &operator-=(int value) {
            pos_ = (pos_ - value) % cap_;
            return *this;
        }

        iterator operator+(int value) const {
            iterator tmp = *this;
            tmp.pos_ = (tmp.pos_ + value) % cap_;
            return tmp;
        }

        iterator operator-(int value) const {
            iterator tmp = *this;
            tmp.pos_ = (tmp.pos_ - value) % cap_;
            return tmp;
        }

        using difference_type = typename std::iterator<std::random_access_iterator_tag, T>::difference_type;

        difference_type operator-(const iterator &obj) const {
            return (pos_ - obj.pos_) % cap_;
        }

        bool operator==(const iterator &it) const {
            return (start_ + pos_) % cap_ == (it.start_ + it.pos_) % cap_;
        }

        bool operator!=(const iterator &it) const {
            return (start_ + pos_) % cap_ != (it.start_ + it.pos_) % cap_;
        }

        bool operator<(const iterator &it) const {
            return (start_ + pos_) % cap_ < (it.start_ + it.pos_) % cap_;
        }

        bool operator<=(const iterator &it) const {
            return (start_ + pos_) % cap_ <= (it.start_ + it.pos_) % cap_;
        }

        bool operator>(const iterator &it) const {
            return (start_ + pos_) % cap_ > (it.start_ + it.pos_) % cap_;
        }

        bool operator>=(const iterator &it) const {
            return (start_ + pos_) % cap_ >= (it.start_ + it.pos_) % cap_;
        }

    private:
        T *iter_;
        size_t pos_;
        size_t cap_;
        size_t start_;
    };

    iterator begin() const {
        if (is_empty())
            return end();
        return iterator(data_, front_, capacity_, front_);
    }

    iterator end() const {
        return iterator(data_, (rear_ + 1) % capacity_, capacity_, front_);
    }

    // в пустом конструкторе создается 8 ячеек для буффера, 1 фиктивный
    explicit circular_buffer() {
        data_ = traits_::allocate(alloc_, capacity_);
    }

    // добавим фиктивный стул, на котором нельзя сидеть
    explicit circular_buffer(const size_t &capacity) : capacity_(capacity + 1) {
        data_ = traits_::allocate(alloc_, capacity_);
    }

    circular_buffer(const size_t &capacity, const T &value) : capacity_(capacity + 1) {
        data_ = traits_::allocate(alloc_, capacity_);
        for (auto it = data_; it != data_ + capacity_ - 1; ++it)
            traits_::construct(alloc_, it, value);
    }

    circular_buffer(const circular_buffer &buffer) : capacity_(buffer.capacity_), front_(buffer.front_),
                                                     rear_(buffer.rear_) {
        T *new_data = traits_::allocate(alloc_, capacity_);
        data_ = new_data;
    };

    ~circular_buffer() {
        if (!is_empty()) {
            int j = front_;
            while (j != (rear_ + 1) % capacity_) {
                traits_::destroy(alloc_, data_ + j);
                j = (j + 1) % capacity_;
            }
        }

        traits_::deallocate(alloc_, data_, capacity_);
    }

    void push_back(const T &value) {
        if (is_full() or is_empty())
            front_ = (front_ + 1) % capacity_;
        rear_ = (rear_ + 1) % capacity_;
        traits_::construct(alloc_, data_ + rear_, value);
    }

    void push_front(const T &value) {
        if (is_full()) {
            rear_ = (rear_ - 1) % capacity_;
        }
        if (is_empty()) {
            front_ += 2;
            rear_++;
        }
        front_ = (front_ - 1) % capacity_;
        traits_::construct(alloc_, data_ + front_, value);
    }

    void pop_front() {
        if (is_empty())
            throw std::out_of_range("You can't pop from empty buffer");

        traits_::destroy(alloc_, data_ + front_);
        if (front_ == rear_)
            front_ = rear_ = -1;
        else
            front_ = (front_ + 1) % capacity_;
    }

    void pop_back() {
        if (is_empty())
            throw std::out_of_range("You can't pop from empty buffer");

        traits_::destroy(alloc_, data_ + rear_);
        if (front_ == rear_)
            front_ = rear_ = -1;
        else
            rear_ = (rear_ - 1) % capacity_;
    }

    // первый элемент
    T &peek() const {
        if (is_empty())
            throw std::out_of_range("Buffer is empty");
        return *(data_ + front_);
    }

    // последний элемент
    T &foot() const {
        if (is_empty()) // исключение
            throw std::out_of_range("Buffer is empty");
        return *(data_ + rear_); // возвращать ссылку
    }

    bool is_empty() const {
        return front_ == -1;
    }

    bool is_full() const {
        return (rear_ + 2) % capacity_ == front_;
    }

    void reserve(size_t new_capacity) {

        new_capacity++;
        if (capacity_ < new_capacity) {

            T *new_data = traits_::allocate(alloc_, new_capacity);

            if (!is_empty()) {
                int i = 0;
                int j = front_;
                while (j != (rear_ + 1) % capacity_) {
                    traits_::construct(alloc_, new_data + i++, *(data_ + j));
                    j = (j + 1) % capacity_;
                }

                j = front_;
                while (j != (rear_ + 1) % capacity_) {
                    traits_::destroy(alloc_, data_ + j);
                    j = (j + 1) % capacity_;
                }

                rear_ = size() - 1;
                front_ = 0;
            }

            traits_::deallocate(alloc_, data_, capacity_);

            data_ = new_data;

            capacity_ = new_capacity;
        }
    }

    void resize(size_t new_size) {

        if (new_size < size()) {
            if (!is_empty()) {
                int j = (front_ + new_size) % capacity_;
                while (j != (rear_ + 1) % capacity_) {
                    traits_::destroy(alloc_, data_ + j);
                    j = (j + 1) % capacity_;
                }
                rear_ = (front_ + new_size - 1) % capacity_;
            }

            if (new_size == 0) {
                front_ = -1;
                rear_ = -1;
            }
        } else if (new_size > size()) {
            if (new_size <= capacity_ - 1) {
                int j = (rear_ + 1) % capacity_;
                while (j != (new_size + 1) % capacity_) {
                    traits_::construct(alloc_, data_ + j, T());
                    j = (j + 1) % capacity_;
                }
            } else {
                T *new_data = traits_::allocate(alloc_, new_size++);

                if (!is_empty()) {
                    int i = 0;
                    int j = front_;
                    while (j != (rear_ + 1) % capacity_) {
                        traits_::construct(alloc_, new_data + i++, *(data_ + j));
                        j = (j + 1) % capacity_;
                    }

                    j = front_;
                    while (j != (rear_ + 1) % capacity_) {
                        traits_::destroy(alloc_, data_ + j);
                        j = (j + 1) % capacity_;
                    }
                } else {
                    for (auto it = new_data; it != new_data + new_size - 1; it++)
                        traits_::construct(alloc_, new_data, T());
                }

                traits_::deallocate(alloc_, data_, capacity_);

                data_ = new_data;

                capacity_ = new_size;
            }
        }
    }

    void resize(size_t new_size, const T &value) {

        if (new_size < size()) {
            if (!is_empty()) {
                int j = (front_ + new_size) % capacity_;
                while (j != (rear_ + 1) % capacity_) {
                    traits_::destroy(alloc_, data_ + j);
                    j = (j + 1) % capacity_;
                }
                rear_ = (front_ + new_size - 1) % capacity_;
            }

            if (new_size == 0) {
                front_ = -1;
                rear_ = -1;
            }
        } else if (new_size > size()) {
            if (new_size <= capacity_ - 1) {
                int j = (rear_ + 1) % capacity_;
                while (j != (new_size + 1) % capacity_) {
                    traits_::construct(alloc_, data_ + j, value);
                    j = (j + 1) % capacity_;
                }
            } else {
                T *new_data = traits_::allocate(alloc_, new_size++);

                if (!is_empty()) {
                    int i = 0;
                    int j = front_;
                    while (j != (rear_ + 1) % capacity_) {
                        traits_::construct(alloc_, new_data + i++, *(data_ + j));
                        j = (j + 1) % capacity_;
                    }
                    for (auto it = new_data + size(); it != new_data + new_size - 1; it++)
                        traits_::construct(alloc_, new_data, value);

                    j = front_;
                    while (j != (rear_ + 1) % capacity_) {
                        traits_::destroy(alloc_, data_ + j);
                        j = (j + 1) % capacity_;
                    }
                } else {
                    for (auto it = new_data; it != new_data + new_size - 1; it++)
                        traits_::construct(alloc_, new_data, value);
                }

                traits_::deallocate(alloc_, data_, capacity_);

                data_ = new_data;

                capacity_ = new_size;
            }
        }
    }

    size_t capacity() const {
        return capacity_ - 1;
    }

    size_t size() const {
        if (is_full())
            return capacity_ - 1;
        if (is_empty())
            return 0;

        if (front_ <= rear_)
            return rear_ - front_ + 1;
        else
            return capacity_ - front_ + rear_ + 1;
    }

    T &operator[](const size_t &index) {
        return *(data_ + (front_ + index) % capacity_);
    }

    const T &operator[](const size_t &index) const {
        return *(data_ + (front_ + index) % capacity_);
    }

private:
    T *data_ = nullptr;
    Allocator alloc_;
    using traits_ = std::allocator_traits<decltype(alloc_)>;
    int front_ = -1;
    int rear_ = -1;
    size_t capacity_ = 9;
};

template<class T>
void info(const circular_buffer<T> &v) {
    std::cout << "Capacity: " << v.capacity() << "\n";
    std::cout << "Size: " << v.size() << "\n";
    std::cout << "Items: ";
    for (const auto &it : v)
        std::cout << it << " ";
    std::cout << "\n";
}

int main() {
    circular_buffer<int> test(3);
    test.push_back(6);
    test.push_back(7);
    test.push_back(8);
    test.pop_front();
    test.pop_back();
    info(test);
    test.push_back(9);
    info(test);

    auto it = min_element(test.begin(), test.end());
    std::cout << *it << ' ';

    it = max_element(test.begin(), test.end());
    std::cout << *it << ' ';

    it = find(test.begin(), test.end(), 7);
    std::cout << *it << ' ';

    std::reverse(test.begin(), test.end());

    for (it = test.begin(); it != test.end(); ++it)
        std::cout << *it << ' ';
    return 0;
}