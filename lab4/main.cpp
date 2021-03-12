#include <iostream>

template<typename Iterator, typename Predicate>
bool all_of(Iterator first, Iterator last, Predicate pred) {
    while (first != last) {
        if (!pred(*first)) return false;
        ++first;
    }
    return true;
}

template<typename Iterator, typename Predicate>
bool any_of(Iterator first, Iterator last, Predicate pred) {
    while (first != last) {
        if (pred(*first)) return true;
        ++first;
    }
    return false;
}

template<typename Iterator, typename Predicate>
bool none_of(Iterator first, Iterator last, Predicate pred) {
    while (first != last) {
        if (pred(*first)) return false;
        ++first;
    }
    return true;
}

template<typename Iterator, typename Predicate>
bool one_of(Iterator first, Iterator last, Predicate pred) {
    int counter = 0;
    while (first != last) {
        if (pred(*first)) counter++;
        if (counter > 1) return false;
        ++first;
    }
    if (counter == 1) return true; else return false;
}

template <typename Iterator>
bool is_sorted (Iterator first, Iterator last)
{
    if (first==last) return true;
    Iterator next = first;
    while (++next!=last) {
        if (*next<*first)
            return false;
        ++first;
    }
    return true;
}

template <typename Iterator, typename Predicate>
bool is_partitioned (Iterator first, Iterator last, Predicate pred)
{
    while (first!=last && pred(*first)) {
        ++first;
    }
    while (first!=last) {
        if (pred(*first)) return false;
        ++first;
    }
    return true;
}

template<class Iterator, class Predicate>
Iterator find_not (Iterator first, Iterator last, Predicate pred)
{
    while (first!=last) {
        if (!pred(*first)) return first;
        ++first;
    }
    return last;
}

template<class Iterator, class Predicate>
Iterator find_backward (Iterator first, Iterator last, Predicate pred)
{
    while (last!=first) {
        if (pred(*last)) return last;
        --last;
    }
    return first;
}

template<class Iterator, class Predicate>
bool is_palindrome (Iterator first, Iterator last, Predicate pred)
{
    while (first<last) {
        if (pred(*first) != pred(*last)) return false;
        --last;
        ++first;
    }
    return true;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
