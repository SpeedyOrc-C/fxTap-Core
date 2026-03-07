#include <stb_ds.h>

void *stbds_arrgrowf(void *a, const size_t elemsize, const size_t addlen, size_t min_cap)
{
	const size_t min_len = stbds_arrlen(a) + addlen;

	// compute the minimum capacity needed
	if (min_len > min_cap)
		min_cap = min_len;

	if (min_cap <= stbds_arrcap(a))
		return a;

	// increase needed capacity to guarantee O(1) amortized
	if (min_cap < 2 * stbds_arrcap(a))
		min_cap = 2 * stbds_arrcap(a);
	else if (min_cap < 4)
		min_cap = 4;

	void *b = realloc(a ? stbds_header(a) : nullptr, elemsize * min_cap + sizeof(stbds_array_header));
	b = (char *) b + sizeof(stbds_array_header);
	if (a == nullptr)
	{
		stbds_header(b)->length = 0;
		stbds_header(b)->hash_table = nullptr;
	}
	stbds_header(b)->capacity = min_cap;
	return b;
}

//
// stbds_hm hash table implementation
//

#define STBDS_CACHE_LINE_SIZE   64
#define STBDS_BUCKET_LENGTH      8
#define STBDS_BUCKET_SHIFT       3
#define STBDS_BUCKET_MASK       (STBDS_BUCKET_LENGTH-1)

#define STBDS_ALIGN_FWD(n,a)   (((n) + (a) - 1) & ~((a)-1))

typedef struct
{
	size_t hash[STBDS_BUCKET_LENGTH];
	ptrdiff_t index[STBDS_BUCKET_LENGTH];
} stbds_hash_bucket; // in 32-bit, this is one 64-byte cache line; in 64-bit, each array is one 64-byte cache line

typedef struct
{
	size_t slot_count;
	size_t used_count;
	size_t used_count_threshold;
	size_t used_count_shrink_threshold;
	size_t tombstone_count;
	size_t tombstone_count_threshold;
	size_t seed;
	stbds_string_arena string;
	stbds_hash_bucket *storage; // not a separate allocation, just 64-byte aligned storage after this struct
} stbds_hash_index;

#define STBDS_INDEX_EMPTY    (-1)
#define STBDS_INDEX_DELETED  (-2)
#define STBDS_INDEX_IN_USE(x)  ((x) >= 0)

#define STBDS_HASH_EMPTY      0
#define STBDS_HASH_DELETED    1

static size_t stbds_hash_seed = 0x31415926;

void stbds_rand_seed(const size_t seed)
{
	stbds_hash_seed = seed;
}

#define stbds_load_32_or_64(var, temp, v32, v64_hi, v64_lo)                                          \
  temp = v64_lo ^ v32, temp <<= 16, temp <<= 16, temp >>= 16, temp >>= 16, /* discard if 32-bit */   \
  var = v64_hi, var <<= 16, var <<= 16,                                    /* discard if 32-bit */   \
  var ^= temp ^ v32

