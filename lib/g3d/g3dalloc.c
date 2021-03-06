#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include "G3d_intern.h"

/*---------------------------------------------------------------------------*/


/*!
 * \brief 
 *
 *  Same as <em>malloc (nBytes)</em>, except that in case of error
 * <tt>G3d_error()</tt> is invoked.
 *
 *  \param nBytes
 *  \return void *: a pointer ... if successful,
 * NULL ... otherwise.

 */

void *G3d_malloc(int nBytes)
{
    void *buf;

    if (nBytes <= 0)
	nBytes = 1;
    if ((buf = malloc(nBytes)) != NULL)
	return buf;

    G3d_error("G3d_malloc: out of memory");
    return (void *)NULL;
}


/*!
 * \brief 
 *
 *  Same as <em>realloc (ptr, nBytes)</em>, except that in case of error
 *  <tt>G3d_error()</tt> is invoked. 
 *
 *  \param ptr
 *  \param nBytes
 *  \return void *: a pointer ... if successful,
 *         NULL ... otherwise.
 */

void *G3d_realloc(void *ptr, int nBytes)
{
    if (nBytes <= 0)
	nBytes = 1;
    if ((ptr = realloc(ptr, nBytes)) != NULL)
	return ptr;

    G3d_error("G3d_realloc: out of memory");
    return (void *)NULL;
}


/*!
 * \brief 
 *
 *  Same as <em>free (ptr)</em>.
 *
 *  \param ptr
 *  \return void
 */

void G3d_free(void *buf)
{
    free(buf);
}
