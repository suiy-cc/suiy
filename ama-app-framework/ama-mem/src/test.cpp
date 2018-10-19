/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <iostream>
#include <new>

//Be sure to put it at the end of statement
#include "ama_mem.h"

struct MyClass {
  int data[100] = {0};
  MyClass() {std::cout << "constructed [" << this << "]\n";}
};

static void debug_malloc_detail(
                const char* file, int line, size_t size, void* user_data)
{
  printf("[%s:%d]%ld\n", file, line, size);
}

int main ()
{

#ifdef _DEBUG_MEMORY_

  std::cout << "1: ";
  MyClass * p1 = new MyClass;
  printf("memory summary: %d\n", debug_malloc_summary());

  std::cout << "constructions (1): ";
  MyClass * ap1 = new MyClass[5];
  std::cout << '\n';

  printf("memory summary: %d\n", debug_malloc_summary());

  char *p2 = (char*)ama_malloc(sizeof(char) * 200);
  printf("memory summary: %d\n", debug_malloc_summary());

  debug_malloc_summary_details(debug_malloc_detail, NULL);

  delete p1;
  printf("memory summary: %d\n", debug_malloc_summary());
  delete[] ap1;
  printf("memory summary: %d\n", debug_malloc_summary());

  ama_free(p2);
  printf("memory summary: %d\n", debug_malloc_summary());


#else

  printf("memory debug not enabled.\n");

#endif

  return 0;
}
