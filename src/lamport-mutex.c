/* lamport-mutex.c */
/* Date  : 04 July 2017
 * Author: Ankit Pati
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t pid, timestamp;
} queue;

typedef struct {
    size_t *events, num_events, num_q;
    queue *q;
    /* events       timestamps for each event
     * num_events   number of events
     * q            maintains per-process queues
     * num_q        number of processes in queue
     */
} process;

int compar_q(const queue *a, const queue *b) /* for qsort(3) */
{
    /* ascending sort by timestamp, then descending sort by PID */
    return a->timestamp > b->timestamp ? 1 :
                    a->timestamp < b->timestamp ? -1 : a->pid > b->pid ? -1 : 1;
}

int main()
{
    size_t p, i, j, p1, p2, e1, e2, nq;
    /* p            is number of processes
     * i, j         are counters
     * p1, p2       are current processes under consideration
     * e1, e2       are current events under consideration
     * nq           is current location in q
     */

    int option;
    process *processes;

    puts("Number of Processes?");
    scanf(" %zu%*c", &p);

    processes = malloc(sizeof(*processes) * p);

    for (i = 0; i < p; ++i) {
        processes[i].events     = NULL;
        processes[i].q          = NULL;
        processes[i].num_events = 0;
        processes[i].num_q      = 0;
    }

    puts(
        "\n"
        "Add Events (Choose an option)\n"
        "1. Isolated Event\n"
        "2. Communicated Event\n"
        "3. Critical Section Request\n"
        "4. Critical Section Release\n"
        "0. Done\n"
    );

    do {
        printf("?> ");
        fflush(stdout);
        scanf(" %d%*c", &option);

        switch (option) {
        case 1:
            puts("Process?");
            scanf(" %zu%*c", &p1);
            --p1;

            if (p1 >= p) {
                fprintf(stderr, "Invalid Process Number!\n");
                break;
            }

            e1 = processes[p1].num_events;
            ++processes[p1].num_events;
            processes[p1].events = realloc(processes[p1].events,
                                           sizeof(*processes[p1].events)
                                            * processes[p1].num_events);
            processes[p1].events[e1] = processes[p1].events[e1 - !!e1] + 1;

            break;

        case 2:
            puts("From?");
            scanf(" %zu%*c", &p1);
            --p1;

            puts("To?");
            scanf(" %zu%*c", &p2);
            --p2;

            if (p1 >= p || p2 >= p) {
                fprintf(stderr, "Invalid Process Number!\n");
                break;
            }

            /*
            if (p1 == p2) {
                fprintf(stderr, "From and To processes may not be same!\n");
                break;
            }

            // Actually, they may be same. This code is commented out by design.
            */

            e1 = processes[p1].num_events;
            ++processes[p1].num_events;
            processes[p1].events = realloc(processes[p1].events,
                                           sizeof(*processes[p1].events)
                                            * processes[p1].num_events);
            processes[p1].events[e1] = processes[p1].events[e1 - !!e1] + 1;

            e2 = processes[p2].num_events;
            ++processes[p2].num_events;
            processes[p2].events = realloc(processes[p2].events,
                                           sizeof(*processes[p2].events)
                                            * processes[p2].num_events);
            processes[p2].events[e2] = 1 + (
                processes[p2].events[e2 - !!e2] > processes[p1].events[e1] ?
                processes[p2].events[e2 - !!e2] : processes[p1].events[e1]
            );

            break;

        case 3:
            puts("Process?");
            scanf(" %zu%*c", &p1);
            --p1;

            if (p1 >= p) {
                fprintf(stderr, "Invalid Process Number!\n");
                break;
            }

            if (processes[0].num_q) {
                for (i = 0; i < p && processes[i].q[0].pid == p1; ++i);
                if (i == p) {
                    printf("Process %zu already in Critical Section.\n", p1+1);
                    break;
                }
            }

            e1 = processes[p1].num_events;
            ++processes[p1].num_events;
            processes[p1].events = realloc(processes[p1].events,
                                           sizeof(*processes[p1].events)
                                            * processes[p1].num_events);
            processes[p1].events[e1] = processes[p1].events[e1 - !!e1] + 1;

            nq = processes[p1].num_q;
            ++processes[p1].num_q;
            processes[p1].q = realloc(processes[p1].q,
                                          sizeof(*processes[p1].q)
                                           * processes[p1].num_q);
            processes[p1].q[nq].pid = p1;
            processes[p1].q[nq].timestamp = processes[p1].events[e1];

            qsort(processes[p1].q, processes[p1].num_q,
                  sizeof(*processes[p1].q),
                  (int (*)(const void *, const void *)) compar_q);

            /* update all clocks and queues for REQUEST broadcast */
            for (p2 = 0; p2 < p; ++p2) {
                if (p2 == p1) continue; /* my clock has already been updated */

                e2 = processes[p2].num_events;
                ++processes[p2].num_events;
                processes[p2].events = realloc(processes[p2].events,
                                               sizeof(*processes[p2].events)
                                                * processes[p2].num_events);
                processes[p2].events[e2] = 1 + (
                    processes[p2].events[e2 - !!e2] > processes[p1].events[e1] ?
                    processes[p2].events[e2 - !!e2] : processes[p1].events[e1]
                );

                nq = processes[p2].num_q;
                ++processes[p2].num_q;
                processes[p2].q = realloc(processes[p2].q,
                                              sizeof(*processes[p2].q)
                                               * processes[p2].num_q);
                processes[p2].q[nq].pid = p1;
                processes[p2].q[nq].timestamp = processes[p1].events[e1];

                qsort(processes[p2].q, processes[p2].num_q,
                      sizeof(*processes[p2].q),
                      (int (*)(const void *, const void *)) compar_q);
            }

            for (i = 0; i < p && processes[i].q[0].pid == p1; ++i);
            if (i == p) {
                /* update own clock for GRANTED responses */
                for (i = 0; i < p - 1; ++i) {
                    e1 = processes[p1].num_events;
                    ++processes[p1].num_events;
                    processes[p1].events = realloc(processes[p1].events,
                                                   sizeof(*processes[p1].events)
                                                    * processes[p1].num_events);
                    processes[p1].events[e1] = processes[p1].events[e1 - !!e1]
                                                                            + 1;
                }

                printf("Process %zu enters Critical Section.\n", p1 + 1);
            }

            break;

        case 4:
            if (!processes[0].num_q) {
                fprintf(stderr, "No Process in Critical Section!\n");
                break;
            }

            p1 = processes[0].q[0].pid;

            for (i = 0; i < p && processes[i].q[0].pid == p1; ++i);
            if (i != p) {
                fprintf(stderr, "Invalid State Detected! Terminating now...\n");
                exit(1);
            }

            e1 = processes[p1].num_events;
            ++processes[p1].num_events;
            processes[p1].events = realloc(processes[p1].events,
                                           sizeof(*processes[p1].events)
                                            * processes[p1].num_events);
            processes[p1].events[e1] = processes[p1].events[e1 - !!e1] + 1;

            for (i = 0; i < p; ++i) {
            }

            /* update all clocks and queues for RELEASE broadcast */
            for (p2 = 0; p2 < p; ++p2) {
                /* No memory reallocation here.
                 * It happens only while requesting Critical Section.
                 * This increases performance, because many reallocs are no-ops.
                 */

                --processes[p2].num_q;

                for (j = 0; j < processes[p2].num_q; ++j)
                    processes[p2].q[j] = processes[p2].q[j + 1];

                if (p2 == p1) continue; /* my clock has already been updated */

                e2 = processes[p2].num_events;
                ++processes[p2].num_events;
                processes[p2].events = realloc(processes[p2].events,
                                               sizeof(*processes[p2].events)
                                                * processes[p2].num_events);
                processes[p2].events[e2] = 1 + (
                    processes[p2].events[e2 - !!e2] > processes[p1].events[e1] ?
                    processes[p2].events[e2 - !!e2] : processes[p1].events[e1]
                );
            }

            printf("Process %zu leaves Critical Section.\n", p1 + 1);

            if (!processes[0].num_q) {
                printf("No Process waiting for Critical Section.\n");
                break;
            }

            p2 = processes[0].q[0].pid;

            for (i = 0; i < p && processes[i].q[0].pid == p2; ++i);
            if (i != p) {
                fprintf(stderr, "Invalid State Detected! Terminating now...\n");
                exit(1);
            }

            e2 = processes[p2].num_events;
            ++processes[p2].num_events;
            processes[p2].events = realloc(processes[p2].events,
                                           sizeof(*processes[p2].events)
                                            * processes[p2].num_events);
            processes[p2].events[e2] = processes[p2].events[e2 - !!e2] + 1;

            printf("Process %zu enters Critical Section.\n", p2 + 1);

            break;

        case 0:
            break;

        default:
            puts("Invalid Option!");
            break;
        }

        putchar('\n');
    } while (option);

    puts("Timestamps According to Lamport’s Rule");
    for (i = 0; i < p; ++i) {
        printf("P%zu: ", i + 1);

        for(j = 0; j < processes[i].num_events; ++j)
            printf("%2zu ", processes[i].events[j]);

        putchar('\n');
    }

    return 0;
}
/* end of lamport-mutex.c */

/* OUTPUT

Number of Processes?
3

Add Events (Choose an option)
1. Isolated Event
2. Communicated Event
3. Critical Section Request
4. Critical Section Release
0. Done

?> 1
Process?
1

?> 2
From?
1
To?
3

?> 3
Process?
3
Process 3 enters Critical Section.

?> 3
Process?
2

?> 3
Process?
3
Process 3 already in Critical Section.

?> 4
Process 3 leaves Critical Section.
Process 2 enters Critical Section.

?> 4
Process 2 leaves Critical Section.
No Process waiting for Critical Section.

?> 4
No Process in Critical Section!

?> 0

Timestamps According to Lamport’s Rule
P1:  1  2  5  7  9 12
P2:  5  6  9 10 11
P3:  3  4  7  8 12

*/
