// stb_ds.h - v0.1 - public domain data structures - http://nothings.org/stb_ds
#pragma once

#ifndef INCLUDE_STB_DS_H
#define INCLUDE_STB_DS_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifndef STBDS_NO_SHORT_NAMES
#define arrlen      stbds_arrlen
#define arrlenu     stbds_arrlenu
#define arrput      stbds_arrput
#define arrpush     stbds_arrput
#define arrfree     stbds_arrfree
#define arraddn     stbds_arraddn
#define arrsetlen   stbds_arrsetlen
#define arrlast     stbds_arrlast
#define arrins      stbds_arrins
#define arrinsn     stbds_arrinsn
#define arrdel      stbds_arrdel
#define arrdeln     stbds_arrdeln
#define arrdelswap  stbds_arrdelswap
#define arrcap      stbds_arrcap
#define arrsetcap   stbds_arrsetcap

#define hmput       stbds_hmput
#define hmputs      stbds_hmputs
#define hmget       stbds_hmget
#define hmgets      stbds_hmgets
#define hmgetp      stbds_hmgetp
#define hmgeti      stbds_hmgeti
#define hmdel       stbds_hmdel
#define hmlen       stbds_hmlen
#define hmlenu      stbds_hmlenu
#define hmfree      stbds_hmfree
#define hmdefault   stbds_hmdefault
#define hmdefaults  stbds_hmdefaults

#define shput       stbds_shput
#define shputs      stbds_shputs
#define shget       stbds_shget
#define shgets      stbds_shgets
#define shgetp      stbds_shgetp
#define shgeti      stbds_shgeti
#define shdel       stbds_shdel
#define shlen       stbds_shlen
#define shlenu      stbds_shlenu
#define shfree      stbds_shfree
#define shdefault   stbds_shdefault
#define shdefaults  stbds_shdefaults
#define sh_new_arena  stbds_sh_new_arena
#define sh_new_strdup stbds_sh_new_strdup

#define stralloc    stbds_stralloc
#define strreset    stbds_strreset
#endif

#ifdef __cplusplus
extern "C"
{

#endif

// for security against attackers, seed the library with a random number, at least time() but stronger is better
extern void stbds_rand_seed(size_t seed);

// these are the hash functions used internally if you want to test them or use them for other purposes
extern size_t stbds_hash_bytes(void *p, size_t len, size_t seed);

extern size_t stbds_hash_string(const char *str, size_t seed);

extern size_t stbds_siphash_bytes(void *p, size_t len, size_t seed); // no special case for 4/8-byte data

// this is a simple string arena allocator, initialize with e.g. 'stbds_string_arena my_arena={0}'.
typedef struct stbds_string_arena stbds_string_arena;

extern char *stbds_stralloc(stbds_string_arena *a, const char *str);

extern void stbds_strreset(stbds_string_arena *a);

// have to #define STBDS_UNIT_TESTS to call this
extern void stbds_unit_tests(void);

///////////////
//
// Everything below here is implementation details
//

extern void *stbds_arrgrowf(void *a, size_t elemsize, size_t addlen, size_t min_cap);

extern void stbds_hmfree_func(void *p, size_t elemsize, size_t keyoff);

extern void *stbds_hmget_key(void *a, size_t elemsize, void *key, size_t keysize, int mode);

extern void *stbds_hmput_default(void *a, size_t elemsize);

extern void *stbds_hmput_key(void *a, size_t elemsize, void *key, size_t keysize, int mode);

extern void *stbds_hmdel_key(void *a, size_t elemsize, void *key, size_t keysize, size_t keyoffset, int mode);

extern void *stbds_shmode_func(size_t elemsize, int mode);

#ifdef __cplusplus
}
#endif

#if defined(__GNUC__) || defined(__clang__)
#define STBDS_HAS_TYPEOF
#ifdef __cplusplus
#define STBDS_HAS_LITERAL_ARRAY
#endif
#endif

#if !defined(__cplusplus)
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define STBDS_HAS_LITERAL_ARRAY
#endif
#endif

// this macro takes the address of the argument, but on gcc/clang can accept rvalues
#if defined(STBDS_HAS_LITERAL_ARRAY) && defined(STBDS_HAS_TYPEOF)
#define STBDS_ADDRESSOF(typevar, value)     ((typeof(typevar)[1]){value}) // literal array decays to pointer to value
#else
#define STBDS_ADDRESSOF(typevar, value)     &(value)
#endif

#define STBDS_OFFSETOF(var,field)           ((char *) &(var)->field - (char *) (var))

