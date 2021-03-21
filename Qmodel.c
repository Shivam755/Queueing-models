#include "Q.c"
#include <time.h>
#include <math.h>

void add_server(int n, int h, int w, queue[n], char[h][w]);
void calc_custom(int t, customer_type[t], int, int);
void map_screen(int h, int w, char[h][w]);
int next_loc(int h, int w, char[h][w], int n, int[n]);
void sync(int h, int w, char[h][w], int n, queue[n], int[n]);
void display_screen(int h, int w, char[h][w]);
void simulate(float, float, int, Time, Time, int);

int main()
{
    //Mandatory inputs
    float arrival_rate, service_rate;
    int num_of_server, sel_cust;
    char choice;
    Time start_time, end_time;

    printf("Average arrival-rate[customer/hr]: ");
    scanf("%f", &arrival_rate);

    printf("\nAverage service-rate per server[customer/hr]: ");
    scanf("%f", &service_rate);

    printf("\nNumber of server(s): ");
    scanf("%d", &num_of_server);
    // Taking input a valid Time
    printf("\nOpening Time in 24hr format[HH:MM]:\n");
    time_in(&start_time);

    printf("\nClosing Time in 24hr format[HH:MM]:\n");
    time_in(&end_time);
    fflush(stdin);
    printf("\nAre there impatient customers for this queue model[y/n]??\n");
    choice = getchar();
    sel_cust = (choice == 'y') ? 2 : 1;

    // Running simulation
    simulate(arrival_rate, service_rate, num_of_server, start_time, end_time, sel_cust);
    if (arrival_rate > service_rate * num_of_server)
    {
        int n = ceil(num_of_server + floor(arrival_rate - service_rate * num_of_server) / service_rate);
        printf("\n\nThe specified queueing model is inefficient and will fail gradually!!\n");
        printf("Increase the average service rate to greater than or equal to %f\n", arrival_rate / num_of_server);
        printf("Or Increase the number of servers to %d\n", n);
    }
    else if (arrival_rate < service_rate * num_of_server)
    {
        int n = floor(floor(service_rate * num_of_server - arrival_rate) / service_rate);
        printf("\n\nThe specified queueing model is very efficient.\n");
        printf("The model will hold even if the number of servers is reduced by %d\n", n);
        printf("It can handle arrival rate upto: %d\n", (int)floor(service_rate * num_of_server));
    }
    else
    {
        printf("The specified queueing model is perfect for the specified condition.\n");
    }
    return 0;
}

void simulate(float arrival_rate, float service_rate, int num_of_server, Time start_time, Time end_time, int sel_cust)
{
    // Creating a seed for random function
    srand(time(0));

    // varibles for loop
    int i, j, k, n;

    // supporting values
    int inter_arriv_time, inter_serve_time;

    // Variables to monitor simultaion
    int operate_time, custom_sys = 0, custom_entered = 0, custom_served = 0, custom_left = 0, custom_wait = 0, total_custom, mins = 0, s_mins = -1, first_entry = 0, ctm_at_once, height, width, max_ctm_entry, queue_num = 0;
    bool arrival_start = false;

    // an array to store corresponding index of queue
    int queue_index[num_of_server];
    for (i = 0, j = 3; i < num_of_server; i++, j += 5)
    {
        queue_index[i] = j;
    }

    // calculating support variables
    inter_arriv_time = floor(60 / arrival_rate);
    inter_serve_time = ceil(60 / service_rate);
    first_entry = rand() % 5;
    max_ctm_entry = round(arrival_rate / 10);

    // Calculating boundary variables
    operate_time = time_diff(start_time, end_time);
    total_custom = round(operate_time * arrival_rate / 60);

    // storing all the expected customers
    customer_type expected[total_custom];
    calc_custom(total_custom, expected, inter_serve_time, sel_cust);

    // Declaring the screen
    height = ceil(total_custom / num_of_server) + 2 /*for borders*/ + 2 /*for server*/ + 1 /*Just for saftey*/;
    width = 5 /*width of one server*/ * num_of_server + 2 /*for borders*/;
    char screen[height][width];
    map_screen(height, width, screen);

    // an array to store customers in queue
    queue servers[num_of_server];
    add_server(num_of_server, height, width, servers, screen);

    while (true)
    {
        //printf("\n\n");
        if (mins == first_entry)
        {
            arrival_start = true;
        }
        if (arrival_start)
        {
            // Adding customers to queue
            if (custom_entered < total_custom)
            {
                do
                {
                    ctm_at_once = 1 + rand() % max_ctm_entry;
                } while (custom_entered + ctm_at_once > total_custom);
                for (i = 0; i < ctm_at_once; i++)
                {
                    queue_num = next_loc(height, width, screen, num_of_server, queue_index);
                    if (!enqueue(&servers[queue_num], expected[custom_entered]))
                    {
                        printf("Not enough memory!!\n");
                        exit(1);
                    }
                    sync(height, width, screen, num_of_server, servers, queue_index);
                    custom_entered++;
                    custom_sys++;
                }
            }
            // Removing customers from queue
            if (custom_sys > 0)
            {
                s_mins++;
                for (i = 0; i < num_of_server; i++)
                {
                    node *temp = servers[i].front;
                    if (temp != NULL)
                    {
                        temp->value.being_served++;
                        if (temp->value.being_served % inter_serve_time == 0)
                        {
                            custom_served++;
                            dequeue(&servers[i]);
                            if (servers[i].front != NULL)
                            {
                                temp = servers[i].front;
                            }
                            custom_sys--;
                        }
                        temp = temp->next;
                        while (temp != NULL)
                        {
                            temp->value.waiting_time++;
                            if (temp->value.waiting_time > temp->value.max_waiting_time)
                            {
                                leave_queue(&servers[i], &temp->value);
                                custom_left++;
                                custom_sys--;
                            }
                            temp = temp->next;
                        }
                    }
                }
            }

            if (custom_sys > num_of_server)
            {
                custom_wait = custom_sys - num_of_server;
            }
            else
            {
                custom_wait = 0;
            }
        }
        sync(height, width, screen, num_of_server, servers, queue_index);

// Checking OS and applying command to clear screen
#ifdef _WIN32
        {
            system("cls");
        }
#elif __APPLE__
        {
            system("clear");
        }
#elif __LINUX__
        {
            system("clear");
        }
#else
        {
            printf("\n\n");
        }
#endif

        // showing simulation
        printf("Customers entered : %d/%d", custom_entered, total_custom);
        printf("\t\t\tSimulation time: %d\n", mins);
        printf("Total customers in system: %d", custom_sys);
        if (s_mins < 0)
        {
            printf("\t\t\tServing time: %d\n", 0);
        }
        else
        {
            printf("\t\t\tServing time: %d\n", s_mins);
        }
        printf("Customers served: %d", custom_served);
        printf("\t\t\t\tS = Server\n");
        printf("Customers in queue waiting to be served: %d", custom_wait);
        printf("\t& = Patient customer\n");
        printf("Customers left without service: %d", custom_left);
        printf("\t\t@ = Impatient customer\n");
        display_screen(height, width, screen);

        // loop termination statement
        if (custom_served + custom_left == total_custom)
        {
            break;
        }

        // Modifying environment variables
        sleep(1);
        mins++;
    }
    if (operate_time < inter_serve_time)
    {
        for (i = 0; i < num_of_server; i++)
        {
            node *temp = servers[i].front;
            while (temp != NULL)
            {
                node *t1 = temp;
                temp = t1->next;
                free(t1);
            }
        }
    }
}

