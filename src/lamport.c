/* lamport.c */
/* Date  : 28 July 2017
 * Author: Ankit Pati
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t *events, num_events;
    /* events       timestamps for each event in each process
     * num_events   is an array having number of events for each process
     */
} process;

int main()
{
    size_t p, i, j, p1, p2, e1, e2;
    /* p            is number of processes
     * i, j         are counters
     * p1, p2       are current processes under consideration
     * e1, e2       are current events under consideration
     */

    int option;
    process *processes;

    puts("Number of Processes?");
    scanf(" %zu%*c", &p);

    processes = malloc(sizeof(*processes) * p);

    for (i = 0; i < p; ++i) {
        processes[i].events     = NULL;
        processes[i].num_events = 0;
    }

    puts(
        "\n"
        "Add Events (Choose an option)\n"
        "1. Isolated Event\n"
        "2. Communicated Event\n"
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
/* end of lamport.c */

/* OUTPUT

Number of Processes?
3

Add Events (Choose an option)
1. Isolated Event
2. Communicated Event
0. Done

?> 2
From?
1
To?
2

?> 1
Process?
3

?> 1
Process?
3

?> 2
From?
3
To?
1

?> 0

Timestamps According to Lamport’s Rule
P1:  1  4
P2:  2
P3:  1  2  3

*/