#define stbds_header(t)  ((stbds_array_header *) (t) - 1)
#define stbds_temp(t)    stbds_header(t)->temp

#define stbds_arrsetcap(a,n) (stbds_arrgrow(a,0,n))
#define stbds_arrsetlen(a,n) ((stbds_arrcap(a) < n ? stbds_arrsetcap(a,n),0 : 0), (a) ? stbds_header(a)->length = (n) : 0)
#define stbds_arrcap(a)       ((a) ? stbds_header(a)->capacity : 0)
#define stbds_arrlen(a)       ((a) ? (ptrdiff_t) stbds_header(a)->length : 0)
#define stbds_arrlenu(a)      ((a) ?             stbds_header(a)->length : 0)
#define stbds_arrput(a,v)     (stbds_arrmaybegrow(a,1), (a)[stbds_header(a)->length++] = (v))
#define stbds_arrpush         stbds_arrput  // synonym
#define stbds_arraddn(a,n)    (stbds_arrmaybegrow(a,n), stbds_header(a)->length += (n))
#define stbds_arrlast(a)      ((a)[stbds_header(a)->length-1])
#define stbds_arrfree(a)      ((void) ((a) ? realloc(stbds_header(a),0) : 0), (a)=NULL)
#define stbds_arrdel(a,i)     stbds_arrdeln(a,i,1)
#define stbds_arrdeln(a,i,n)  (memmove(&(a)[i], &(a)[(i)+(n)], sizeof *(a) * (stbds_header(a)->length-(n)-(i))), stbds_header(a)->length -= (n))
#define stbds_arrdelswap(a,i) ((a)[i] = stbds_arrlast(a), stbds_header(a)->length -= 1)
#define stbds_arrinsn(a,i,n)  (stbds_arraddn((a),(n)), memmove(&(a)[(i)+(n)], &(a)[i], sizeof *(a) * (stbds_header(a)->length-(n)-(i))))
#define stbds_arrins(a,i,v)   (stbds_arrinsn((a),(i),1), (a)[i]=(v))

#define stbds_arrmaybegrow(a,n)  ((!(a) || stbds_header(a)->length + (n) > stbds_header(a)->capacity) \
                                  ? (stbds_arrgrow(a,n,0),0) : 0)

#define stbds_arrgrow(a,b,c)   ((a) = stbds_arrgrowf((a), sizeof *(a), (b), (c)))

#define stbds_hmput(t, k, v) \
    ((t) = stbds_hmput_key_wrapper((t), sizeof *(t), STBDS_ADDRESSOF((t)->key, (k)), sizeof (t)->key, 0),   \
     (t)[stbds_temp((t)-1)].key = (k), \
     (t)[stbds_temp((t)-1)].value = (v))

#define stbds_hmputs(t, s) \
    ((t) = stbds_hmput_key_wrapper((t), sizeof *(t), &(s).key, sizeof (s).key, STBDS_HM_BINARY), \
     (t)[stbds_temp((t)-1)] = (s))

#define stbds_hmgeti(t,k) \
    ((t) = stbds_hmget_key_wrapper((t), sizeof *(t), STBDS_ADDRESSOF((t)->key, (k)), sizeof (t)->key, STBDS_HM_BINARY), \
      stbds_temp((t)-1))

#define stbds_hmgetp(t, k) \
    ((void) stbds_hmgeti(t,k), &(t)[stbds_temp((t)-1)])

#define stbds_hmdel(t,k) \
    (stbds_hmdel_key((t),sizeof *(t), STBDS_ADDRESSOF((t)->key, (k)), sizeof (t)->key, STBDS_OFFSETOF((t),key), STBDS_HM_BINARY))

#define stbds_hmdefault(t, v) \
    ((t) = stbds_hmput_default_wrapper((t), sizeof *(t)), (t)[-1].value = (v))

#define stbds_hmdefaults(t, s) \
    ((t) = stbds_hmput_default_wrapper((t), sizeof *(t)), (t)[-1] = (s))

#define stbds_hmfree(p)        \
    ((void) ((p) != NULL ? stbds_hmfree_func((p)-1,sizeof*(p),STBDS_OFFSETOF((p),key)),0 : 0),(p)=NULL)

#define stbds_hmgets(t, k) (*stbds_hmgetp(t,k))
#define stbds_hmget(t, k)  (stbds_hmgetp(t,k)->value)
#define stbds_hmlen(t)     (stbds_arrlen((t)-1)-1)
#define stbds_hmlenu(t)    (stbds_arrlenu((t)-1)-1)

