#define CAST(T, X) ((T)X)

#include <stddef.h>

void foo(void)
{
  int *i = NULL;
  void *a = CAST(void *, i);
}