void add_server(int n, int h, int w, queue server[n], char s[h][w])
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        init_queue(&server[i]);
    }
    for (i = 0, j = 1; i < n; i++, j += 5)
    {
        s[1][j] = '|';
        s[1][j + 2] = 'S';
        s[1][j + 4] = '|';
        int k = j + 1;
        while (k < j + 4)
        {
            s[2][k] = '-';
            k++;
        }
    }
}

void calc_custom(int total_custom, customer_type expected[total_custom], int wait_time, int sel_cust)
{
    int i, j = 0, imp_rat = round(total_custom * 0.2);
    for (i = 0; i < total_custom; i++)
    {
        expected[i].type = rand() % sel_cust;
        expected[i].waiting_time = 0;
        expected[i].being_served = 0;
        if (expected[i].type && j <= imp_rat)
        {
            expected[i].symbol = '@';
            if (wait_time < 10)
            {
                expected[i].max_waiting_time = 10;
            }
            else
            {
                expected[i].max_waiting_time = wait_time;
            }
            j++;
        }
        else
        {
            expected[i].symbol = '&';
            expected[i].max_waiting_time = __INT32_MAX__;
        }
    }
}

void map_screen(int h, int w, char s[h][w])
{
    int i, j;
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            if (i == 0 || i == h - 1)
            {
                if (j == 0 || j == w - 1)
                {
                    s[i][j] = '+';
                }
                else
                {
                    s[i][j] = '-';
                }
            }
            else if (j == 0 || j == w - 1)
            {
                s[i][j] = '|';
            }
            else
            {
                s[i][j] = ' ';
            }
        }
    }
}

int next_loc(int height, int width, char screen[height][width], int num_of_server, int q_index[num_of_server])
{
    int j, k, n;
    for (j = 3; j < height; j++)
    {
        for (n = 0; n < num_of_server; n++)
        {
            k = q_index[n];
            if (screen[j][k] == ' ')
            {
                return n;
            }
        }
    }
    return -1;
}

void sync(int h, int w, char screen[h][w], int n, queue servers[n], int q_index[n])
{
    int i, j, k;
    for (i = 3; i < h - 1; i++)
    {
        for (j = 3; j < w - 1; j += 5)
        {
            screen[i][j] = ' ';
        }
    }
    for (i = 0; i < n; i++)
    {
        node *temp = servers[i].front;
        k = q_index[i];
        j = 3;
        while (temp != NULL)
        {
            node *t1 = temp;
            screen[j][k] = t1->value.symbol;
            temp = t1->next;
            j++;
        }
    }
}

void display_screen(int h, int w, char s[h][w])
{
    int i, j;
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            printf("%c", s[i][j]);
        }
        printf("\n");
    }
}
