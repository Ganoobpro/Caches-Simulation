#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// Type aliases
typedef long long ll;
typedef unsigned long long ull;
typedef long double ld;

typedef uint8_t byte;
typedef uint32_t AddressType;

// Predicates
#define success            0 ==
#define failure            0 !=
#define fail               0 !=
#define even               0 == 1 &
#define odd                1 == 1 &
#define positive           0 <
#define negative           0 >
#define zero               0 ==
#define empty              NULL ==
#define null               NULL ==

// Ternaries and conditionals.
#define ifnot(...)   if(!(__VA_ARGS__))
#define elif         else if
#define elifnot(...) else if(!(__VA_ARGS__))
#define when
#define then        ?
#define other       :
#define otherwise   :
#define otherwhen   :
#define only        : NULL

// Loops and blocks. Lisp, Lua, Ruby.
#define until(...)   while(!(__VA_ARGS__))
#define always       while(1)
#define forever      while(1)
#define loop         while(1)
#define indefinitely while(1)
#define never        while(0)
#define comment      while(0)
#define repeat       do
#define done         break
#define finish       break
#define pass         continue

// Short typing
#define between(a, x, b)    ((a) <= (x) && (x) <= (b))
#define in_range(a, x, b)   ((a) < (x) && (x) < (b))

#define all(x)  (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