static stbds_hash_index *stbds_make_hash_index(const size_t slot_count, const stbds_hash_index *ot)
{
	stbds_hash_index *t = realloc(
		nullptr, (slot_count >> STBDS_BUCKET_SHIFT) * sizeof(stbds_hash_bucket) + sizeof(stbds_hash_index) +
		   STBDS_CACHE_LINE_SIZE - 1);
	t->storage = (stbds_hash_bucket *) STBDS_ALIGN_FWD((size_t) (t+1), STBDS_CACHE_LINE_SIZE);
	t->slot_count = slot_count;
	STBDS_ASSERT(t->slot_count != 50001);
	t->tombstone_count = 0;
	t->used_count = 0;
	t->used_count_threshold = slot_count * 13 / 16; // if 12/16th of table is occupied, rebuild
	t->tombstone_count_threshold = slot_count * 1 / 16; // if 14/16th of table is occupied+tombstones, rebuild
	t->used_count_shrink_threshold = slot_count * 6 / 16; // if table is only 6/16th
	if (ot)
	{
		t->string = ot->string;
		// reuse old seed so we can reuse old hashes so below "copy out old data" doesn't do any hashing
		t->seed = ot->seed;
	}
	else
	{
		size_t a, b, temp;
		memset(&t->string, 0, sizeof(t->string));
		t->seed = stbds_hash_seed;
		// LCG
		// in 32-bit, a =          2147001325   b =  715136305
		// in 64-bit, a = 2862933555777941757   b = 3037000493
		stbds_load_32_or_64(a, temp, 2147001325, 0x27bb2ee6, 0x87b0b0fd);
		stbds_load_32_or_64(b, temp, 715136305, 0, 0xb504f32d);
		stbds_hash_seed = stbds_hash_seed * a + b;
	}

	{
		size_t j;
		for (size_t i = 0; i < slot_count >> STBDS_BUCKET_SHIFT; ++i)
		{
			stbds_hash_bucket *b = &t->storage[i];
			for (j = 0; j < STBDS_BUCKET_LENGTH; ++j)
				b->hash[j] = STBDS_HASH_EMPTY;
			for (j = 0; j < STBDS_BUCKET_LENGTH; ++j)
				b->index[j] = STBDS_INDEX_EMPTY;
		}
	}

	// copy out the old data, if any
	if (ot)
	{
		t->used_count = ot->used_count;
		for (size_t i = 0; i < ot->slot_count >> STBDS_BUCKET_SHIFT; ++i)
		{
			const stbds_hash_bucket *ob = &ot->storage[i];
			for (size_t j = 0; j < STBDS_BUCKET_LENGTH; ++j)
			{
				if (STBDS_INDEX_IN_USE(ob->index[j]))
				{
					const size_t hash = ob->hash[j];
					size_t pos = hash & t->slot_count - 1;
					size_t step = STBDS_BUCKET_LENGTH;
					for (;;)
					{
						size_t limit, z;
						pos &= t->slot_count - 1;
						stbds_hash_bucket *bucket = &t->storage[pos >> STBDS_BUCKET_SHIFT];

						for (z = pos & STBDS_BUCKET_MASK; z < STBDS_BUCKET_LENGTH; ++z)
						{
							if (bucket->hash[z] == 0)
							{
								bucket->hash[z] = hash;
								bucket->index[z] = ob->index[j];
								goto done;
							}
						}

						limit = pos & STBDS_BUCKET_MASK;
						for (z = 0; z < limit; ++z)
						{
							if (bucket->hash[z] == 0)
							{
								bucket->hash[z] = hash;
								bucket->index[z] = ob->index[j];
								goto done;
							}
						}

						pos += step; // quadratic probing
						step += STBDS_BUCKET_LENGTH;
					}
				}
			done:
				;
			}
		}
	}

	return t;
}

#define STBDS_SIZE_T_BITS           ((sizeof (size_t)) * 8)
#define STBDS_ROTATE_LEFT(val, n)   (((val) << (n)) | ((val) >> (STBDS_SIZE_T_BITS - (n))))
#define STBDS_ROTATE_RIGHT(val, n)  (((val) >> (n)) | ((val) << (STBDS_SIZE_T_BITS - (n))))

size_t stbds_hash_string(const char *str, const size_t seed)
{
	size_t hash = seed;
	while (*str)
		hash = STBDS_ROTATE_LEFT(hash, 9) + (unsigned char) *str++;

	// Thomas Wang 64-to-32 bit mix function, hopefully also works in 32 bits
	hash ^= seed;
	hash = ~hash + (hash << 18);
	hash ^= hash ^ STBDS_ROTATE_RIGHT(hash, 31);
	hash = hash * 21;
	hash ^= hash ^ STBDS_ROTATE_RIGHT(hash, 11);
	hash += hash << 6;
	hash ^= STBDS_ROTATE_RIGHT(hash, 22);
	return hash + seed;
}

#ifdef STBDS_SIPHASH_2_4
#define STBDS_SIPHASH_C_ROUNDS 2
#define STBDS_SIPHASH_D_ROUNDS 4
typedef int STBDS_SIPHASH_2_4_can_only_be_used_in_64_bit_builds[sizeof(size_t) == 8 ? 1 : -1];
#endif

#ifndef STBDS_SIPHASH_C_ROUNDS
#define STBDS_SIPHASH_C_ROUNDS 1
#endif
#ifndef STBDS_SIPHASH_D_ROUNDS
#define STBDS_SIPHASH_D_ROUNDS 1
#endif

