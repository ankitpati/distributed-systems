/* addnum.c */
/* Date  : 02 November 2017
 * Author: Ankit Pati
 */

#include "add.h"

double *addnum_1_svc(two_doubles *d, struct svc_req *req)
{
    static double sum;
    sum = d->a + d->b;
    return &sum;
}
/* end of addnum.c */
