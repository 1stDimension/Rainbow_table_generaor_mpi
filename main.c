#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "chains.h"

// hash_value
// new_hash_value
// reduce_value
// new_reduce_value

int main()
{
  unsigned char ibuf[] = "compute sha1";
  uint8_t obuf[20];

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