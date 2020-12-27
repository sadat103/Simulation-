#include "simlib.c"
#define Arrival         1

#define Departure       2

#define End_Simulation  3
#define STREAM_INTERARRIVAL   1
#define STREAM_JOB_TYPE       2
#define STREAM_SERVICE        3
#define total_stations      5
#define total_jobs     3

/* Declare non-simlib global variables. */

int   stations, jobss, i, j;
int  machines[total_stations + 1];
int Tasks[total_jobs +1];
int  route[total_jobs +1][total_stations + 1];
int  busy_machn[total_stations + 1], job_type, task;
float mean_interarrival;
float length_simulation, job_probab[26] ;
float mean_service[total_jobs +1][ total_stations + 1];
FILE  *infile, *outfile;

void arrive(int new_job)
{
    int station;
    if (new_job == 1)
    {
        float b = expon(mean_interarrival, STREAM_INTERARRIVAL) ;
        float a = sim_time +  b ;
        event_schedule(a, Arrival);
        job_type = random_integer(job_probab, STREAM_JOB_TYPE);
        task    = 1;
    }

    station = route[job_type][task];

    if (busy_machn[station] == machines[station]) {

        transfer[1] = sim_time;
        transfer[2] = job_type;
        transfer[3] = task;
        list_file(LAST, station);
    }

    else {

        sampst(0.0, station);                              /* For station. */
        sampst(0.0, stations + job_type);              /* For job type. */
        ++busy_machn[station];
        timest((float) busy_machn[station], station);



        transfer[3] = job_type;
        transfer[4] = task;
        float c = erlang(2, mean_service[job_type][task], STREAM_SERVICE);
        float d  = c + sim_time ;
        event_schedule(d, Departure);
    }
}


void depart(void)
{
    int station, job_type_queue, task_queue;
    job_type = transfer[3];
    task     = transfer[4];
    station  = route[job_type][task];

    if (list_size[station] == 0) {

        --busy_machn[station];
        timest((float) busy_machn[station], station);
    }

    else {

        list_remove(FIRST, station);

        /* Tally this delay for this station. */
        float e = sim_time - transfer[1] ;
        float d = stations + job_type_queue ;
        sampst(e, station);

        job_type_queue = transfer[2];
        task_queue     = transfer[3];
        sampst(e, d);


        transfer[3] = job_type_queue;
        transfer[4] = task_queue;
        float f  = erlang(2, mean_service[job_type_queue][task_queue], STREAM_SERVICE);
        float g = f +sim_time ;
        event_schedule(g , Departure);
}



    if (task < Tasks[job_type])
    {
        ++task;
        arrive(2);
    }
}


void report(void)
{
    int   i;
    float overall_avg_job_tot_delay, avg_job_tot_delay, sum_probs;



    fprintf(outfile, "\n\n\n\nJob type     Average total delay in queue");
    overall_avg_job_tot_delay = 0.0;
    sum_probs                 = 0.0;
    for (i = 1; i <= jobss; ++i)
    {
        avg_job_tot_delay = sampst(0.0, -(stations + i)) * Tasks[i];
        fprintf(outfile, "\n\n%4d%27.3f", i, avg_job_tot_delay);
        overall_avg_job_tot_delay += (job_probab[i] - sum_probs)
                                     * avg_job_tot_delay;
        sum_probs = job_probab[i];
    }
    float y = (-1) *overall_avg_job_tot_delay;
    fprintf(outfile, "\n\nOverall average job total delay =%10.3f\n",y);


    fprintf(outfile,
           "\n\n\n Work      Average number      Average       Average delay");
    fprintf(outfile,
             "\nstation       in queue       job        in queue");
    for (j = 1; j <= stations; ++j)
        fprintf(outfile, "\n\n%4d%17.3f%17.3f%17.3f", j, filest(j),
                timest(0.0, -j) / machines[j], sampst(0.0, -j));
}


main()
{


    infile  = fopen("jobshop.in",  "r");
    outfile = fopen("jobshop.out", "w");


    fscanf(infile, "%d %d %f %f", &stations, &jobss,
           &mean_interarrival, &length_simulation);
    for (j = 1; j <= stations; ++j)
        fscanf(infile, "%d", &machines[j]);
    for (i = 1; i <= jobss; ++i)
        fscanf(infile, "%d", &Tasks[i]);
    for (i = 1; i <= jobss; ++i) {
        for (j = 1; j <= Tasks[i]; ++j)
            fscanf(infile, "%d", &route[i][j]);
        for (j = 1; j <= Tasks[i]; ++j)
            fscanf(infile, "%f", &mean_service[i][j]);
    }
    for (i = 1; i <= jobss; ++i)
        fscanf(infile, "%f", &job_probab[i]);



    fprintf(outfile, "Job-shop model\n\n");
    fprintf(outfile, "Number of work stations%21d\n\n", stations);
    fprintf(outfile, "Number of machines in each station");
    for (j = 1; j <= stations; ++j)
        fprintf(outfile, "%5d", machines[j]);
    fprintf(outfile, "\n\nNumber of job types%25d\n\n", jobss);
    fprintf(outfile, "Number of tasks for each job type      ");
    for (i = 1; i <= jobss; ++i)
        fprintf(outfile, "%5d", Tasks[i]);
    fprintf(outfile, "\n\nDistribution function of job types  ");
    for (i = 1; i <= jobss; ++i)
        fprintf(outfile, "%8.3f", job_probab[i]);
    fprintf(outfile, "\n\nMean interarrival time of jobs%14.2f hours\n\n",
            mean_interarrival);
    fprintf(outfile, "Length of the simulation%20.1f eight-hour days\n\n\n",
            length_simulation);
    fprintf(outfile, "Job type     Work stations on route");
    for (i = 1; i <= jobss; ++i) {
        fprintf(outfile, "\n\n%4d        ", i);
        for (j = 1; j <= Tasks[i]; ++j)
            fprintf(outfile, "%5d", route[i][j]);
    }
    fprintf(outfile, "\n\n\nJob type     ");
    fprintf(outfile, "Mean service time (in hours) for successive tasks");
    for (i = 1; i <= jobss; ++i) {
        fprintf(outfile, "\n\n%4d    ", i);
        for (j = 1; j <= Tasks[i]; ++j)
            fprintf(outfile, "%9.2f", mean_service[i][j]);
    }


    for (j = 1; j <= stations; ++j)
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


