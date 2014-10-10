#include "util.h"

int main(void)
{
  int * a = alloc(20,10);
  init_test(3, 3, a, 20);
  affiche(20,10,a, 20, stdout);
  return 0;
}
