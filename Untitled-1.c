int test_case_21()
{
  //init the arena
  mavalloc_init(65535,FIRST_FIT);

  //check if size returns 1
  int size =mavalloc_size();
  TINYTEST_EQUAL(1,size);

  //destroy the arena
  mavalloc_destroy();

  //check if size returns 0
  size=mavalloc_size();
  TINYTEST_EQUAL(0,size);

  return 1;
}


int test_case_22()
{
  //init the arena
  mavalloc_init(65535,FIRST_FIT);
  char * ptr=(char *)mavalloc_alloc(100);
  printf("This is ptr \n");
  printf("%p\n",ptr);
  // printf("It works \n");
  return 1;
}

int test_case_23()
{
  mavalloc_init(65535,FIRST_FIT);
  char * ptr=(char *)mavalloc_alloc(100);
  printf("%p\n",ptr);
  mavalloc_free(ptr);
  ptr=(char *)mavalloc_alloc(100);
  printf("This is the second ptr \n");
  printf("%p\n",ptr);

  return 1;
}

TINYTEST_ADD_TEST(test_case_21, tinytest_setup, tinytest_teardown);
TINYTEST_ADD_TEST(test_case_22, tinytest_setup, tinytest_teardown);
TINYTEST_ADD_TEST(test_case_23, tinytest_setup, tinytest_teardown);