#define stbds_shput(t, k, v) \
    ((t) = stbds_hmput_key_wrapper((t), sizeof *(t), (k), sizeof (t)->key, STBDS_HM_STRING),   \
     (t)[stbds_temp(t-1)].value = (v))

#define stbds_shputs(t, s) \
    ((t) = stbds_hmput_key_wrapper((t), sizeof *(t), (s).key, sizeof (s).key, STBDS_HM_STRING), \
     (t)[stbds_temp(t-1)] = (s))

#define stbds_shgeti(t,k) \
     ((t) = stbds_hmget_key_wrapper((t), sizeof *(t), (k), sizeof (t)->key, STBDS_HM_STRING), \
      stbds_temp(t))

#define stbds_shgetp(t, k) \
    ((void) stbds_shgeti(t,k), &(t)[stbds_temp(t-1)])

#define stbds_shdel(t,k) \
    ((t) = stbds_hmdel_key((t),sizeof *(t), (k), sizeof (t)->key, STBDS_OFFSETOF((t),key), STBDS_HM_STRING))

#define stbds_sh_new_arena(t)  \
    ((t) = stbds_shmode_func_wrapper(t, sizeof *(t), STBDS_SH_ARENA))
#define stbds_sh_new_strdup(t) \
    ((t) = stbds_shmode_func_wrapper(t, sizeof *(t), STBDS_SH_STRDUP))

#define stbds_shdefault(t, v) stbds_hmdefault(t,v)
#define stbds_shdefaults(t, s) stbds_hmdefaults(t,s)

#define stbds_shfree       stbds_hmfree
#define stbds_shlenu       stbds_hmlenu

#define stbds_shgets(t, k) (*stbds_shgetp(t,k))
#define stbds_shget(t, k)  (stbds_shgetp(t,k)->value)
#define stbds_shlen        stbds_hmlen

typedef struct
{
	size_t length;
	size_t capacity;
	void *hash_table;
	ptrdiff_t temp;
} stbds_array_header;

typedef struct stbds_string_block
{
	struct stbds_string_block *next;
	char storage[8];
} stbds_string_block;

struct stbds_string_arena
{
	stbds_string_block *storage;
	size_t remaining;
	unsigned char block;
	unsigned char mode; // this isn't used by the string arena itself
};

enum
{
	STBDS_HM_BINARY,
	STBDS_HM_STRING,
};

enum
{
	STBDS_SH_NONE,
	STBDS_SH_STRDUP,
	STBDS_SH_ARENA
};

#ifdef __cplusplus
// in C we use implicit assignment from these void*-returning functions to T*.
// in C++ these templates make the same code work
template<class T> static T *stbds_arrgrowf_wrapper(T *a, size_t elemsize, size_t addlen, size_t min_cap)
{
	return (T *) stbds_arrgrowf((void *) a, elemsize, addlen, min_cap);
}
template<class T> static T *stbds_hmget_key_wrapper(T *a, size_t elemsize, void *key, size_t keysize, int mode)
{
	return (T *) stbds_hmget_key((void *) a, elemsize, key, keysize, mode);
}
template<class T> static T *stbds_hmput_default_wrapper(T *a, size_t elemsize)
{
	return (T *) stbds_hmput_default((void *) a, elemsize);
}
template<class T> static T *stbds_hmput_key_wrapper(T *a, size_t elemsize, void *key, size_t keysize, int mode)
{
	return (T *) stbds_hmput_key((void *) a, elemsize, key, keysize, mode);
}
template<class T> static T *stbds_hmdel_key_wrapper(
	T *a, size_t elemsize, void *key, size_t keysize, size_t keyoffset, int mode)
{
	return (T *) stbds_hmdel_key((void *) a, elemsize, key, keysize, keyoffset, mode);
}
template<class T> static T *stbds_shmode_func_wrapper(T *, size_t elemsize, int mode)
{
	return stbds_shmode_func(elemsize, mode);
}
#else
#define stbds_arrgrowf_wrapper            stbds_arrgrowf
#define stbds_hmget_key_wrapper           stbds_hmget_key
#define stbds_hmput_default_wrapper       stbds_hmput_default
#define stbds_hmput_key_wrapper           stbds_hmput_key
#define stbds_hmdel_key_wrapper           stbds_hmdel_key
#define stbds_shmode_func_wrapper(t,e,m)  stbds_shmode_func(e,m)
#endif

#endif

#ifdef STB_DS_IMPLEMENTATION
#include <assert.h>
#include <string.h>

#ifndef STBDS_ASSERT
#define STBDS_ASSERT(x)   ((void) 0)
#endif

#endif

/*
Copyright (c) 2019 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
