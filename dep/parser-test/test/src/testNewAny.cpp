#include <iostream>
#include <map>
#include <vector>
#include <Any.hpp>
#include <UniquePtr.hpp>

int main() {
    UniquePtr<int> kk(42);
    std::cout << *kk.data << std::endl;

    std::vector<int> ll;
    ll.push_back(15);
    ll.push_back(23);
    ll.push_back(46);
    ll.push_back(85);
    ll.push_back(100);
    UniquePtr<std::vector<int> > tt(ll);
    std::vector<int> hh = *tt.data;
    for (std::vector<int>::const_iterator iter = hh.begin(); iter != hh.end(); ++iter) {
        std::cout << "V: " << *iter << std::endl;
    }

    UniquePtr<Any> any(AnyType<int>(42));
    std::cout << (int)*any.data << std::endl;
}