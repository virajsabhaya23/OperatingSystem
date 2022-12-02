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

#define WHITESPACE " \t\n" // We want to split our command line up into tokens
                           // so we need to define what delimits our tokens.
                           // In this case  white space
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 3 // Mav shell only supports three arguments

#define NUMBER_OF_BLOCKS 4226    // number of blocks in file system
#define BLOCK_SIZE 8192          // file system block size
#define MAX_NUM_OF_FILES 125     // max number of files in file system
#define MAX_FILE_SIZE 10240000   // max file size per file in file system
#define MAX_FILE_NAME_SIZE 200   // define file name size
#define MAX_BLOCKS_PER_FILE 1250 // MAX_FILE_SIZE divided by BLOCK_SIZE gives us the MAX_BLOCKS_PER_FILE

// inodes maintain a list of which blocks make up a file
// one inode per file
// we have 125 files in our image so we need 125 inodes
/// blocks make up a file

// Directory maps from the filename to an inode number that holds file info

unsigned char data_blocks[NUMBER_OF_BLOCKS][BLOCK_SIZE]; // 34,619,392
int used_blocks[NUMBER_OF_BLOCKS];                       // 4226
FILE *fp = NULL;

/*
  This directory structure will contain values that specify 
  the name of a file, if that directory index is in use (0 or 1) 
  and the inode index which maps in to an inode or the inode structure
  */
struct directory_entry
{
  char *name;
  int in_use;
  int inodeIndex;
};
/*
  Initializing a pointer to a struct called directory_ptr. 
  This element's indexes will map to other structures
  that will hold our data. 
*/
struct directory_entry *directory_ptr;

/*
  The inode structure contains values such as if its in_use. We can 
  skip the inode if its in use. The size of the file. The date the 
  file was created. Lastly, a blocks array that will map to our
  data_blocks array. This is the way were going to map our data.
*/
struct inode
{
  time_t date;
  int in_use;
  int size;
  int blocks[32];
};
/*
  Initializing a array of pointers to a struct with a size of 125.
  This element will hold our inode indexes. Each file will belong to
  one file.
*/
struct inode *inode_array_ptr[MAX_NUM_OF_FILES];

/* Init function
  This function initializes and assigns a value to each index of that
certain structure/element. The directory_ptr is pointing to the first
block in data_blocks. This way we now that our directory exists in the 
first block of data_blocks. We initially set our directory indexes to not
in use or 0. This is used because once we use that directory index we'll 
set it to 1 or in use. For the inode pointer we do the same thing 
We just run a second for loop and access the blocks array and set it to -1. 
This will notify us that the block is not in use.

*/
void init()
{
  int i;
  int j;
  directory_ptr = (struct directory_entry *)&data_blocks[0];

  for (i = 0; i < MAX_NUM_OF_FILES; i++)
  {
    directory_ptr[i].in_use = 0;
  }

  int inode_idx = 0;
  for (i = 0; i < 125; i++)
  {
    inode_array_ptr[i] = (struct inode *)&data_blocks[i + 5];
    inode_array_ptr[i]->in_use = 0;
    for (j = 0; j < 32; j++)
    {
      inode_array_ptr[i]->blocks[j] = -1;
    }
    inode_idx++;
  }
}

/*
  This function is findFreeBlock(), this function will
  iterate throught the used_blocks array from 130-4226 
  and attempt to find a free space. If it does it returns 
  the index of the free block.
*/
int findFreeBlock()
{

  int retval = -1;
  int i = 0;

  for (i = 130; i < 4226; i++)
  {
    if (used_blocks[i] == 0)
    {
      retval = i;
      break;
    }
  }
  return retval;
}

/*
  This function findFreeDirectoryEntry() will find the index
  of a free directory by interating thru directory_ptr. If its
  not in use it will equal 0 so we can return that index and say
  we found a free directory to use. If not it returns a -1.
*/
int findFreeDirectoryEntry()
{
  int i;
  int retval = -1;

  for (i = 0; i < MAX_NUM_OF_FILES; i++)
  {
    if (directory_ptr[i].in_use == 0) // can be directory_ptr[i].in_use
    {
      retval = i;
      break;
    }
  }
  return retval;
}

