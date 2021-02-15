#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Computes b**x. Note: O(1) space, O(n) time.
unsigned long ipow(int b, int x);

// ASCII is essentially just base-128. This converts a
// string of base-10 ASCII digits into a radix-128 notation integer.
unsigned long to_radix(char *str);

void err(char *emsg);

#endif
