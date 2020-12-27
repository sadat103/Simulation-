#include "simlib.c"

#define Arrival         1
#define Departure       2
#define End_Simulation  3
#define STREAM_INTERARRIVAL   1
#define STREAM_food_circle       2
#define STREAM_SERVICE        3
#define total_food_court      4
#define total_food_circle     4

int   court, food, i, j;
int group ;
int  machines[total_food_court + 1];
int Tasks[total_food_circle +1];
int  route[total_food_circle +1][total_food_court + 1];
int  busy_machn[total_food_court + 1],food_circle, task;
float mean_interarrival;
float length_simulation, food_probab[26] ;
float group_probab[26] ;
float mean_service[total_food_circle +1][ total_food_court + 1];
FILE  *infile, *outfile;

/* Declare non-simlib functions. */

int printRandoms(int lower, int upper,  int count)
{
    int i;
    for (i = 0; i < count; i++) {
        int num = (rand() %  (upper - lower + 1)) + lower;
        return num;
    }
}
void arrive(int new_food)
{
    int station;


    if (new_food == 1)
    {
        float b = expon(mean_interarrival, STREAM_INTERARRIVAL) ;
        float a = sim_time +  b ;
        event_schedule(a, Arrival);
       food_circle = random_integer(food_probab, STREAM_food_circle);
        task    = 1;
    }


    station = route[food_circle][task];



    if (busy_machn[station] == machines[station]) {


        transfer[1] = sim_time;
        transfer[2] =food_circle;
        transfer[3] = task;
        list_file(LAST, station);
    }

    else {



        sampst(0.0, station);
        sampst(0.0, court +food_circle);
        ++busy_machn[station];
        timest((float) busy_machn[station], station);



        transfer[3] =food_circle;
        transfer[4] = task;
        int p = printRandoms(0,4,1);
        if(p==1)
        {
        float c = erlang(2, mean_service[food_circle][task] + group_probab[p], STREAM_SERVICE);
        float d  = c + sim_time ;
        event_schedule(d, Departure);
        }
         if(p==2)
        {
        float c = erlang(2, mean_service[food_circle][task] + group_probab[p], STREAM_SERVICE);
        float d  = c + sim_time ;
        event_schedule(d, Departure);
        }
         if(p==3)
        {
        float c = erlang(2, mean_service[food_circle][task] + group_probab[p], STREAM_SERVICE);
        float d  = c + sim_time ;
        event_schedule(d, Departure);
        }
         if(p==4)
        {
        float c = erlang(2, mean_service[food_circle][task] + group_probab[p], STREAM_SERVICE);
        float d  = c + sim_time ;
        event_schedule(d, Departure);
        }


    }
}


void depart(void)
{

    int station,food_circle_queue, task_queue;


   food_circle = transfer[3];
    task     = transfer[4];
    station  = route[food_circle][task];



    if (list_size[station] == 0) {


        --busy_machn[station];
        timest((float) busy_machn[station], station);
    }

    else {

        list_remove(FIRST, station);
        float e = sim_time - transfer[1] ;
        float d = court +food_circle_queue ;
        sampst(e, station);

       food_circle_queue = transfer[2];
        task_queue     = transfer[3];
        sampst(e, d);

        transfer[3] =food_circle_queue;
        transfer[4] = task_queue;
        float f  = erlang(2, mean_service[food_circle_queue][task_queue], STREAM_SERVICE);
        float g = f +sim_time ;
        event_schedule(g , Departure);
}

    if (task < Tasks[food_circle])
    {
        ++task;
        arrive(2);
    }
}


