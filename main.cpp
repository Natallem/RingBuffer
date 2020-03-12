/**
  ******************************************************************************
  * @file main.cpp
  * @author  Vasiliy Turchenko
  * @version V0.0.1
  * @date    12-Mar-2020
  * @brief
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT STC </center></h2>
  ******************************************************************************
  */

#include <iostream>
#include <string>

#include "RingBuffer.hpp"
#include "RingBufferError.hpp"

int main()
{
  std::cout << "Test started..\n";

  RingBuffer a(10);
  std::cout << "Step 1. Buffer a has size " << a.size() << " bytes\n";
  std::cout << "Step 1. Buffer a has capacity " << a.getCapacity() << " bytes\n";

  /* let's insert 4 bytes */
  std::vector<char> first = {'a', 'b', 'c', 'd'};

  a.addAllBytes(first);
  std::cout << "Step 2. Adding " << first.size() << " bytes\n";
  std::cout << "Step 2. Buffer a has size " << a.size() << " bytes\n";
  std::cout << "Step 2. Buffer a has capacity " << a.getCapacity() << " bytes\n";

  std::cout << "Step 3. Try to read " << (a.getCapacity() + 1) << " bytes\n";

  std::vector<char> second = a.readSomeBytes(a.getCapacity() + 1);

/* caught an exception here instead of reading 4 bytes */


  std::cout << "Test finished.\n";
}