size_t stbds_hash_bytes(void *p, const size_t len, const size_t seed)
{
	const unsigned char *d = p;

	if (len == 4)
	{
		unsigned int hash = d[0] | d[1] << 8 | d[2] << 16 | d[3] << 24;
		hash ^= seed;
		hash -= hash << 6;
		hash ^= hash >> 17;
		hash -= hash << 9;
		hash ^= hash << 4;
		hash -= hash << 3;
		hash ^= hash << 10;
		hash ^= hash >> 15;
		return hash ^ seed;
	}
	if (len == 8 && sizeof(size_t) == 8)
	{
		size_t hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24) | ((size_t) d[4] << 32) | ((size_t) d[5] << 40) | (
			              (size_t) d[6] << 48) | ((size_t) d[7] << 56);
		hash ^= seed;
		hash = (~hash) + (hash << 21);
		hash ^= STBDS_ROTATE_RIGHT(hash, 24);
		hash *= 265;
		hash ^= STBDS_ROTATE_RIGHT(hash, 14);
		hash *= 21;
		hash ^= STBDS_ROTATE_RIGHT(hash, 28);
		hash += (hash << 31);
		hash = (~hash) + (hash << 18);
		return hash ^ seed;
	}
	return stbds_siphash_bytes(p, len, seed);
}

size_t stbds_siphash_bytes(void *p, const size_t len, const size_t seed)
{
	unsigned char *d = p;
	size_t i, j;
	size_t data;

	// hash that works on 32- or 64-bit registers without knowing which we have
	// (computes different results on 32-bit and 64-bit platform)
	// derived from siphash, but on 32-bit platforms very different as it uses 4 32-bit state not 4 64-bit
	size_t v0 = (((size_t) 0x736f6d65 << 16) << 16) + 0x70736575 ^ seed;
	size_t v1 = (((size_t) 0x646f7261 << 16) << 16) + 0x6e646f6d ^ ~seed;
	size_t v2 = (((size_t) 0x6c796765 << 16) << 16) + 0x6e657261 ^ seed;
	size_t v3 = (((size_t) 0x74656462 << 16) << 16) + 0x79746573 ^ ~seed;

#ifdef STBDS_TEST_SIPHASH_2_4
	// hardcoded with key material in the siphash test vectors
	v0 ^= 0x0706050403020100ull ^ seed;
	v1 ^= 0x0f0e0d0c0b0a0908ull ^ ~seed;
	v2 ^= 0x0706050403020100ull ^ seed;
	v3 ^= 0x0f0e0d0c0b0a0908ull ^ ~seed;
#endif

#define STBDS_SIPROUND() \
  do {                   \
    v0 += v1; v1 = STBDS_ROTATE_LEFT(v1, 13);  v1 ^= v0; v0 = STBDS_ROTATE_LEFT(v0,STBDS_SIZE_T_BITS/2); \
    v2 += v3; v3 = STBDS_ROTATE_LEFT(v3, 16);  v3 ^= v2;                                                 \
    v2 += v1; v1 = STBDS_ROTATE_LEFT(v1, 17);  v1 ^= v2; v2 = STBDS_ROTATE_LEFT(v2,STBDS_SIZE_T_BITS/2); \
    v0 += v3; v3 = STBDS_ROTATE_LEFT(v3, 21);  v3 ^= v0;                                                 \
  } while (0)

	for (i = 0; i + sizeof(size_t) <= len; i += sizeof(size_t), d += sizeof(size_t))
	{
		data = d[0] | d[1] << 8 | d[2] << 16 | d[3] << 24;
		data |= (size_t) (d[4] | d[5] << 8 | d[6] << 16 | d[7] << 24) << 16 << 16; // discarded if size_t == 4

		v3 ^= data;
		for (j = 0; j < STBDS_SIPHASH_C_ROUNDS; ++j)
			STBDS_SIPROUND();
		v0 ^= data;
	}
	data = len << (STBDS_SIZE_T_BITS - 8);
	switch (len - i)
	{
	case 7: data |= (size_t) d[6] << 48;
	case 6: data |= (size_t) d[5] << 40;
	case 5: data |= (size_t) d[4] << 32;
	case 4: data |= d[3] << 24;
	case 3: data |= d[2] << 16;
	case 2: data |= d[1] << 8;
	case 1: data |= d[0];
	case 0: break;
	}
	v3 ^= data;
	for (j = 0; j < STBDS_SIPHASH_C_ROUNDS; ++j)
		STBDS_SIPROUND();
	v0 ^= data;
	v2 ^= 0xff;
	for (j = 0; j < STBDS_SIPHASH_D_ROUNDS; ++j)
		STBDS_SIPROUND();
#ifdef STBDS_SIPHASH_2_4
	return v0 ^ v1 ^ v2 ^ v3;
#else
	return v1 ^ v2 ^ v3; // slightly stronger since v0^v3 in above cancels out final round operation
#endif
}