void report(void)
{
    int   i;
    float overall_avg_food_tot_delay, avg_food_tot_delay, sum_probs;
    fprintf(outfile, "\n\n\n\nfood type     Average total delay in queue");
    overall_avg_food_tot_delay = 0.0;
    sum_probs                 = 0.0;
    for (i = 1; i <= food; ++i)
    {
        avg_food_tot_delay = sampst(0.0, -(court + i)) * Tasks[i];
        fprintf(outfile, "\n\n%4d%27.3f", i, avg_food_tot_delay);
        overall_avg_food_tot_delay += (food_probab[i] - sum_probs)
                                     * avg_food_tot_delay;
        sum_probs = food_probab[i];
    }
    fprintf(outfile, "\n\nOverall average food total delay =%10.3f\n",
            overall_avg_food_tot_delay);

    fprintf(outfile,
           "\n\n\n Work      Average number      Average       Average delay");
    fprintf(outfile,
             "\nstation       in queue       food        in queue");
    for (j = 1; j <= court; ++j)
        fprintf(outfile, "\n\n%4d%17.3f%17.3f%17.3f", j, filest(j),
                timest(0.0, -j) / machines[j], sampst(0.0, -j));
}


main()
{


    infile  = fopen("cafe.in",  "r");
    outfile = fopen("cafe.out", "w");



    fscanf(infile, "%d %d %d %f %f", &court, &food,&group,
           &mean_interarrival, &length_simulation);
    for (j = 1; j <= court; ++j)
        fscanf(infile, "%d", &machines[j]);
    for (i = 1; i <= food; ++i)
        fscanf(infile, "%d", &Tasks[i]);
    for (i = 1; i <= food; ++i) {
        for (j = 1; j <= Tasks[i]; ++j)
            fscanf(infile, "%d", &route[i][j]);
        for (j = 1; j <= Tasks[i]; ++j)
            fscanf(infile, "%f", &mean_service[i][j]);
    }
    for (i = 1; i <= food; ++i)
        fscanf(infile, "%f", &food_probab[i]);
    for (i = 1; i <= group; ++i)
        fscanf(infile, "%f", &group_probab[i]);

    /* Write report heading and input parameters. */

    fprintf(outfile, "food-shop model\n\n");
    fprintf(outfile, "Number of work court%21d\n\n", court);
    fprintf(outfile, "Number of machines in each station");
    for (j = 1; j <= court; ++j)
        fprintf(outfile, "%5d", machines[j]);
    fprintf(outfile, "\n\nNumber of food types%25d\n\n", food);
    fprintf(outfile, "Number of rotation each food type      ");
    for (i = 1; i <= food; ++i)
        fprintf(outfile, "%5d", Tasks[i]);
    fprintf(outfile, "\n\nDistribution function of food types  ");
    for (i = 1; i <= food; ++i)
        fprintf(outfile, "%8.3f", food_probab[i]);
    fprintf(outfile, "\n\nMean interarrival time of foods%14.2f hours\n\n",
            mean_interarrival);
    fprintf(outfile, "Length of the simulation%20.1f 1.5hr days\n\n\n",
            length_simulation);
    fprintf(outfile, "food type     Work court on route");
    for (i = 1; i <= food; ++i) {
        fprintf(outfile, "\n\n%4d        ", i);
        for (j = 1; j <= Tasks[i]; ++j)
            fprintf(outfile, "%5d", route[i][j]);
    }
    fprintf(outfile, "\n\n\nfood type     ");
    fprintf(outfile, "Mean service time (in hours) for every food circle");
    for (i = 1; i <= food; ++i) {
        fprintf(outfile, "\n\n%4d    ", i);
        for (j = 1; j <= Tasks[i]; ++j)
            fprintf(outfile, "%9.2f", mean_service[i][j]);
    }

    for (j = 1; j <= court; ++j)
        busy_machn[j] = 0;

    init_simlib();
    maxatr = 4;
    event_schedule(expon(mean_interarrival, STREAM_INTERARRIVAL), Arrival);

    event_schedule(8 * length_simulation, End_Simulation);

    do {


        timing();
        switch (next_event_type) {
            case Arrival:
                arrive(1);
                break;
            case Departure:
                depart();
                break;
            case End_Simulation:
                report();
                break;
        }

    } while (next_event_type != End_Simulation);

    fclose(infile);
    fclose(outfile);

    return 0;
}