/*
  This function findFreeInode() searches inode_array_ptr
  for an index not in use. Since its one inode per file
  we need to search for an inode thats not in use or equal 0.
  If we do we can return the index if not we return -1 or we 
  didnt find an inode available.
*/
int findFreeInode()
{
  int i;
  int retval = -1;
  // initialized an inode array of size 125
  // search the inode_array_ptr for a free inode
  // return the index ov the free inode
  for (i = 0; i < MAX_NUM_OF_FILES; i++)
  {
    if (inode_array_ptr[i]->in_use == 0)
    {
      retval = i;
      break;
    }
  }
  return retval;
}

/*
  This function findFreeInodeBlockEntry() searches the block element
  in inode_array_ptr that has a certian inode index and looks for a free block 
  to index to a used_block index. Each block has a size 8192 so if its in use 
  that means that 8192 bytes have been used. The function the returns that index of 
  blocks. 
*/
int findFreeInodeBlockEntry(int inode_index)
{
  int i;
  int retval = -1;

  for (i = 0; i < 32; i++)
  {
    if (inode_array_ptr[inode_index]->blocks[i] == -1)
    {
      retval = i;
      break;
    }
  }
  return retval;
}

/*
  The df functionreturns how much space we have left in the filesystem Image,
  uses the count variable to keep track of how many blocks are in use
*/
int df()
{
  int count = 0;
  int i = 0;
  for (i = 130; i < 4226; i++)
  {
    if (used_blocks[i] == 0)
    {
      count++;
    }
  }
  return count * BLOCK_SIZE;
}

