#include "types.h"
#include "user.h"

void randomtest(void)
{
  char *random_str = 0;

  printf(1, "TESTING RANDOM NUMBER GENERATOR\n");
  random(random_str, 10);
}

int main(void)
{
  randomtest();
  exit();
}
