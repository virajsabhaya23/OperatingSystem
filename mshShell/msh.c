/*
  NAME : VIRAJ SABHAYA
  ID : 1001828871
*/

// The MIT License (MIT)
//
// Copyright (c) 2016 Trevor Bakker
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
#include <string.h>

#define WHITESPACE " \t\n" // We want to split our command line up into tokens
                           // so we need to define what delimits our tokens.
                           // In this case  white space
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11 // Mav shell only supports ten arguments
#define MAX_NUM_HISTORY_ARGS 15 // Mav shell only supports 15 history arguments

int main()
{

  char *command_string = (char *)malloc(MAX_COMMAND_SIZE);
  
  char *history[MAX_NUM_HISTORY_ARGS];
  int hist_counter = 0;
  for(int j = 0 ; j< MAX_NUM_HISTORY_ARGS; j++)
  {
    history[j] = (char *)malloc(MAX_COMMAND_SIZE);
  }

  while (1)
  {
    // Print out the msh prompt
    printf("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(command_string, MAX_COMMAND_SIZE, stdin));

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];
    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;

    char *working_string = strdup(command_string);

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while (((argument_ptr = strsep(&working_string, WHITESPACE)) != NULL) && (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }

    // Creating a process ID variable to hold the processes
    pid_t pid;
    // This variable keep tracks of the child processes and the parent processes.
    int pid_status;

    if (token[0] != NULL)
    {

      char *usr_input_for_hist = strdup(command_string);
      strcpy(history[hist_counter], usr_input_for_hist);

      if (strcmp(token[0], "exit") == 0 || strcmp(token[0], "quit") == 0)
      {
        exit(0);
      }
      else if (strcmp(token[0], "cd") == 0)
      {
        chdir(token[1]);
      }
      else if (strcmp(token[0], "history") == 0)
      {
        if(hist_counter > 0)
        {
          for(int i = 0; i < hist_counter; i++)
          {
            printf("[%d]: %s", i+1, history[i]);
          }
        }
        else
        {
          printf("No commands in history.\n");
        }
      }
      else if (strcmp(token[0],"!") == 0)
      {
        char *input = strtok(command_string, "!");
        int input_num = atoi(input);

        // if(history != NULL)
        // {

        // }
        // else
        // {
        //   printf("No commands in history.");
        // }
      }
      else
      {
        pid_t pid = fork();
        if (pid == 0)
        {
          if (execvp(token[0], token) == -1)
          {
            printf("%s: Command not found.\n", token[0]);
          }
        }
        else
        {
          int status;
          wait(&status);
        }
      }
    }
    hist_counter++;
    free(head_ptr);
  }
  return 0;
  // e2520ca2-76f3-90d6-0242ac120003
}
