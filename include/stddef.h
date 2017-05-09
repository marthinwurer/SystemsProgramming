#ifndef _STDDEF_H
#define _STDDEF_H

#ifndef NULL
#define NULL ((void*)0)
#endif

// since we're using gcc we can use the builtin
#define offsetof(st, m) __builtin_offsetof(st, m)

typedef unsigned int size_t;
typedef int ptrdiff_t; // for pointer math

#endif
