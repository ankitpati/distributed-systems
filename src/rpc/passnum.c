/* passnum.c */
/* Date  : 02 November 2017
 * Author: Ankit Pati
 */

#include <stdio.h>
#include <stdlib.h>
#include "add.h"

int main(int argc, char **argv)
{
    CLIENT *clnt;
    double *sum;
    char *server;
    two_doubles nums;

    if (argc != 4) {
        fprintf(stderr, "Usage:\n\tpassnum <server> <number 1> <number 2>\n");
        exit(1);
    }

    server = argv[1];
    nums.a = atof(argv[2]);
    nums.b = atof(argv[3]);

    if (!(clnt = clnt_create(server, ADDPROG, ADDVER, "tcp"))) {
        clnt_pcreateerror(server);
        exit(2);
    }

    if (!(sum = addnum_1(&nums, clnt))) {
        fprintf(stderr, "Error while calling Remote Procedure!\n");
        exit(3);
    }

    printf("%lf + %lf = %lf\n", nums.a, nums.b, *sum);

    clnt_destroy(clnt);
    return 0;
}
/* end of passnum.c */
