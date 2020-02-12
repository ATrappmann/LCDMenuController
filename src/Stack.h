// NAME: Stack.h
//
// DESC: Simple stack implementation.
//
// This file is part of the LCDMenuController library for the Arduino environment.
//
#ifndef STACK_H
#define STACK_H

template<class T>
class Stack {
private:
  int maxSize;
  T   **stack;
  int stackPtr;

public:
  Stack(const int stackSize) {
    maxSize = stackSize;
    stack = new T*[maxSize];
    stackPtr = 0;
  }

  virtual ~Stack() {
    delete[] stack;
    stack = NULL;
  }

  void push(T* obj) {
    if (stackPtr < maxSize) {
      stack[stackPtr++] = obj;
    }
  }

  T* pop() {
    if (stackPtr > 0) {
      return stack[--stackPtr];
    }
    else return NULL;
  }

  int getMaxSize() {
    return maxSize;
  }

};

#endif /* STACK_H */
