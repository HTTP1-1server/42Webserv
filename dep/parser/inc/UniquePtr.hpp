#pragma once

template <class T>
class UniquePtr {
public:
    T *data;
    int *ref_count;

    UniquePtr(const UniquePtr &rhs);
    template <class U> UniquePtr(const U &rhs);
    ~UniquePtr();
};

template <class T>
UniquePtr<T>::UniquePtr(const UniquePtr<T> &rhs): data(rhs.data), ref_count(rhs.ref_count) {
    ++(*this->ref_count);
}

template <class T> template <class U>
UniquePtr<T>::UniquePtr(const U &rhs): data(new U(rhs)), ref_count(new int(1)) {
}

template <class T>
UniquePtr<T>::~UniquePtr() {
    --(*this->ref_count);
    if (this->data && this->ref_count && *this->ref_count == 0) {
        delete this->data;
        delete this->ref_count;
    }
}