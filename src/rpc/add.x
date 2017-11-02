/* add.x */
/* Date  : 02 November 2017
 * Author: Ankit Pati
 */

struct two_doubles {
    double a;
    double b;
};

typedef struct two_doubles two_doubles;

program ADDPROG {
    version ADDVER {
        double ADDNUM(two_doubles) = 1;
    } = 1;
} = 0x20000001;

/* end of add.x */
