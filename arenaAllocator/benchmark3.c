/*
  Name: Viraj Sabhaya
  ID: 1001828871
  Name: Jose J Aguilar
  ID: 1001128942
*/
#include "mavalloc.h"
#include <stdlib.h>
#include <time.h>

clock_t start, end;
int main( int argc, char * argv[] )
{
  start = clock();
  unsigned char * array [10000];
  mavalloc_init( 500000, BEST_FIT );

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

  end = clock();
  printf("\n%f\n", (double)(end-start)/CLOCKS_PER_SEC);
  return 0;
}