/*
  put function will take a file name and will write the file to the
  filesystem image.
*/
void put(char *token[])
{
  printf("%s\n", token[1]);
  struct stat buf;
  int status = stat(token[1], &buf);
  // if the file does not exist stat returns a -1
  if (status == -1)
  {
    printf("Error: File not found\n");
    return;
  }

  if (buf.st_size > df())
  {
    printf("Put error: Not enough disk space.\n");
    return;
  }

  // directory index
  int dir_index = findFreeDirectoryEntry();
  if (dir_index == -1)
  {
    printf("Put Error: Not enough room in the filesystem directory\n");
    return;
  }

  if (strlen(token[1]) > MAX_FILE_NAME_SIZE)
  {
    printf("Put Error: File name too long.\n");
  }
  // this is where we will store our data
  // this means its being used now
  directory_ptr[dir_index].in_use = 1;
  // Now we have to copy the filename into the directory entry
  // We allocate memory for the name
  directory_ptr[dir_index].name = (char *)malloc(32);
  memset(directory_ptr[dir_index].name, 0, 32);
  // We string copy into directory_ptr[dir_index].name
  strncpy(directory_ptr[dir_index].name, token[1], strlen(token[1]));

  // Now we have to find a free inode
  int inode_idx = findFreeInode();
  // if inode_idx equals -1 unable to find free inode
  if (inode_idx == -1)
  {
    printf("Error: No free inode\n");
    return;
  }
  // store inode_idx into inodeIndex of the directory entry were currently using
  directory_ptr[dir_index].inodeIndex = inode_idx;

  inode_array_ptr[inode_idx]->size = buf.st_size;
  inode_array_ptr[inode_idx]->in_use = 1;
  inode_array_ptr[inode_idx]->date = time(NULL);

  // we now have captured all the metadata for the file
  // All of our metadata is currently populated
  // Now we need to start copying our data from the file into our data blocks

  // What were going to do is loop over the file in block size chunks
  // All operations will be in units of blocks size

  // Take the size of the file divided by the BLOCK_SIZE
  // and thats the amount of times we will go thru the loop to copy the file

  FILE *ifp = fopen(token[1], "r");
  printf("Reading %d bytes from %s\n", (int)buf.st_size, token[1]);

  int copy_size = buf.st_size;
  int offset = 0;

  while (copy_size >= BLOCK_SIZE)
  {
    // we use this to find out the value of block_index everytime the loop
    // runs instead of using block_index++
    int block_index = findFreeBlock();

    if (block_index == -1)
    {
      // if this print statement prints theres a bug in the code
      // user should not see this or be in this situation
      printf("Error: No free block found\n");
      // cleanup a bunch of directory and inode stuff
      return;
    }

    used_blocks[block_index] = 1;

    int inode_block_entry = findFreeInodeBlockEntry(inode_idx);
    if (inode_block_entry == -1)
    {
      printf("Error: No free node block found\n");
      // cleanup a bunch of directory and inode stuff
      return;
    }
    inode_array_ptr[inode_idx]->blocks[inode_block_entry] = block_index;
    // fseek is a system call that moves a file pointer a certain number of bytes
    // First parameter is the file Pointer
    // Second parameter is the number of bytes to move
    // Third parameter is how you want to move it
    fseek(ifp, offset, SEEK_SET);
    // fread
    // first parameter where youre reading to
    // second parameter how many things youre reading
    // third parameter how big are the things youre reading
    // fourth parameter where we are reading from
    // fread is going to return the number of bytes that are read
    int bytes = fread(data_blocks[block_index], BLOCK_SIZE, 1, ifp);

    if (bytes == 0 && !feof(ifp))
    {
      printf("An error occured reading from the input file.\n");
      return;
    }

    clearerr(ifp);
    copy_size -= BLOCK_SIZE;
    offset += BLOCK_SIZE;
  }

  // to handle remainder
  if (copy_size > 0)
  {

    int block_index = findFreeBlock();

    if (block_index == -1)
    {
      printf("Error: No free block found\n");
      // cleanup a bunch of directory and inode stuff
      return;
    }

    int inode_block_entry = findFreeInodeBlockEntry(inode_idx);
    if (inode_block_entry == -1)
    {
      printf("Error: No free node block found\n");
      // cleanup a bunch of directory and inode stuff
      return;
    }
    inode_array_ptr[inode_idx]->blocks[inode_block_entry] = block_index;

    used_blocks[block_index] = 1;
    fseek(ifp, offset, SEEK_SET);

    // we use copy_size bc copy_size holds the remainder
    int bytes = fread(data_blocks[block_index], copy_size, 1, ifp);
  }

  fclose(ifp);
  return;
}

/*
  get <filename> - copies the file to the local directory.
  get <filename1> <filename2> - copies file1 to file2.
  it opens the file and writes to it and then closes it.
*/
void get(char *token[])
{
  int i;
  int block_index;
  int copy_size;
  int offset;

  FILE *ofp;
  ofp = fopen(token[2], "w");

  if (ofp == NULL)
  {
    printf("Could not open output file: %s\n", token[2]);
    perror("Opening output file returned");
    return;
  }

  for (i = 0; i < MAX_NUM_OF_FILES; i++)
  {
    if (strcmp(directory_ptr[i].name, token[1]) == 0)
    {
      int in_idx = directory_ptr[i].inodeIndex;
      // printf("in_idx: %d\n", in_idx);
      copy_size = inode_array_ptr[in_idx]->size;
      // printf("copy_size: %d\n", copy_size);
      block_index = inode_array_ptr[in_idx]->blocks[0];
      // printf("block_index: %d\n", block_index);
      offset = 0;
      // printf("Writing %d bytes to %s\n", copy_size, token[2]);
      while (copy_size > 0)
      {
        int num_bytes;
        if (copy_size < BLOCK_SIZE)
        {
          num_bytes = copy_size;
        }
        else
        {
          num_bytes = BLOCK_SIZE;
        }
        fwrite(data_blocks[block_index], num_bytes, 1, ofp);
        copy_size -= BLOCK_SIZE;
        offset += BLOCK_SIZE;
        block_index++;
        fseek(ofp, offset, SEEK_SET);
      }
      break;
    }
  }
  fclose(ofp);
}

