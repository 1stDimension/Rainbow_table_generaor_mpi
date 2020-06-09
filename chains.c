#include <stdio.h>
#include <openssl/sha.h>

int hash(uint8_t *input, size_t n, uint8_t *output)
{
  SHA1(input, n, output);
}
int reduction(uint8_t *input, size_t n, uint8_t *output, size_t length)
{
  //get first length character of input
  // memcpy();
  for (int i = 0; i < length; i++)
  {
    sprintf(output + 2 * i, "%x", input[i]);
  }
  // char *format = "%2x";
  // sprintf(output, "");
}