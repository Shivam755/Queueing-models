#include "Q.h"

// A function to input time
void time_in(Time *t1)
{
    while (true)
    {
        scanf("%d:%d", &t1->hour, &t1->minute);
        if (t1->hour < 24 && t1->minute <= 60 && t1->hour >= 0 && t1->minute >= 0)
        {
            break;
        }
        else
        {
            printf("The entered Time is invalid!!\nTry again: ");
        }
    }
}

// A function to find difference between time in minutes
int time_diff(Time s_time, Time e_time)
{
    int s_time_min, e_time_min, diff_m;
    s_time_min = s_time.minute + (s_time.hour * 60);
    e_time_min = e_time.minute + (e_time.hour * 60);
    if (s_time_min > e_time_min)
    {
        diff_m = 1440 + e_time_min - s_time_min;
    }
    else
    {
        diff_m = e_time_min - s_time_min;
    }
    return diff_m;
}

// A function to initialise a queue
void init_queue(queue *q)
{
    q->front = q->rear = NULL;
}

// A function to add an element to a queue
bool enqueue(queue *q, customer_type value)
{
    //if the queue is empty then add an element
    if (q->front == NULL)
    {
        node *n = (node *)malloc(sizeof(node));
        if (n == NULL)
        {
            return false;
        }
        else
        {
            n->value = value;
            n->next = NULL;
            q->front = q->rear = n;
        }
    }
    else
    {
        node *n = (node *)malloc(sizeof(node));
        if (n == NULL)
        {
            return false;
        }
        else
        {
            n->value = value;
            n->next = NULL;
            q->rear->next = n;
            q->rear = n;
        }
    }
    return true;
}

// A function to remove an element from a queue
bool dequeue(queue *q)
{
    node *temp = q->front;
    if (q->front == NULL)
    {
        return false;
    }
    // If only one element in the queue
    else if (q->front == q->rear)
    {
        q->front = q->rear = NULL;
    }
    else
    {
        q->front = temp->next;
    }
    free(temp);
    return true;
}

// A function to remove a customer from queue
bool leave_queue(queue *q, customer_type *c)
{
    node *temp = q->front;
    while (temp != NULL)
    {
        node *t1 = temp->next;
        if (c == &t1->value)
        {
            temp->next = t1->next;
            free(t1);
            return true;
        }
        temp = t1;
    }
    return false;
}