static int stbds_is_key_equal(void *a, const size_t elemsize, const void *key, const size_t keysize, const int mode, const size_t i)
{
	if (mode >= STBDS_HM_STRING)
		return 0 == strcmp(key, *(char **) ((char *) a + elemsize * i));
	return 0 == memcmp(key, (char *) a + elemsize * i, keysize);
}

#define STBDS_HASH_TO_ARR(x,elemsize) ((char*) (x) - (elemsize))
#define STBDS_ARR_TO_HASH(x,elemsize) ((char*) (x) + (elemsize))
#define STBDS_FREE(x)   realloc(x,0)

#define stbds_hash_table(a)  ((stbds_hash_index *) stbds_header(a)->hash_table)

void stbds_hmfree_func(void *p, const size_t elemsize, size_t keyoff)
{
	if (p == nullptr) return;
	if (stbds_hash_table(p) != nullptr)
	{
		if (stbds_hash_table(p)->string.mode == STBDS_SH_STRDUP)
		{
			// skip 0th element, which is default
			for (size_t i = 1; i < stbds_header(p)->length; ++i)
				STBDS_FREE(*(char**) ((char *) p + elemsize*i));
		}
		stbds_strreset(&stbds_hash_table(p)->string);
	}
	STBDS_FREE(stbds_header(p)->hash_table);
	STBDS_FREE(stbds_header(p));
}

static ptrdiff_t stbds_hm_find_slot(void *a, const size_t elemsize, void *key, const size_t keysize, const int mode)
{
	void *raw_a = STBDS_HASH_TO_ARR(a, elemsize);
	const stbds_hash_index *table = stbds_hash_table(raw_a);
	size_t hash = mode >= STBDS_HM_STRING
		              ? stbds_hash_string(key, table->seed)
		              : stbds_hash_bytes(key, keysize, table->seed);
	size_t step = STBDS_BUCKET_LENGTH;
	size_t i;

	if (hash < 2) hash += 2; // stored hash values are forbidden from being 0, so we can detect empty slots

	size_t pos = hash;

	for (;;)
	{
		pos &= table->slot_count - 1;
		const stbds_hash_bucket *bucket = &table->storage[pos >> STBDS_BUCKET_SHIFT];

		// start searching from pos to end of bucket, this should help performance on small hash tables that fit in cache
		for (i = pos & STBDS_BUCKET_MASK; i < STBDS_BUCKET_LENGTH; ++i)
		{
			if (bucket->hash[i] == hash)
			{
				if (stbds_is_key_equal(a, elemsize, key, keysize, mode, bucket->index[i]))
				{
					return (pos & ~STBDS_BUCKET_MASK) + i;
				}
			}
			else if (bucket->hash[i] == STBDS_HASH_EMPTY)
			{
				return -1;
			}
		}

		// search from beginning of bucket to pos
		const size_t limit = pos & STBDS_BUCKET_MASK;
		for (i = 0; i < limit; ++i)
		{
			if (bucket->hash[i] == hash)
			{
				if (stbds_is_key_equal(a, elemsize, key, keysize, mode, bucket->index[i]))
				{
					return (pos & ~STBDS_BUCKET_MASK) + i;
				}
			}
			else if (bucket->hash[i] == STBDS_HASH_EMPTY)
			{
				return -1;
			}
		}

		// quadratic probing
		pos += step;
		step += STBDS_BUCKET_LENGTH;
	}
}

