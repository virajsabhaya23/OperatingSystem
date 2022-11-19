#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define array_size 1000

clock_t start, end;

int main( int argc, char * argv[] )
{
  start = clock();
  unsigned char * array [array_size];
  mavalloc_init( 500000, BEST_FIT );

  int i = 0;
  for(i=0;i<1000;i++)
  {
    array[i]=mavalloc_alloc(400);
  }

  for(i=0;i<1000;i++)
  {
    mavalloc_free(array[i]);
  }

  char * ptr  = malloc(100);
  char * ptr1 = malloc(1000);
  char * ptr2 = malloc(3000000);
  char * ptr3 = malloc(200);
  char * ptr4 = malloc(2220000);
  char * ptr5 = malloc(4111111);

  free (ptr4);
  free (ptr5);

  ptr5 = malloc(11111);
  char * ptr6 = malloc(110000);
  char * ptr7 = malloc(5000);
  char * ptr8 = malloc(1000000);
  char * ptr9 = malloc(200000);

  end = clock();

  printf("\n%f\n", (double)(end-start)/CLOCKS_PER_SEC);

  return 0;
}
