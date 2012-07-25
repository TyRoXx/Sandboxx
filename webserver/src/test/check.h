#ifndef TEST_CHECK_H
#define TEST_CHECK_H


#include <stdio.h>


#define TEST_EXPECT(x) if (!(x)) { fprintf(stderr, "Failed (%s:%u): %s\n", __FILE__, __LINE__, #x); }


#endif