void *stbds_hmget_key(void *a, const size_t elemsize, void *key, const size_t keysize, const int mode)
{
	if (a == nullptr)
	{
		// make it non-empty so we can return a temp
		a = stbds_arrgrowf(nullptr, elemsize, 0, 1);
		stbds_header(a)->length += 1;
		memset(a, 0, elemsize);
		stbds_temp(a) = STBDS_INDEX_EMPTY;
		// adjust a to point after the default element
		return STBDS_ARR_TO_HASH(a, elemsize);
	}
	void *raw_a = STBDS_HASH_TO_ARR(a, elemsize);
	// adjust a to point to the default element
	const stbds_hash_index *table = (stbds_hash_index *) stbds_header(raw_a)->hash_table;
	if (table == nullptr)
	{
		stbds_temp(raw_a) = -1;
	}
	else
	{
		const ptrdiff_t slot = stbds_hm_find_slot(a, elemsize, key, keysize, mode);
		if (slot < 0)
		{
			stbds_temp(raw_a) = STBDS_INDEX_EMPTY;
		}
		else
		{
			const stbds_hash_bucket *b = &table->storage[slot >> STBDS_BUCKET_SHIFT];
			stbds_temp(raw_a) = b->index[slot & STBDS_BUCKET_MASK];
		}
	}
	return a;
}

void *stbds_hmput_default(void *a, const size_t elemsize)
{
	// three cases:
	//   a is NULL <- allocate
	//   a has a hash table but no entries, because of shmode <- grow
	//   a has entries <- do nothing
	if (a == nullptr || stbds_header(STBDS_HASH_TO_ARR(a,elemsize))->length == 0)
	{
		a = stbds_arrgrowf(a ? STBDS_HASH_TO_ARR(a, elemsize) : nullptr, elemsize, 0, 1);
		stbds_header(a)->length += 1;
		memset(a, 0, elemsize);
		a = STBDS_ARR_TO_HASH(a, elemsize);
	}
	return a;
}

static char *stbds_strdup(const char *str);

static char *stbds_string_arena_alloc(stbds_string_arena *a, char *str);

