#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/sha.h>

void hash(uint8_t *input, size_t n, uint8_t *output)
{
  SHA1(input, n, output);
}

// input and output should be pre allocated
void reduction(uint8_t *input, size_t n, uint8_t *output, size_t length)
{
  //get first length character of input
  //take length first bytes of input
  //divide by 2 then for each bite make bitwise and with 128
  //needs error checking
  memcpy(output, input, length);
  for (int i = 0; i < length; i++)
  {
    output[i] %= 95;
    output[i] += 32;
  }
}