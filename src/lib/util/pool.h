/***************************************************************************

    pool.h

    Abstract object pool management

****************************************************************************

    Copyright Aaron Giles
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are 
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in 
          the documentation and/or other materials provided with the 
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be 
          used to endorse or promote products derived from this software 
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR 
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT, 
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
    POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

#ifndef __POOL_H__
#define __POOL_H__

#include "osdcore.h"


/***************************************************************************
    MACROS
***************************************************************************/

/* helper macros for memory pools that pass file/line number */
#define pool_malloc(pool, size)			pool_malloc_file_line((pool), (size), __FILE__, __LINE__)
#define pool_realloc(pool, ptr, size)	pool_realloc_file_line((pool), (ptr), (size), __FILE__, __LINE__)
#define pool_strdup(pool, size)			pool_strdup_file_line((pool), (size), __FILE__, __LINE__)

/* macro to define a 4-character type for a pool */
#define OBJECT_TYPE(a,b,c,d)			(((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

/* built-in pool types */
#define OBJTYPE_WILDCARD				(0)
#define OBJTYPE_MEMORY					OBJECT_TYPE('m','e','m','o')



/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

/* pool types are UINT32s */
typedef UINT32 object_type;

/* opaque type representing a pool of objects */
typedef struct _object_pool object_pool;

/* opaque type representing an iterator over pool objects */
typedef struct _object_pool_iterator object_pool_iterator;



/***************************************************************************
    PROTOTYPES
***************************************************************************/


/* ----- object pool management ----- */

/* allocate a new object pool */
object_pool *pool_alloc(void (*fail)(const char *message));

/* register a new object type; returns TRUE if the type already existed and was overridden */
void pool_type_register(object_pool *pool, object_type type, const char *friendly, void (*destructor)(void *, size_t));

/* free all allocated objects in a pool */
void pool_clear(object_pool *pool);

/* free an object pool, including all allocated objects */
void pool_free(object_pool *pool);



/* ----- object management ----- */

/* add an object to the pool, along with its filename/line number */
void *pool_object_add_file_line(object_pool *pool, object_type type, void *object, size_t size, const char *file, int line);

/* remove an object from the pool (optionally calling destructor) */
void *pool_object_remove(object_pool *pool, void *object, int destruct);

/* does an object exist in the pool? */
int pool_object_exists(object_pool *pool, object_type type, void *object);



/* ----- object iterators ----- */

/* begin iterating over objects in an object pool */
object_pool_iterator *pool_iterate_begin(object_pool *pool, object_type type);

/* get the next object in the object pool */
int pool_iterate_next(object_pool_iterator *iter, void **objectptr, size_t *sizeptr, object_type *typeptr);

/* finish iterating over objects in an object pool */
void pool_iterate_end(object_pool_iterator *iter);



/* ----- memory helpers ----- */

/* malloc memory and register it with the given pool */
void *pool_malloc_file_line(object_pool *pool, size_t size, const char *file, int line);

/* realloc memory and register it with the given pool */
void *pool_realloc_file_line(object_pool *pool, void *ptr, size_t size, const char *file, int line);

/* strdup memory and register it with the given pool */
char *pool_strdup_file_line(object_pool *pool, const char *str, const char *file, int line);



/* ----- miscellaneous ----- */

/* internal unit tests */
int test_memory_pools(void);


#endif /* __POOL_H__ */