void *stbds_hmput_key(void *a, const size_t elemsize, void *key, const size_t keysize, const int mode)
{
	if (a == nullptr)
	{
		a = stbds_arrgrowf(nullptr, elemsize, 0, 1);
		memset(a, 0, elemsize);
		stbds_header(a)->length += 1;
		// adjust a to point AFTER the default element
		a = STBDS_ARR_TO_HASH(a, elemsize);
	}

	// adjust a to point to the default element
	void *raw_a = a;
	a = STBDS_HASH_TO_ARR(a, elemsize);

	stbds_hash_index *table = stbds_header(a)->hash_table;

	if (table == nullptr || table->used_count >= table->used_count_threshold)
	{
		const size_t slot_count = table == nullptr ? STBDS_BUCKET_LENGTH : table->slot_count * 2;
		stbds_hash_index *nt = stbds_make_hash_index(slot_count, table);
		if (table)
		{
			STBDS_FREE(table);
		}
		stbds_header(a)->hash_table = table = nt;
	}

	// we iterate hash table explicitly because we want to track if we saw a tombstone
	{
		size_t hash = mode >= STBDS_HM_STRING
			              ? stbds_hash_string(key, table->seed)
			              : stbds_hash_bytes(key, keysize, table->seed);
		size_t step = STBDS_BUCKET_LENGTH;
		size_t limit, i;
		ptrdiff_t tombstone = -1;
		stbds_hash_bucket *bucket;

		// stored hash values are forbidden from being 0, so we can detect empty slots to early out quickly
		if (hash < 2) hash += 2;

		size_t pos = hash;

		for (;;)
		{
			pos &= table->slot_count - 1;
			bucket = &table->storage[pos >> STBDS_BUCKET_SHIFT];

			// start searching from pos to end of bucket
			for (i = pos & STBDS_BUCKET_MASK; i < STBDS_BUCKET_LENGTH; ++i)
			{
				if (bucket->hash[i] == hash)
				{
					if (stbds_is_key_equal(raw_a, elemsize, key, keysize, mode, i))
					{
						stbds_temp(a) = bucket->index[i];
						return STBDS_ARR_TO_HASH(a, elemsize);
					}
				}
				else if (bucket->hash[i] == 0)
				{
					pos = (pos & ~STBDS_BUCKET_MASK) + i;
					goto found_empty_slot;
				}
				else if (tombstone < 0)
				{
					if (bucket->index[i] == STBDS_INDEX_DELETED)
						tombstone = (ptrdiff_t) ((pos & ~STBDS_BUCKET_MASK) + i);
				}
			}

			// search from beginning of bucket to pos
			limit = pos & STBDS_BUCKET_MASK;
			for (i = 0; i < limit; ++i)
			{
				if (bucket->hash[i] == hash)
				{
					if (stbds_is_key_equal(raw_a, elemsize, key, keysize, mode, i))
					{
						stbds_temp(a) = bucket->index[i];
						return STBDS_ARR_TO_HASH(a, elemsize);
					}
				}
				else if (bucket->hash[i] == 0)
				{
					pos = (pos & ~STBDS_BUCKET_MASK) + i;
					goto found_empty_slot;
				}
				else if (tombstone < 0)
				{
					if (bucket->index[i] == STBDS_INDEX_DELETED)
						tombstone = (ptrdiff_t) ((pos & ~STBDS_BUCKET_MASK) + i);
				}
			}

			// quadratic probing
			pos += step;
			step += STBDS_BUCKET_LENGTH;
		}
	found_empty_slot:
		if (tombstone >= 0)
		{
			pos = tombstone;
			--table->tombstone_count;
		}
		else
		{
			++table->used_count;
		}

		{
			ptrdiff_t i = stbds_arrlen(a);
			// we want to do stbds_arraddn(1), but we can't use the macros since we don't have something of the right type
			if ((size_t) i + 1 > stbds_arrcap(a))
				a = stbds_arrgrowf(a, elemsize, 1, 0);
			raw_a = STBDS_ARR_TO_HASH(a, elemsize);

			STBDS_ASSERT((size_t) i+1 <= stbds_arrcap(a));
			stbds_header(a)->length = i + 1;
			bucket = &table->storage[pos >> STBDS_BUCKET_SHIFT];
			bucket->hash[pos & STBDS_BUCKET_MASK] = hash;
			bucket->index[pos & STBDS_BUCKET_MASK] = i - 1;
			stbds_temp(a) = i - 1;

			switch (table->string.mode)
			{
			case STBDS_SH_STRDUP: *(char **) ((char *) a + elemsize * i) = stbds_strdup(key);
				break;
			case STBDS_SH_ARENA: *(char **) ((char *) a + elemsize * i) = stbds_stralloc(&table->string, key);
				break;
			}
		}
		return STBDS_ARR_TO_HASH(a, elemsize);
	}
}

void *stbds_shmode_func(const size_t elemsize, const int mode)
{
	void *a = stbds_arrgrowf(nullptr, elemsize, 0, 1);
	stbds_hash_index *h;
	stbds_header(a)->hash_table = h = stbds_make_hash_index(STBDS_BUCKET_LENGTH, nullptr);
	h->string.mode = mode;
	return STBDS_ARR_TO_HASH(a, elemsize);
}

