#include "types.h"
#include "user.h"

void randomtest(void)
{
  char *random_str = 0;

  printf(1, "TESTING RANDOM NUMBER GENERATOR\n");

  random_str = malloc(11);

  random(random_str, 10);

  random_str[10] = 0;
  printf(1, "BYTE TEST:");
  for (int i = 0; i < 10; i++) {
    printf(1, "%x ", random_str[i]);
  }
}

int main(void)
{
  randomtest();
  exit();
}