/*
  listFiles function will list all the files in the files system images.
  It will print out the name of the file and the size of the file,
  along with the date and time.
*/
void listFiles()
{
  int i;
  char *date_string;
  int lenght;

  for (i = 0; i < MAX_NUM_OF_FILES; i++)
  {
    if (directory_ptr[i].in_use == 1 && inode_array_ptr[i]->in_use == 1)
    {
      lenght = strlen(ctime(&inode_array_ptr[i]->date)) - 1;
      strncpy(date_string, ctime(&inode_array_ptr[i]->date), lenght);
      printf("%d %s %s\n", inode_array_ptr[i]->size, date_string, directory_ptr[i].name);
    }
  }
}

/*
  deleteFile function deletes a file from the file system,
  if the file exsists in the list, by using the inuse variable form the
  directory ptr Struct or else prompts an error if file ont found.
*/
void deleteFile(char *token[])
{
  int i;
  int in_idx;
  int bl_index;
  int j;
  for (i = 0; i < MAX_NUM_OF_FILES; i++)
  {
    if (strcmp(directory_ptr[i].name, token[1]) == 0)
    {
      in_idx = directory_ptr[i].inodeIndex;
      for (j = 0; j < 32; j++)
      {
        bl_index = inode_array_ptr[in_idx]->blocks[j];
        used_blocks[bl_index] = 0;
      }
      inode_array_ptr[in_idx]->in_use = 0;
      directory_ptr[i].in_use = 0;
      return;
    }
  }
  printf("Error: File not found\n");
}

/*
  createfs fucntion creates a file system with the given size
  and the given name in local direcotry and stores it in the data blocks
  upto 34 M.
*/
void createfs(char *token[])
{
  fp = fopen(token[1], "w");
  
}

/*
  fileSystemCommands function takes the user input in form of
  of tokens, and calls the relevant function that matches the
  if conidtion and further perform action for that token.
*/
void filesystemCommands(char *token[])
{
  FILE *fileImg;

  if (strcmp(token[0], "put") == 0)
  {
    if (token[1] == NULL)
    {
      printf("Error: Filename required\n");
    }
    else
    {
      put(token);
    }
  }
  else if (strcmp(token[0], "get") == 0)
  {
    get(token);
    return;
  }
  else if (strcmp(token[0], "quit") == 0)
  {
    exit(0);
  }
  else if (strcmp(token[0], "df") == 0)
  {
    int leftover_disk_space = df();
    printf(" %d bytes free \n", leftover_disk_space);
  }
  else if (strcmp(token[0], "open") == 0)
  {
    if (token[1] == NULL)
    {
      printf("Error: No such file exists. \n");
      return;
    }
    else if (token[1] != NULL)
    {
      //write the open file name to the variable fileImg
      fileImg = fopen(token[1], "r");
    }
  }
  else if (strcmp(token[0], "close") == 0)
  {
    if (fileImg == NULL)
    {
      printf("Error: No file is opened. \n");
      return;
    }
  }
  else if (strcmp(token[0], "createfs") == 0)
  {
    // create a 34M file in the local directory
    // write data_blocks to that file
    // close the file
    if (token[1] == NULL)
    {
      printf("Error: No such file exists. \n");
      return;
    }
    else if (token[1] != NULL)
    {
      createfs(token);
    }
  }
  else if (strcmp(token[0], "list") == 0)
  {
    listFiles();
  }
  else if (strcmp(token[0], "del") == 0)
  {
    deleteFile(token);
  }
  else if (strcmp(token[0], "cd") == 0)
  {
    chdir(token[1]);
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
  init();
  char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);

  while (1)
  {
    // Print out the mfs prompt
    printf("mfs> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
      ;

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    char *working_str = strdup(cmd_str);

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while (((arg_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }

    // it will only look for the commands if the arg[0] is not null
    if (token[0] != NULL)
    {
      filesystemCommands(token);
    }

    free(working_root);
  }
  return 0;
}
