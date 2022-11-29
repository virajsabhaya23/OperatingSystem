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
#include <sys/stat.h>
#include <time.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 3     // Mav shell only supports three arguments


#define NUMBER_OF_BLOCKS 4226 //number of blocks in file system
#define BLOCK_SIZE 8192 //file system block size
#define MAX_NUM_OF_FILES 125 // max number of files in file system
#define MAX_FILE_SIZE 10240000 // max file size per file in file system
#define MAX_FILE_NAME_SIZE 32 //define file name size
#define MAX_BLOCKS_PER_FILE 1250 // MAX_FILE_SIZE divided by BLOCK_SIZE gives us the MAX_BLOCKS_PER_FILE


//inodes maintain a list of which blocks make up a file
//one inode per file
//we have 125 files in our image so we need 125 inodes
///blocks make up a file

//Directory maps from the filename to an inode number that holds file info


unsigned char data_blocks[NUMBER_OF_BLOCKS][BLOCK_SIZE]; //34,619,392
int used_blocks[NUMBER_OF_BLOCKS]; //4226


struct directory_entry
{
  char * name;
  int in_use;
  int inodeIndex;
  int valid;
};

struct directory_entry *directory_ptr;


struct inode
{
  time_t date;
  int in_use;
  int size;
  int blocks [MAX_BLOCKS_PER_FILE];
};

struct inode *inode_array_ptr[MAX_NUM_OF_FILES];
struct stat buf;


void init()
{
  int i;
  directory_ptr = (struct directory_entry*) &data_blocks[0];

  for(i=0; i<MAX_NUM_OF_FILES;i++)
  {
    directory_ptr[i].in_use =0;
  }

  int inode_idx=0;
  for(i=1; i<130;i++)
  {
    inode_array_ptr[inode_idx++]=(struct inode*) &data_blocks[i];
  }
}
int findFreeBlock()
{

  int retval = -1;
  int i=0;

  for(i=130; i<4226;i++)
  {
    if(used_blocks[i]==0)
    {
      retval = i;
      break;
    }
    return retval;
  }
}

//QUESTION:
int findFreeInodeBlockEntry(int inode_index)
{
  int i;
  int retval =-1;

  for(i=0; i<MAX_BLOCKS_PER_FILE; i++)
  {
    if(inode_array_ptr[inode_index]->blocks[i]==-1)
    {
      retval=i;
      break;
    }
  }
  return retval;
}

// 0 entry in the block is a directory
// 1-129 are inodes
// 130 to 4226
int df()
{
  int count=0;
  int i=0;
  for(i=130; i<4226;i++)
  {
    if(used_blocks[i]==0)
    {
       count++;
    }
  }
  return count * BLOCK_SIZE;
}

// QUESTION
// directory is taking 0 and 1 blocks 
//  what do we set and end i to to find a free directory block
int findFreeDirectoryEntry()
{
  int i;
  int retval=-1;

  for(i=0;MAX_NUM_OF_FILES;i++)
  {
    if(directory_ptr[i].in_use == 0) //block that is not being used
    {
      retval=i;
      break;
    }
  }
  return retval;
}

//Same question as above 
int findFreeInode()
{
  int i;
  int retval=-1;
  //initialized an inode array of size 125
  //search the array for a free inode
  for(i=0;i<MAX_NUM_OF_FILES;i++)
  {
    if(inode_array_ptr[i]->in_use==0)
    {
      retval=i;
      break;
    }
  }
  return retval;
}

