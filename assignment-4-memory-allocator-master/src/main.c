#define _DEFAULT_SOURCE

#include <stdio.h>

#include "test.h"

int main()
{
  if (run_all_tests(stdout))
    printf("All tests passed!");
  return 0;
}