void *stbds_hmdel_key(
	void *a, const size_t elemsize, void *key, const size_t keysize, const size_t keyoffset, const int mode)
{
	if (a == nullptr)
		return nullptr;
	void *raw_a = STBDS_HASH_TO_ARR(a, elemsize);
	stbds_hash_index *table = stbds_header(raw_a)->hash_table;
	if (table == nullptr)
		return a;
	ptrdiff_t slot = stbds_hm_find_slot(a, elemsize, key, keysize, mode);
	if (slot < 0)
		return a;
	stbds_hash_bucket *b = &table->storage[slot >> STBDS_BUCKET_SHIFT];
	int i = slot & STBDS_BUCKET_MASK;
	const ptrdiff_t old_index = b->index[i];
	const ptrdiff_t final_index = (stbds_arrlen(raw_a)) - 1 - 1;
	// minus one for the raw_a vs a, and minus one for 'last'
	STBDS_ASSERT(slot < (ptrdiff_t) table->slot_count);
	--table->used_count;
	++table->tombstone_count;
	STBDS_ASSERT(table->used_count >= 0);
	//STBDS_ASSERT(table->tombstone_count < table->slot_count/4);
	b->hash[i] = STBDS_HASH_DELETED;
	b->index[i] = STBDS_INDEX_DELETED;

	if (mode == STBDS_HM_STRING && table->string.mode == STBDS_SH_STRDUP)
		STBDS_FREE(*(char**) ((char *) a+elemsize*old_index));

	// swap delete
	if (old_index != final_index)
	{
		// if indices are the same, memcpy is a no-op, but back-pointer-fixup below will fail
		memcpy((char *) a + elemsize * old_index, (char *) a + elemsize * final_index, elemsize);

		// now find the slot for the last element
		if (mode == STBDS_HM_STRING)
			slot = stbds_hm_find_slot(a, elemsize, *(char **) ((char *) a + elemsize * old_index + keyoffset),
			                          keysize, mode);
		else
			slot = stbds_hm_find_slot(a, elemsize, (char *) a + elemsize * old_index + keyoffset, keysize, mode);
		STBDS_ASSERT(slot >= 0);
		b = &table->storage[slot >> STBDS_BUCKET_SHIFT];
		i = slot & STBDS_BUCKET_MASK;
		STBDS_ASSERT(b->index[i] == final_index);
		b->index[i] = old_index;
	}
	stbds_header(raw_a)->length -= 1;

	if (table->used_count < table->used_count_shrink_threshold && table->slot_count > STBDS_BUCKET_LENGTH)
	{
		stbds_header(raw_a)->hash_table = stbds_make_hash_index(table->slot_count >> 1, table);
	}
	else if (table->tombstone_count > table->tombstone_count_threshold)
	{
		stbds_header(raw_a)->hash_table = stbds_make_hash_index(table->slot_count, table);
	}

	return a;
}

static char *stbds_strdup(const char *str)
{
	// to keep replaceable allocator simple, we don't want to use strdup.
	// rolling our own also avoids problem of strdup vs _strdup
	const size_t len = strlen(str) + 1;
	char *p = realloc(nullptr, len);
	memcpy(p, str, len);
	return p;
}

#ifndef STBDS_STRING_ARENA_BLOCKSIZE_MIN
#define STBDS_STRING_ARENA_BLOCKSIZE_MIN  512
#endif
#ifndef STBDS_STRING_ARENA_BLOCKSIZE_MAX
#define STBDS_STRING_ARENA_BLOCKSIZE_MAX  1<<20
#endif

char *stbds_stralloc(stbds_string_arena *a, const char *str)
{
	const size_t len = strlen(str) + 1;
	if (len > a->remaining)
	{
		// compute the next blocksize
		size_t blocksize = a->block;

		// size is 512, 512, 1024, 1024, 2048, 2048, 4096, 4096, etc., so that
		// there are log(SIZE) allocations to free when we destroy the table
		blocksize = (size_t) STBDS_STRING_ARENA_BLOCKSIZE_MIN << (blocksize >> 1);

		// if size is under 1M, advance to next blocktype
		if (blocksize < (size_t) (STBDS_STRING_ARENA_BLOCKSIZE_MAX))
			++a->block;

		if (len > blocksize)
		{
			// if string is larger than blocksize, then just allocate the full size.
			// note that we still advance string_block so block size will continue
			// increasing, so e.g. if somebody only calls this with 1000-long strings,
			// eventually the arena will start doubling and handling those as well
			stbds_string_block *sb = realloc(nullptr, sizeof(*sb) - 8 + len);
			memcpy(sb->storage, str, len);
			if (a->storage)
			{
				// insert it after the first element, so that we don't waste the space there
				sb->next = a->storage->next;
				a->storage->next = sb;
			}
			else
			{
				sb->next = nullptr;
				a->storage = sb;
				a->remaining = 0; // this is redundant, but good for clarity
			}
			return sb->storage;
		}
		stbds_string_block *sb = realloc(nullptr, sizeof(*sb) - 8 + blocksize);
		sb->next = a->storage;
		a->storage = sb;
		a->remaining = blocksize;
	}

	STBDS_ASSERT(len <= a->remaining);
	char *p = a->storage->storage + a->remaining - len;
	a->remaining -= len;
	memcpy(p, str, len);
	return p;
}

void stbds_strreset(stbds_string_arena *a)
{
	stbds_string_block *x = a->storage;
	while (x)
	{
		stbds_string_block *y = x->next;
		realloc(x, 0);
		x = y;
	}
	memset(a, 0, sizeof(*a));
}
