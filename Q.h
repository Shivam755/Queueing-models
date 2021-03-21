#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// Defining a struct for time
typedef struct time
{
    int hour;
    int minute;
} Time;

// Customer type
typedef struct customer_type
{
    int type;
    char symbol;
    int waiting_time;
    int being_served;
    int max_waiting_time;
} customer_type;

// Creating a linked_list struct
typedef struct node
{
    customer_type value;
    struct node *next;
} node;

//Creating a queue
typedef struct queue
{
    node *front;
    node *rear;
} queue;

// A function to input time
void time_in(Time *);

// A functon to find difference of time in minutes
int time_diff(Time, Time);

// A function to initialise a queue
void init_queue(queue *);

// A function to add an element to a queue
bool enqueue(queue *, customer_type);

// A function to remove an element from a queue
bool dequeue(queue *);

// A function to remove a customer from queue
bool leave_queue(queue *, customer_type *);
