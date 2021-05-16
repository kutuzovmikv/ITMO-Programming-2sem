#include <iostream>
#include <memory>
#include <algorithm>
#include <cmath>

template<class T, class Allocator = std::allocator<T>>
class circular_buffer {
public:

    class iterator : public std::iterator<std::random_access_iterator_tag, T> {
    public:
        explicit iterator(T *start, const size_t &pos, const size_t &cap) : iter_(start), pos_(pos), cap_(cap) {};

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

        iterator operator+(int value) {
            pos_ = (pos_ + value) % cap_;
            return *this;
        }

        iterator operator-(int value) {
            pos_ = (pos_ - value) % cap_;
            return *this;
        }

        using difference_type = typename std::iterator<std::random_access_iterator_tag, T>::difference_type;

        difference_type operator-(const iterator &obj) const {
            return (pos_ - obj.pos_) % cap_;
        }

        bool operator==(const iterator &it) const {
            return iter_ + pos_ == it.iter_ + it.pos_;
        }

        bool operator!=(const iterator &it) const {
            return iter_ + pos_ != it.iter_ + it.pos_;
        }

        bool operator<(const iterator &it) const {
            return iter_ + pos_ < it.iter_ + it.pos_;
        }

        bool operator<=(const iterator &it) const {
            return iter_ + pos_ <= it.iter_ + it.pos_;
        }

        bool operator>(const iterator &it) const {
            return iter_ + pos_ > it.iter_ + it.pos_;
        }

        bool operator>=(const iterator &it) const {
            return iter_ + pos_ >= it.iter_ + it.pos_;
        }

    private:
        T *iter_;
        size_t pos_;
        size_t cap_;
    };

    iterator begin() const {
        if (is_empty())
            return iterator(data_, front_ + 1, capacity_);
        return iterator(data_, front_, capacity_);
    }

    iterator start() {
        if (is_empty()) {
            front_++;
            rear_++;
            return iterator(data_, front_, capacity_);
        }
        return iterator(data_, 0, capacity_);
    }

    iterator end() const {
        return iterator(data_, (rear_ + 1) % capacity_, capacity_);
    }

    // добавим фиктивный стул, на котором нельзя сидеть
    explicit circular_buffer(const size_t &capacity) : capacity_(capacity + 1) {
        data_ = traits_::allocate(alloc_, capacity_);
    }

    circular_buffer(const size_t &capacity, const T &value) : capacity_(capacity + 1) {
        data_ = traits_::allocate(alloc_, capacity_);
        for (auto it = data_; it != data_ + capacity_; ++it)
            traits_::construct(alloc_, it, value);
    }

    circular_buffer(const circular_buffer &buffer)
            : data_(buffer.data_), capacity_(buffer.capacity_), front_(buffer.front_), rear_(buffer.rear_),
              alloc_(buffer.alloc_) {};

    ~circular_buffer() {
        for (auto it = data_; it != data_ + capacity_; ++it)
            traits_::destroy(alloc_, it);
        traits_::deallocate(alloc_, data_, capacity_);
    }

    void push_back(const T &value) {
        if (is_full())
            shrink_to_fit(); // чтобы не терять бабки - увеличим размер бара
        else if (is_empty())
            front_++;
        rear_ = (rear_ + 1) % capacity_;
        traits_::construct(alloc_, data_ + rear_, value);
    }

    // так как кольцевой буфер - это очередь в суши-бар, то чтобы бармен не потерял сознание он будет поворачиваться только вправо.
    // для получения первого элемента есть функция peek()
    void push_front(const T &value) {
        if (is_empty()) {
            rear_++;
            front_++;
        }
        traits_::construct(alloc_, data_ + front_, value);
    }

    void pop_front() {
        if (is_empty())
            return;

        traits_::destroy(alloc_, data_ + front_);
        if (front_ == rear_)
            front_ = rear_ = -1;
        else
            front_ = (front_ + 1) % capacity_;
    }

    // если очередь не дошла, то можно уступить, но логично, что на твое место придет новый человек
    void pop_back() {
        if (is_empty())
            return;

        traits_::destroy(alloc_, data_ + rear_);
    }

    // первый элемент
    T peek() const {
        if (is_empty())
            return T();
        return *(data_ + front_);
    }

    // последний элемент
    T foot() const {
        if (is_empty())
            return T();
        return *(data_ + rear_);
    }

    bool is_empty() const {
        return front_ == -1;
    }

