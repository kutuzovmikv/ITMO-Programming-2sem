#include <iostream>
#include <memory>
#include <algorithm>
#include <cmath>

template<class T, class Allocator = std::allocator<T>>
class circular_buffer {
private:
    T *data_ = nullptr;
    Allocator alloc_;
    using traits_ = std::allocator_traits<decltype(alloc_)>;
    int front_ = -1;
    int rear_ = -1;
    size_t capacity_ = 8;
public:
    explicit circular_buffer(const size_t &capacity) : capacity_(capacity) {
        data_ = traits_::allocate(alloc_, capacity_);
    }

    circular_buffer(const size_t &capacity, const T &value) : capacity_(capacity) {
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
            resize();
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
        return (rear_ + 1) % capacity_ == front_;
    }

    //коли суши-бар заполнен - пора его расширять, ведь терять клиентов не выгодно
    //в этом resize() мы не знаем точное новое количество клиентов, так что тупо принесем в 2 раза больше стульев
    void resize() {
        T *new_data = traits_::allocate(alloc_, capacity_);
        int i = 0;
        int j = front_;

        do {
            traits_::construct(alloc_, new_data + i++, *(data_ + j));
            j = (j + 1) % capacity_;
        } while (j != front_);

        for (auto it = data_; it != data_ + capacity_; ++it)
            traits_::destroy(alloc_, it);
        traits_::deallocate(alloc_, data_, capacity_);

        front_ = 0;
        rear_ = capacity_ - 1;
        capacity_ *= 2;
        data_ = new_data;
    }

    // если меньше - задестроить элементы за размером, такой же ниче, если больше - добавляем дефолт, либо впихиваем больше
    void resize(const size_t &new_capacity, const T &value) {

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
    void resize(const size_t &new_capacity) {

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
        return capacity_;
    }

    size_t size() const {
        if (is_full())
            return capacity_;

        if (front_ <= rear_)
            return rear_ - front_;
        else
            return capacity_ - front_ + rear_;
    }

    T operator[](const size_t &index) const {
        return *(data_ + index % capacity_);
    }
};

template<class T>
void info(const circular_buffer<T> &v) {
    std::cout << "Capacity: " << v.capacity() << "\n";
    std::cout << "Size: " << v.size() << "\n";
    std::cout << "Items: ";
    /*for (const auto &it : v)
        std::cout << it << ' ';*/
    std::cout << "\n";
}

int main() {
    circular_buffer<int> test(5);
    test.push_back(-1);
    test.push_back(0);
    test.push_back(1);
    test.push_back(2);
    test.push_back(3);
    test.push_back(4);
    test.push_back(5);
    std::cout << test[0] << "\n";
    std::cout << test[1] << "\n";
    std::cout << test[2] << "\n";
    std::cout << test[3] << "\n";
    std::cout << test[4] << "\n";
    std::cout << test[5] << "\n";
    std::cout << test[6] << "\n";
    //std::sort(test.begin(), test.end());
    info(test);
    return 0;
}