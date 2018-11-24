#include "types.h"
#include "user.h"

void randomtest(void)
{
  char *random_str = 0;

  printf(1, "TESTING RANDOM NUMBER GENERATOR\n");

  random_str = malloc(10);

  random(random_str, 10);

  for (int i = 0; i < 10; i++) {
    printf(1, "BYTE DATA: %d\n", random_str[i]);
  }
}

int main(void)
{
  randomtest();
  exit();
}
