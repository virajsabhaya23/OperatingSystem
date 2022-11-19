/*
  Name: Viraj Sabhaya
  ID: 1001828871
  Name: Jose J Aguilar
  ID: 1001128942
*/
// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

//define file system size
#define FILE_NAME_SIZE 32

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  FILE *fileNameOne, *fileNameTwo;
  char tmep;

  while( 1 )
  {
    // Print out the mfs prompt
    printf ("mfs> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality

    int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ )
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );
    }

    if (token[0] != NULL)
    {
      //quits the file system application if the user inputs quit
      if (strcmp(token[0], "quit") == 0)
      {
        exit(0);
      }
      // compares if the user inputs to close the file system
      if (strcmp(token[0], "open") == 0)
      {
        if(token[1] != NULL)
        {
          //opens the file system
          fileNameOne = fopen(token[1], "r+");
          // fileNameTwo = fopen(token[2], "w+");
          {
            printf("Opening file system : %s\n", token[1]);
          }

          do
          {
              /* Read single character from file */
              tmep = fgetc(fileNameOne);

              /* Print character read on console */
              putchar(tmep);

          } while(tmep != EOF);
          // tmep = fgetc(fileNameOne);
          // while(tmep != EOF)
          // {
          //   fputs(tmep, fileNameTwo);
          //   tmep = fgetc(fileNameOne);
          // }
        }
        else if(token[2] != NULL)
        {
          printf("too many arguments/ more than 1 filename provided.\n");
        }
    else if(token[0]=="del")
    {
      int var=remove(token[1]);
      if(var==0)
      {
        printf("File deleted\n");
      }
      else
      {
        printf("error: File not found\n");
      }
    }
        else
        {
          printf("No file name provided.\n");
        }
      }
      else
      {
        pid_t pid = fork();

        if (pid == 0)
        {
          if (execvp(token[0], token) == -1)
          {
            printf("%s: Command not found.\n", token[0]);
            exit(0);
          }
        }
        else
        {
          int status;
          waitpid(pid, &status, 0);
        }
      }
    }

    free( working_root );

  }
  return 0;
}
