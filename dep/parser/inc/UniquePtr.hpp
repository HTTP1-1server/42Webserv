#pragma once

template <class T>
class UniquePtr {
public:
    T *data;

    template <class U> UniquePtr(const U &rhs);
    ~UniquePtr();
};

template <class T> template <class U>
UniquePtr<T>::UniquePtr(const U &rhs): data(new U(rhs)) {

}

template <class T>
UniquePtr<T>::~UniquePtr() {
    if (this->data)
        delete this->data;
}