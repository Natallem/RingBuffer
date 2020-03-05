#include <iostream>
#include <utility>
#include <type_traits>
#include "RingBuffer_lib/headers/RingBuffer.hpp"
template<class T>
void do_math() noexcept (std::is_copy_constructible<T>::value && noexcept( std::declval<T>()+std::declval<T>()) && std::is_assignable<T&,T>::value)
{
    std::cout << std::is_copy_constructible<T>::value << std::endl;
    std::cout << noexcept( std::declval<T>()+std::declval<T>()) << std::endl;
    std::cout << std::is_assignable<T&,T>::value<< std::endl;

    // тело функции нужно оставить пустым
}
template<class T>
void dod() noexcept (std::is_copy_constructible<T>::value && noexcept( std::declval<T>()+std::declval<T>()) && std::is_assignable<T&,T>::value){
    std::cout << std::is_copy_constructible<T>::value;
    std::is_assignable<T,T>::value;
}
int main() {
//    std::recursive_mutex m;

    RingBuffer buffer;
    
    buffer.addByte(4);
//    std::lock_guard<std::recursive_mutex> lockGuard(m);

   std::cout << (int) buffer.readByte();


}