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

  //let's assume
  size_t password_length = 4;

  uint8_t *new_reduce_value = NULL;
  new_reduce_value = malloc((password_length + 1) * sizeof(*new_reduce_value));

  hash(ibuf, strlen(ibuf), obuf);
  reduction(obuf, 20, new_reduce_value, password_length);

  int i;
  for (i = 0; i < 20; i++)
  {
    printf("%x", obuf[i]);
  }
  printf("\n");
  printf("reduced:\n");
  for (i = 0; i < password_length; i++)
  {
    printf("%c", new_reduce_value[i]);
  }
  printf("\n");

  return 0;
}