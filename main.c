#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <mpi.h>
#include "chains.h"

#define CVECTOR_LOGARITHMIC_GROWTH
#define HASH_LENGTH 20
#include "c-vector/cvector.h"

#define MASTER_ID 0

#define SWAP(a, b) \
  {                \
    typeof(a) t;   \
    t = a;         \
    a = b;         \
    b = t;         \
  }
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

int chain(int chain_length, char *input, int password_length, char *output, int outputlength)
{
  uint8_t reduced[password_length];

  hash(input, password_length, output);
  chain_length--;
  for (int j = 0; j < chain_length; j++)
  {
    reduction(output, HASH_LENGTH, reduced, password_length);
    hash(reduced, password_length, output);
  }
}

//if returns != there was overflow
// begining and next should be pre-allocated
int get_next(int step, int min, int max, int range, int length, int remainder, char *begining, char *next)
{
  int max_step = range - 1;
  for (int i = 0; i < length; i++)
  {
    int power = ipow(range, i + 1);
    int tmp = step % power;
    char this_step = tmp / ipow(range, i);
    this_step += remainder;
    remainder = 0;
    char test = begining[length - i - 1] + this_step;
    // potensiall overflow problem
    //Max step is also the bigest possible value in one column
    if (test > (max_step + min) || test < begining[length - i - 1])
    {
      remainder = 1;
      test -= range;
    }
    next[length - i - 1] = test;
  }
  return remainder;
}

