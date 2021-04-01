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
    return !any_of(first, last, pred);
}

template<typename Iterator, typename Predicate>
bool one_of(Iterator first, Iterator last, Predicate pred) { //flag
    enum Colors {
        none = 0,
        one,
        more
    };

    Colors flag(none);
    while (first != last) {
        if (pred(*first)) flag++;
        if (flag == more) return false;
        ++first;
    }
    if (flag == one) return true; else return false;
}

template<typename ForwardIt, typename Compare>
bool is_sorted(ForwardIt first, ForwardIt last, Compare comp) {
    if (first != last) {
        ForwardIt next = first;
        while (++next != last) {
            if (comp(*next, *first))
                return false;
            first = next;
        }
    }
    return true;
}

template<typename Iterator, typename Predicate>
bool is_partitioned(Iterator first, Iterator last, Predicate pred) {
    while (first != last && pred(*first)) {
        ++first;
    }
    while (first != last) {
        if (pred(*first)) return false;
        ++first;
    }
    return true;
}

template<typename Iterator, typename Predicate>
Iterator find_not(Iterator first, Iterator last, Predicate pred) {
    while (first != last) {
        if (!pred(*first)) return first;
        ++first;
    }
    return last;
}

template<typename Iterator, typename Predicate>
Iterator find_backward(Iterator first, Iterator last, Predicate pred) {
    if (first == last)
        return first;
    --last;
    while (last != first) {
        if (pred(*last)) return last;
        --last;
    }
    return first;
}

template<typename Iterator, typename Predicate>
bool is_palindrome(Iterator first, Iterator last, Predicate pred) {
    if (first == last)
        return true;
    --last;
    while (first < last) {
        if (pred(*first) != pred(*last)) return false;
        --last;
        ++first;
    }
    return true;
}
