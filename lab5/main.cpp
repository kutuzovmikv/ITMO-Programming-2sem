#include <algorithm>
#include <iostream>
#include <initializer_list>
#include <algorithm>

template<class T, class Allocator = std::allocator<T>>
class circular_buffer {
public:
    class iterator : public std::iterator<std::random_access_iterator_tag, T> {
    public:
        using difference_type = typename std::iterator<std::random_access_iterator_tag, T>::difference_type;

        difference_type operator-(const iterator &obj) const {
            return _data - obj._data;
        }

        explicit iterator(T *start) {
            _data = start;
        }

        iterator(const iterator &obj) {
            _data = obj._data;
        }

        ~iterator() = default;

        T &operator*() const{
            return *(_data);
        }

        T *operator->() const{
            return _data;
        }

        iterator operator++() {
            ++_data;
            return *this;
        }

        iterator operator--() {
            --_data;
            return *this;
        }

        iterator operator+(int value) {
            _data = _data + value;
            return *this;
        }

        iterator operator-(int value) {
            _data = _data - value;
            return *this;
        }

        bool operator==(const iterator &it) const {
            return _data == it._data;
        }

        bool operator!=(const iterator &it) const {
            return _data != it._data;
        }

        bool operator<(const iterator &it) const {
            return _data < it._data;
        }

        bool operator<=(const iterator &it) const {
            return _data <= it._data;
        }

        bool operator>(const iterator &it) const {
            return _data > it._data;
        }

        bool operator>=(const iterator &it) const {
            return _data >= it._data;
        }

    private:
        T *_data = nullptr;
    };

    [[nodiscard]] iterator begin() const {
        return iterator(_data);
    }

    [[nodiscard]] iterator end() const {
        return iterator(_data + _size);
    }

    [[nodiscard]] size_t capacity() const {
        return this->_capacity;
    }

    [[nodiscard]] size_t size() const {
        return this->_size;
    }

    explicit circular_buffer(const size_t &capacity) : _capacity(capacity) {
        _data = traits_::allocate(alloc_, _capacity);
    }

    circular_buffer(const circular_buffer &buffer)
            : _size(buffer._size),
              _capacity(buffer._capacity),
              _data(buffer._data), alloc_(buffer.alloc_) {};

    ~circular_buffer() {
        for (auto it = _data; it != _data + _capacity; ++it)
            traits_::destroy(alloc_, it);
        traits_::deallocate(alloc_, _data, _capacity);
    }

    void push_front(const T &value) {
        if (_size < _capacity) {
            for (size_t i = _size; i >= 1; i--)
                traits_::construct(alloc_, _data + i, *(_data + i - 1));
            traits_::construct(alloc_, _data, value);
            _size++;
        } else {
            for (size_t i = _capacity - 1; i >= 1; i--)
                traits_::construct(alloc_, _data + i, *(_data + i - 1));
            traits_::construct(alloc_, _data, value);
        }
    }

    void pop_front() {
        for (size_t i = 0; i < _size - 1; i++)
            traits_::construct(alloc_, _data + i, *(_data + i + 1));
        _size--;
    }

    void push_back(const T &value) {
        if (_size < _capacity) {
            traits_::construct(alloc_, _data + _size, value);
            _size++;
        } else {
            for (size_t i = 1; i < _capacity; i++)
                traits_::construct(alloc_, _data + i - 1, *(_data + i));
            traits_::construct(alloc_, _data + _capacity - 1, value);
        }
    }

    void pop_back() {
        if (_size > 0)
            _size--;
    }

    void insert(const size_t &it, const T &value) {
        traits_::construct(alloc_, _data + (it % _capacity), value);
    }

    void insert(iterator it, const T &value) {
        *(begin() + (it - begin()) % _size) = value;
    }

    void remove(const size_t &it) {
        for (size_t i = it + 1; i < _size; i++)
            traits_::construct(alloc_, _data + (i - 1) % _capacity, *(_data + (i % _capacity)));
        _size--;
    }

    void resize(const size_t &new_capacity) {
        if (new_capacity < _capacity) {

            for (auto it = _data + new_capacity; it != _data + _capacity; ++it)
                traits_::destroy(alloc_, it);

            _capacity = new_capacity;

            if (_size > new_capacity) {
                _size = new_capacity;
            }
        } else if (new_capacity > _capacity) {

            T *new_data = traits_::allocate(alloc_, new_capacity);
            for (auto it = _data; it != _data + _capacity; ++it)
                traits_::construct(alloc_, new_data, *it);

            for (auto it = _data; it != _data + _capacity; ++it)
                traits_::destroy(alloc_, it);
            traits_::deallocate(alloc_, _data, _capacity);

            _data = new_data;
        }
    }

    void resize(const size_t &new_capacity, const T &value) {
        // если меньше - задестроить элементы за размером, такой же ниче, если больше - добавляем дефолт, либо впихиваем больше

        if (new_capacity < _capacity) {

            for (auto it = _data + new_capacity; it != _data + _capacity; ++it)
                traits_::destroy(alloc_, it);

            if (_size > new_capacity) {
                _size = new_capacity;
            }
        } else if (new_capacity > _capacity) {

            T *new_data = traits_::allocate(alloc_, new_capacity);
            for (auto it = _data; it != _data + _capacity; ++it)
                traits_::construct(alloc_, new_data, *it);
            for (auto it = _data + _capacity; it != _data + new_capacity; ++it)
                traits_::construct(alloc_, new_data, value);

            for (auto it = _data; it != _data + _capacity; ++it)
                traits_::destroy(alloc_, it);
            traits_::deallocate(alloc_, _data, _capacity);

            _size = new_capacity;
            _data = new_data;
        }

        _capacity = new_capacity;
    }

    T operator[](size_t it) {
        return _data[it % _capacity];
    }

private:
    size_t _size = 0;
    size_t _capacity = 0;
    T *_data = nullptr;
    Allocator alloc_;
    using traits_ = std::allocator_traits<decltype(alloc_)>;
};

template<class T>
void info(const circular_buffer<T> &v) {
    std::cout << "Capacity: " << v.capacity() << "\n";
    std::cout << "Size: " << v.size() << "\n";
    std::cout << "Items: ";
    for (const auto &it : v)
        std::cout << it << ' ';
    std::cout << "\n";
}

int main() {
    circular_buffer<int> test(3);
    test.push_front(5);
    test.push_back(1);
    test.push_back(24);
    test.pop_front();
    std::sort(test.begin(), test.end());
    info(test);
    return 0;
}