#include "mavalloc.h"

int main( int argc, char * argv[] )
{
  unsigned char * array [10000];
  mavalloc_init( 1000000, WORST_FIT );

  int i = 0;
  for(i=0;i<1000;i++)
  {
    mavalloc_alloc(100);
  }

  for(i=0;i<1000;i++)
  {
    mavalloc_free(array[i]);
  }

  char * ptr = malloc(10000);
  char * ptr1 = malloc(10000);
  char * ptr2= malloc(10000);
  char * ptr3 = malloc(10000);

  free (ptr1);
  free (ptr2);

  char * ptr4 = malloc(500);
  char * ptr5 = malloc(500);

  return 0;
}