int handle_input(int argc, char **argv, int *length_ptr, int *link_length_ptr, int *step_ptr, char **outputfile)
{
  int wrong = 0;
  if (argc < 2)
  {
    printf("Password length must be specified\n");
    wrong = 1;
  }
  else
  {
    *length_ptr = atoi(argv[1]);
    if (*length_ptr == 0)
    {
      printf("Wrong value passed as password length\n");
      wrong = 1;
    }
  }
  if (argc > 2)
  {
    printf("2\n");
    *link_length_ptr = atoi(argv[2]);
    if (*link_length_ptr == 0)
    {
      printf("Wrong value passed as link length\n");
      wrong = 2;
    }
  }
  if (argc > 3)
  {
    printf("3\n");
    *step_ptr = atoi(argv[3]);
    if (*step_ptr == 0)
    {
      printf("Wrong value passed as step size\n");
      wrong = 3;
    }
  }
  if (argc > 4)
  {
    int size = strlen(argv[4] + 1);
    *outputfile = malloc((size + 1) * sizeof(**outputfile));
    memcpy(*outputfile, argv[4], size);
  }
  else
  {
    char d[] = "output.txt";
    int size = strlen(d);
    *outputfile = malloc((size + 1) * sizeof(**outputfile));
    (*outputfile)[size] = 0;
    memcpy(*outputfile, d, size);
  }
  if (wrong != 0)
  {
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    // MPI_Finalize();
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char **argv)
{
  // (Always push back length of passwords times)
  MPI_Init(NULL, NULL);
  int step = 6000;
  int min = 32;
  int max = 126;
  int range = 95;
  int remainder = 0;
  int max_step = range - 1;
  int num_passwords;
  int link_length = 20;
  int world_size, rank;
  int length = 5;
  char *file;
  char *start;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == MASTER_ID)
  {
    handle_input(argc, argv, &length, &link_length, &step, &file);
    printf("length = %d, link_length = %d, step = %d, outputfile = %s\n", length, link_length, step, file);
    start = malloc((length + 1) * sizeof(*start));
    memset(start, ' ', (length) * sizeof(*start));
    start[length] = 0;
  }
  cvector_vector_type(char) all_possibilities = NULL;
  cvector_vector_type(char) my_input = NULL;
  cvector_vector_type(char) my_output = NULL;
  cvector_vector_type(char) all_outputs = NULL;
  if (rank == MASTER_ID)
  {
    // +1 because adding null terminating string
    for (int i = 0; i < length + 1; i++)
    {
      cvector_push_back(all_possibilities, start[i]);
    }
    // loop until overflow happens;
    for (int j = 0;; j++)
    {
      int overflow = 0;
      // allocate memory for next part
      for (int k = 0; k < length + 1; k++)
      {
        cvector_push_back(all_possibilities, 0);
      }
      for (int k = 0; k < HASH_LENGTH; k++)
      {
        cvector_push_back(all_outputs, 0);
      }
      char *this = all_possibilities + j * (length + 1);
      char *n = this + length + 1;
      overflow = get_next(step, min, max, range, length, remainder, this, n);
      //adjust size because last element is not important if overflow happened
      if (overflow != 0)
      {
        for (int k = 0; k < length + 1; k++)
        {
          cvector_pop_back(all_possibilities);
        }
        for (int k = 0; k < HASH_LENGTH; k++)
        {
          cvector_pop_back(all_outputs);
        }
        break;
      }
    }
    num_passwords = cvector_size(all_possibilities) / (length + 1);
  }
  MPI_Bcast(&num_passwords, 1, MPI_INT, MASTER_ID, MPI_COMM_WORLD);
  MPI_Bcast(&link_length, 1, MPI_INT, MASTER_ID, MPI_COMM_WORLD);
  MPI_Bcast(&length, 1, MPI_INT, MASTER_ID, MPI_COMM_WORLD);
  MPI_Bcast(&step, 1, MPI_INT, MASTER_ID, MPI_COMM_WORLD);
  if (rank != MASTER_ID)
  {
    start = malloc((length + 1) * sizeof(*start));
    memset(start, ' ', (length) * sizeof(*start));
    start[length] = 0;
  }
  // Data type for passwords
  MPI_Datatype password_type;
  MPI_Type_contiguous(length + 1, MPI_CHAR, &password_type);
  MPI_Type_commit(&password_type);

  const int my_share = num_passwords / world_size;
  // Allocate vector to fit my share of passwords
  for (int i = 0; i < my_share; i++)
  {
    for (int j = 0; j < length + 1; j++)
    {
      cvector_push_back(my_input, 0);
    }
    for (int j = 0; j < HASH_LENGTH; j++)
    {
      cvector_push_back(my_output, 0);
    }
  }
  // send data to workers
  MPI_Scatter(all_possibilities, my_share, password_type, my_input, my_share, password_type, MASTER_ID, MPI_COMM_WORLD);

  for (int i = 0; i < my_share; i++)
  {
    chain(link_length, my_input + i * (length + 1), length, my_output + i * HASH_LENGTH, HASH_LENGTH);
  }

  MPI_Gather(my_output, HASH_LENGTH * my_share, MPI_CHAR, all_outputs, HASH_LENGTH * my_share, MPI_CHAR, MASTER_ID, MPI_COMM_WORLD);
  cvector_free(my_output);
  cvector_free(my_input);
  if (rank == MASTER_ID)
  {
    int last = my_share * world_size;

    for (int i = last; i < num_passwords; i++)
    {
      chain(link_length, all_possibilities + i * (length + 1), length, all_outputs + i * HASH_LENGTH, HASH_LENGTH);
    }

    FILE *w = fopen(file, "w");
    printf("Saving to file\n");
    for (int i = 0; i < num_passwords; i++)
    {
      fprintf(w, "%s;", all_possibilities + i * (length + 1));
      for (int j = 0; j < HASH_LENGTH; j++)
      {
        fprintf(w, "%02x", (uint8_t)all_outputs[i * (HASH_LENGTH) + j]);
      }
      fprintf(w, "\n");
    }
    fclose(w);
  }

  if (rank == MASTER_ID)
  {
    free(start);
    free(file);
    cvector_free(all_possibilities);
    cvector_free(all_outputs);
  }
  MPI_Finalize();
  return 0;
}