/*
  Name: Viraj Sabhaya
  ID: 1001828871
  Name: Jose J Aguilar
  ID: 1001128942
*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mavalloc.h"
#include <time.h>

#define MAX_LINKED_LIST_SIZE 10000

#define ROOTNODE 0

static int initialized = 0;

static int lastUsed = -1;

static enum ALGORITHM gAlgorithm;
static void *gArena;

enum TYPE
{
    P = 0,
    H
};

struct Node
{
    int size;
    enum TYPE type;
    void *arena;
    int in_use;
};

static struct Node LinkedList[MAX_LINKED_LIST_SIZE];

int findFreeNodeInternal()
{
    int i = 0;
    for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
    {
        if (LinkedList[i].in_use == 0)
        {
            return i;
        }
    }
    return -1;
}

int insertNodeInternal(int var, int value, unsigned char *new_arena)
{
    int i = 0;

    if (initialized && var < 0 || var >= MAX_LINKED_LIST_SIZE)
    {
        printf("ERROR: Tried to insert a node beyond our bounds %d\n", var);
        return -1;
    }

    for (i = 10000; i >= var; i--)
    {
        LinkedList[i + 1].size = LinkedList[i].size;
        LinkedList[i + 1].in_use = LinkedList[i].in_use;
    }

    LinkedList[var].in_use = 1;
    LinkedList[var].size = value;
    LinkedList[var].arena = new_arena;

    return 0;
}

int removeNodeInternal(int node)
{
    int i;

    if (node < 0 || node >= MAX_LINKED_LIST_SIZE)
    {
        printf("ERROR: Can not remove node %d because it is out of our array bounds"
               " of 0 ... %d\n",
               node, MAX_LINKED_LIST_SIZE);
        return -1;
    }

    if (LinkedList[node].in_use == 0)
    {
        printf("ERROR: Can not remove node %d.  It is not in use\n", node);
        return -1;
    }

    LinkedList[node].in_use = 0;

    for (i = node; i < lastUsed; i++)
    {
        LinkedList[i].size = LinkedList[i + 1].size;

        LinkedList[i].in_use = LinkedList[i + 1].in_use;
    }

    LinkedList[lastUsed].size = -1;
    LinkedList[lastUsed].in_use = 0;

    lastUsed = lastUsed - 1;

    return 0;
}

int removeNode(int index) //1 ~ for ptr1
{
    int i = 0;

    for (i = index; i < MAX_LINKED_LIST_SIZE - 1; i++)
    {
        //ptr1.values = ptr2.values
        LinkedList[i].in_use = LinkedList[i + 1].in_use;
        LinkedList[i].size = LinkedList[i + 1].size;
        LinkedList[i].arena = LinkedList[i + 1].arena;
        LinkedList[i].type = LinkedList[i + 1].type;
    }

    LinkedList[MAX_LINKED_LIST_SIZE - 1].in_use = 0;
}

// int insertNode(int value, unsigned char *new_arena)
int insertNode(int var, int value, unsigned char *new_arena)
{
    int previous = -1;
    int ret = -1;
    // int index = ROOTNODE;

    // for (i = ROOTNODE; i < MAX_LINKED_LIST_SIZE; i++)
    // {
    //   if ((LinkedList[i].in_use && LinkedList[i].size > value) ||
    //       (LinkedList[i].in_use == 0))
    //   {
    // previous = i;
    //     break;
    //   }
    // }
    
    if (previous >= -1)
    {
        ret = insertNodeInternal(var, value, new_arena); // 0,leftover_size, new_arena;
    }
    else if (previous >= MAX_LINKED_LIST_SIZE || previous < 0)
    {
        printf("Error: Tried to insert beyond the bounds of the allocated list.\n");
    }
    return ret;
}

void printList()
{

    int i = 0;

    for (i = ROOTNODE; i < MAX_LINKED_LIST_SIZE; i++)
    {
        if (LinkedList[i].in_use == 0)
        {
            break;
        }
        printf("LinkedList[%d]: %d\n", i, LinkedList[i].size);
    }
}

int mavalloc_init(size_t size, enum ALGORITHM algorithm)
{
    // Initialize our link List
    int i = 0;
    for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
    {
        LinkedList[i].size = 0;
        LinkedList[i].in_use = 0;
        LinkedList[i].arena = 0;
        LinkedList[i].type = H;
    }
    // Allocate the pool
    gArena = malloc(ALIGN4(size));
    // Save the algorithm type
    gAlgorithm = algorithm;

    // set the first entry to point to the arena
    LinkedList[0].in_use = 1;
    LinkedList[0].size = ALIGN4(size);
    LinkedList[0].type = H;
    LinkedList[0].arena = gArena;

    return 0;
}

void mavalloc_destroy()
{
    int i = 0;
    for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
    {
        LinkedList[i].size = 0;
        LinkedList[i].in_use = 0;
        LinkedList[i].arena = 0;
        LinkedList[i].type = H;
    }

    free(gArena);
    return;
}

void *mavalloc_alloc(size_t size)
{
    void *ptr = NULL;
    int ret = 0;

    size = ALIGN4(size);

    if (gAlgorithm == FIRST_FIT)
    {
        // start at the beginning of the List
        int i = 0;
        // if the node type is hole and in_use equals 1 and size <node size

        for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
        {
            if (LinkedList[i].type == H && LinkedList[i].in_use && size <= LinkedList[i].size)
            {
                int leftover_size = LinkedList[i].size - size;
                // calculate the arena
                unsigned char *arena = (unsigned char *)LinkedList[i].arena + size;
                int var = i + 1;
                // unsigned char * new_ptr = (unsigned char *)LinkedList[i].arena + (unsigned char *)size;
                ret = insertNode(var, leftover_size, arena);
                if (ret == -1)
                {
                    printf("There was an error with insert node");
                }
                LinkedList[i].type = P;
                return LinkedList[i].arena;
            }
        }
        // If there is leftover size then insert a new node as a hole
        // that holds that leftover space
        //
        // then return that node arena pointer
    }
    else if (gAlgorithm == BEST_FIT)
    {
        int i = 0;
        int winner = -1;
        int previous_leftover_size = INT_MAX;
        for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
        {
            if (LinkedList[i].type == H && LinkedList[i].in_use && size <= LinkedList[i].size)
            {
                if (previous_leftover_size > LinkedList[i].size - (int)size)
                {
                    previous_leftover_size = LinkedList[i].size - (int)size;
                    winner = i;
                }
            }
        }

        if (winner != -1)
        {
            void *arena = (void *)((long int)LinkedList[winner].arena + (long int)size);
            int var = winner + 1;
            if (previous_leftover_size > 0)
            {
                ret = insertNode(var, previous_leftover_size, arena);
                if (ret == -1)
                {
                    printf("There was an error with insert node");
                }
            }
            LinkedList[winner].type = P;
            LinkedList[winner].size = ALIGN4(size);
            ptr = LinkedList[winner].arena;
        }
        return ptr;
    }
    else if (gAlgorithm == WORST_FIT)
    {
        int i = 0;
        int winner = -1;
        int previous_leftover_size = INT_MIN;
        for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
        {
            if (LinkedList[i].type == H && LinkedList[i].in_use && size <= LinkedList[i].size)
            {
                if (previous_leftover_size < LinkedList[i].size - (int)size)
                {
                    previous_leftover_size = LinkedList[i].size - (int)size;
                    winner = i;
                }
            }
        }

        if (winner != -1)
        {
            void *arena = (void *)((long int)LinkedList[winner].arena + (long int)size);
            int var = winner + 1;
            if (previous_leftover_size > 0)
            {
                ret = insertNode(var, previous_leftover_size, arena);
                if (ret == -1)
                {
                    printf("There was an error with insert node");
                }
            }
            LinkedList[winner].type = P;
            LinkedList[winner].size = ALIGN4(size);
            ptr = LinkedList[winner].arena;
        }
        return ptr;
    }
    else if (gAlgorithm == NEXT_FIT)
    {
        // start at the last node used.
        int i, previous = -1;
        for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
        {
            if (LinkedList[i].type == H && LinkedList[i].in_use && size <= LinkedList[i].size)
            {
                previous = i;
                int leftover_size = LinkedList[previous].size - size;
                // int leftover_size = LinkedList[i].size - size;
                unsigned char *arena = (unsigned char *)LinkedList[i].arena + size;
                int var = i + 1;
                ret = insertNode(var, leftover_size, arena);
                if (ret == -1)
                {
                    printf("There was an error with insert node");
                }
            }
            else if (LinkedList[i].type == H && LinkedList[i].in_use && size > LinkedList[i].size)
            {
                return NULL;
            }
            LinkedList[i].type = P;
            return LinkedList[i].arena;
        }
    }
    return ptr;
}

void mavalloc_free(void *ptr)
{
    // search for the node containing the value given by ptr
    int i = 0;
    //only matchds ptr 1 if ptr1 is passed in
    for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
    {
        if (ptr == LinkedList[i].arena)
        {
            LinkedList[i].type = H;
        }
    }
    // set that node to be a type H
    // check if adjacent nodes are free if they are combine them
    for (i = 0; i < MAX_LINKED_LIST_SIZE - 1; i++)
    {
        if (LinkedList[i].type == H && LinkedList[i + 1].type == H)
        {
            // combine the sizes into LinkedList[i]
            LinkedList[i].size = LinkedList[i].size + LinkedList[i + 1].size;
            // remove the next node at LinkedList[i+1]
            removeNode(i + 1); //i=0
        }
    }
    return;
}

int mavalloc_size()
{
    int number_of_nodes = 0;
    int i = 0;
    for (i = 0; i < MAX_LINKED_LIST_SIZE; i++)
    {
        if (LinkedList[i].in_use)
        {
            number_of_nodes++;
        }
    }

    return number_of_nodes;
}