void put(char * token [])
{
  int status= stat(token[1],&buf);

  if(status == -1)
  {
    printf("Error: File not found\n");
    return;
  }

  if(buf.st_size > df())
  {
    printf("Put error: Not enough disk space.\n");
    return;
  }

  int dir_index=findFreeDirectoryEntry();
  if(dir_index == -1)
  {
    printf("Put Error: Not enough room in the filesystem directory\n");
    return;
  }

  if(strlen(token[1]) > MAX_FILE_NAME_SIZE)
  {
    printf("Put Error: File name too long.\n");
  }

  directory_ptr[dir_index].valid = 1;
  directory_ptr[dir_index].name=(char*)malloc(strlen(token[1]));
  strncpy(directory_ptr[dir_index].name, token[1], strlen(token[1]));

  int inode_idx=findFreeInode();
  if(inode_idx == -1)
  {
    printf("Error: No free inode\n");
    return;
  }
  directory_ptr[dir_index].inodeIndex=inode_idx;

  inode_array_ptr[inode_idx]->size=buf.st_size;
  inode_array_ptr[inode_idx]->in_use=1;
  inode_array_ptr[inode_idx]->date=time(NULL);



  FILE *ifp =fopen(token[1],"r");
  printf("Reading %d bytes from %s\n", (int)buf.st_size, token[1]);

  int copy_size = buf.st_size;
  int offset=0;




  while(copy_size >= BLOCK_SIZE)
  {
    int block_index=findFreeBlock();
    if(block_index == -1)
    {
      printf("Error: No free block found\n");
      //cleanup a bunch of directory and inode stuff
      return;
    }
    used_blocks[block_index]=1;

    int inode_block_entry = findFreeInodeBlockEntry(inode_idx);
    if(inode_block_entry == -1)
    {
      printf("Error: No free node block found\n");
      //cleanup a bunch of directory and inode stuff
      return;
    }
    inode_array_ptr[inode_idx]->blocks[inode_block_entry] = block_index;
    //fseek is a system call that moves a file pointer a certain number of bytes
    //First parameter is the file Pointer
    //Second parameter is the number of bytes to move
    //Third parameter is how you want to move it
    fseek(ifp, offset, SEEK_SET);
    //fread
    //first parameter where youre reading to
    //second parameter how many things youre reading
    //third parameter how big are the things youre reading
    //fourth parameter where we are reading from
    //fread is going to return the number of bytes that are read
    int bytes  = fread( data_blocks[block_index], BLOCK_SIZE, 1, ifp );

    if( bytes == 0 && !feof( ifp ) )
    {
      printf("An error occured reading from the input file.\n");
      return;
    }

    clearerr( ifp );

    copy_size -= BLOCK_SIZE;

    offset += BLOCK_SIZE;

  }
  if(copy_size>0)
  {
    int block_index=findFreeBlock();

    if(block_index == -1)
    {
      printf("Error: No free block found\n");
      //cleanup a bunch of directory and inode stuff
      return;
    }
    int inode_block_entry = findFreeInodeBlockEntry(inode_idx);
    if(inode_block_entry == -1)
    {
      printf("Error: No free node block found\n");
      //cleanup a bunch of directory and inode stuff
      return;
    }
    inode_array_ptr[inode_idx]->blocks[inode_block_entry] = block_index;

    used_blocks[block_index]=1;
    fseek(ifp, offset, SEEK_SET);
    int bytes  = fread( data_blocks[block_index], copy_size, 1, ifp );
  }

  fclose( ifp );
  return;
}


void get(char * token [])
{
    FILE *ofp;
    ofp = fopen(token[2], "w");

    if(ofp == NULL)
    {
      printf("Could not open output file: %s\n", token[2] );
      perror("Opening output file returned");
      return -1;
    }

    int block_index = 0;
    int copy_size = buf.st_size;
    int offset= 0;

    printf("Writing %d bytes to %s\n", (int)buf.st_size, token[2]);


    while( copy_size > 0 )
    {

      int num_bytes;

      if( copy_size < BLOCK_SIZE )
      {
        num_bytes = copy_size;
      }
      else
      {
        num_bytes = BLOCK_SIZE;
      }

      fwrite( data_blocks[block_index], num_bytes, 1, ofp );


      copy_size -= BLOCK_SIZE;
      offset += BLOCK_SIZE;
      block_index ++;

      fseek( ofp, offset, SEEK_SET );
    }

    fclose( ofp );
}


void filesystemCommands(char * token [])
{
  if(token[0]=="put")
  {
    put(token);
  }
  else if(token[0]=="get")
  {
    get(token);
  }
  else if(token[0]=="quit")
  {
    exit(0);
  }
  else if(token[0]=="df")
  {
    int leftover_disk_space=df();
    printf("Disk Space left in the filesystem : %d\n", leftover_disk_space);
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




int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

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
    /*
    if( token_count != 3 )
    {
      printf("Incorrect number of parameters. \nUse: \n	./a.out filein fileout\n");
      return -1;
    }
    else
    {
      fileCommands(token);
    }

    */
    fileCommands(token);
    free( working_root );

  }
  return 0;
}
