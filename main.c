#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "chains.h"

#define SWAP(a, b) \
  {                \
    typeof(a) t;   \
    t = a;         \
    a = b;         \
    b = t;         \
  }
// hash_value
// new_hash_value
// reduce_value
// new_reduce_value
int ipow(int base, int exp)
{
  int result = 1;
  for (;;)
  {
    if (exp & 1)
      result *= base;
    exp >>= 1;
    if (!exp)
      break;
    base *= base;
  }
  return result;
}

int main()
{
  const char *start = "     ";
  int step = 20000000;
  // 32 - 126 = 94; podzielić na 94ki
  int length = strlen(start);
  char *begining = malloc((length + 1) * sizeof(*begining));
  memcpy(begining, start, length);
  char *next = malloc((length + 1) * sizeof(*next));
  next[length] = 0;
  int min = 32;
  int max = 126;
  int range = 95;
  int remainder = 0;
  int max_step = range - 1;
  int times = 9000;
  for (int j = 0; j < times; j++)
  {
    for (int i = 0; i < length; i++)
    {
      int power = ipow(range, i + 1);
      int tmp = step % power;
      char this_step = tmp / ipow(range, i);
      this_step += remainder;
      remainder = 0;
      // printf("DEBUG: begining =\"%s\"\n", begining);
      // printf("DEBUG: next =\"%s\"\n", next);
      char test = begining[length - i - 1] + this_step;
      // potensiall overflow problem
      //Max step is also the bigest possible value in one column
      if (test > (max_step + min) || test < begining[length - i - 1])
      {
        remainder = 1;
        test -= range;
      }
      // }

      next[length - i - 1] = test;
    }
    if (remainder != 0)
    {
      printf("after loop non zero remainder\n");
      break;
    }
    else
    {
      uint8_t computed_hash[20];
      uint8_t reduced[length];

      printf("hash = ");
      hash(next, length, computed_hash);
      for (int i = 0; i < 20; i++)
        printf("%02x", computed_hash[i]);
      printf("\n");

      printf("reduction = \"");
      reduction(computed_hash, 20, reduced, length);
      for (int i = 0; i < length; i++)
        printf("%c", (int)reduced[i]);
      printf("\"\n");
    }
    // printf("begining =\"%s\"\n", begining);
    printf("next      =\"%s\"\n", next);
    // printf("next(char)=");
    // for (int i = 0; i < length; i++)
    // {
    // printf("%2c|", next[i]);
    // }
    // printf("\n");
    // printf("next(int)= ");
    // for (int i = 0; i < length + 1; i++)
    // {
    // printf("%2d|", next[i]);
    // }
    // printf("\n");
    // printf("sizeof char* = %ld\nsizeof uint64_t %zu\n", sizeof(char *), sizeof(uint64_t));
    SWAP(next, begining);
  }
  printf("start     =\"%s\"\n", start);
  printf("last     =\"%s\"\n", next);
  printf("hash = ");
  // printf("next (int)=\"");
  // for (int i = 0; i < length + 1; i++)
  // {
  // printf(" %d ", next[i]);
  // }
  // printf("\"\n");
  // printf("next = %s\n", next);

  return 0;
}