    bool is_full() const {
        return (rear_ + 2) % capacity_ == front_;
    }

    void insert(iterator it, T value) {
        *(start() + (it - start()) % capacity_) = value;
    }

    //коли суши-бар заполнен - пора его расширять, ведь терять клиентов не выгодно
    //в этом resize() мы не знаем точное новое количество клиентов, так что тупо принесем в 2 раза больше стульев
    void shrink_to_fit() {
        T *new_data = traits_::allocate(alloc_, capacity_ * 2 - 1);
        int i = 0;
        int j = front_;

        do {
            traits_::construct(alloc_, new_data + i++, *(data_ + j));
            j = (j + 1) % (capacity_ - 1);
        } while (j != front_);

        for (auto it = data_; it != data_ + capacity_; ++it)
            traits_::destroy(alloc_, it);
        traits_::deallocate(alloc_, data_, capacity_);

        front_ = 0;
        rear_ = capacity_ - 2;
        capacity_ = capacity_ * 2 - 1;
        data_ = new_data;
    }

    // если меньше - задестроить элементы за размером, такой же ниче, если больше - добавляем дефолт, либо впихиваем больше
    void resize(size_t &new_capacity, const T &value) {

        new_capacity++;
        if (new_capacity < capacity_) {

            for (auto it = data_ + new_capacity; it != data_ + capacity_; ++it)
                traits_::destroy(alloc_, it);

            if (front_ > new_capacity && rear_ > new_capacity)
                front_ = rear_ = -1;
            else if (rear_ > new_capacity)
                rear_ = new_capacity - 1;
            else if (front_ > new_capacity)
                front_ = 0;

        } else if (new_capacity > capacity_) {

            T *new_data = traits_::allocate(alloc_, new_capacity);
            int i = 0;
            int j = front_;

            do {
                traits_::construct(alloc_, new_data + i++, *(data_ + j));
                j = (j + 1) % capacity_;
            } while (j != front_);

            do {
                traits_::construct(alloc_, new_data + i++, value);
            } while (i != new_capacity);

            for (auto it = data_; it != data_ + capacity_; ++it)
                traits_::destroy(alloc_, it);
            traits_::deallocate(alloc_, data_, capacity_);

            rear_ = size() - 1;
            front_ = 0;
            data_ = new_data;
        }

        capacity_ = new_capacity;
    }

    // если меньше - задестроить элементы за размером, такой же ниче, если больше - добавляем дефолт, либо впихиваем больше
    void resize(size_t &new_capacity) {

        new_capacity++;
        if (new_capacity < capacity_) {

            for (auto it = data_ + new_capacity; it != data_ + capacity_; ++it)
                traits_::destroy(alloc_, it);

            if (front_ > new_capacity && rear_ > new_capacity)
                front_ = rear_ = -1;
            else if (rear_ > new_capacity)
                rear_ = new_capacity - 1;
            else if (front_ > new_capacity)
                front_ = 0;

        } else if (new_capacity > capacity_) {

            T *new_data = traits_::allocate(alloc_, new_capacity);
            int i = 0;
            int j = front_;

            do {
                alloc_.construct(new_data + i++, *(data_ + j));
                j = (j + 1) % capacity_;
            } while (j != front_);

            for (auto it = data_; it != data_ + capacity_; ++it)
                traits_::destroy(alloc_, it);
            traits_::deallocate(alloc_, data_, capacity_);

            rear_ = size() - 1;
            front_ = 0;
            data_ = new_data;
        }

        capacity_ = new_capacity;
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
            return capacity_ - front_ + rear_;
    }

    T operator[](const size_t &index) const {
        return *(data_ + index % capacity_);
    }

private:
    T *data_ = nullptr;
    Allocator alloc_;
    using traits_ = std::allocator_traits<decltype(alloc_)>;
    int front_ = -1;
    int rear_ = -1;
    size_t capacity_ = 8;
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
    info(test);
    test.push_back(9);
    info(test);

    auto it = min_element(test.begin(), test.end());
    std::cout << *it << ' ';
    it = max_element(test.begin(), test.end());
    std::cout << *it << ' ';
    it = find(test.begin(), test.end(), 7);
    std::cout << *it << ' ';
    test.insert(it, 24);
    info(test);

    std::reverse(test.begin(), test.end());

    for (it = test.begin(); it != test.end(); ++it)
        std::cout << *it << ' ';
    return 0;
}