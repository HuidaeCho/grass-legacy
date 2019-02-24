/* sos/above3.c */

/*--------------------------------------------------------------------------*/

#include "basic.h"
#include "sos.h"
#include "internal.h"

/*--------------------------------------------------------------------------*/

int sos_above3 (i, j, k, l)
     int i, j, k, l;
     /* SoS predicate above3.
        Assumes indices in proper range and pairwise different. */
{
  int result, s, d1, d2;
#ifdef __DEBUG__
  if (sos_proto_flag)
    print ("sos_above3 (%d,%d,%d,%d)\n", i, j, k, l); 
#endif
  (void) basic_isort3 (&i, &j, &k);
  d1 = sos_lambda3 (i, j, k) -> signum;
  s = basic_isort4p (&i, &j, &k, &l);
  d2 = sos_lambda4 (i, j, k, l) -> signum;
  result = If (Odd (s), (d1 == d2), (d1 != d2));
#ifdef __DEBUG__
  if (sos_test_flag)
    if (not ((0 < i) and (i < j) and (j < k) and (k < l)
             and (l <= sos_common.n)))
      basic_error ("sos_above3: arguments were wrong");
  if (sos_proto_flag)
    print ("sos_above3 (%d,%d,%d,%d) result: %d (%d %d %d)\n",
           i, j, k, l, result, s, d1, d2);
#endif
  return (result);
}
