#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/sha.h>

void hash(uint8_t *input, size_t n, uint8_t *output)
{
  SHA1(input, n, output);
}
void reduction(uint8_t *input, size_t n, uint8_t *output, size_t length)
{
  //get first length character of input
  // for (int i = 0; i < length; i++)
  // {
  // sprintf(output + 2 * i, "%x", input[i]);
  // }
  //take length first bytes of input
  //divide by 2 then for each bite make bitwise and with 128
  memcpy(output, input, length);
  for (int i = 0; i < length; i++)
  {
    output[i] %= 95;
    output[i] += 32;
  }
}