#ifndef __include_cref_h
#define __include_cref_h

struct ref {
    void (*free)(const struct ref *);
    int count;
};

static inline int
ref_inc(const struct ref *ref)
{
	int count = __sync_add_and_fetch((int *)&ref->count, 1);
	return count;
}

static inline int
ref_dec(const struct ref *ref)
{
	int count = __sync_sub_and_fetch((int *)&ref->count, 1);
    if (count == 0)
        ref->free(ref);
}

#endif // #ifndef __include_cref_h
