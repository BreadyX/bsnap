#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

typedef struct blist_s blist;
typedef struct blist_iter_s blist_iter;

typedef struct bdict_s bdict;
typedef struct bdict_iter_s bdict_iter;

// btree

#ifdef __STRUCTURES_INTERNAL

#define BDICT_LOAD_FACTOR 0.6
struct bdict_item_s {
	_Bool tombstone;
	void *item;
	char *key;
};

struct bdict_s {
	int total_size;
	int filled;
	struct bitem_s *items;
};

struct bdict_iter_s {
	int current;
};
#endif

// List/Queue/Stack + iter
blist *blist_create(void);
int    blist_len(blist *list);
void   blist_append(blist *list, void *item);
void   blist_insert(blist *list, void *item, int index);
int    blist_index(blist *list, void *item, _Bool (*equals)(void*, void*));
void  *blist_get(blist *list, int index);
void  *blist_remove(blist *list, int index);
void   blist_clear(blist *list);
void   blist_destroy(blist **list);

inline _Bool blist_empty(blist *list) { return blist_len(list) == 0; }
inline void  blist_push(blist *list, void *item) { blist_insert(list, item, 0); }
inline void *blist_pop(blist *list) { return blist_remove(list, 0); }
inline void *blist_drop(blist *list) { return blist_remove(list, blist_len(list) - 1); }
inline void *blist_peek(blist *list) { return blist_get(list, 0); }
inline void *blist_peek_last(blist *list) { return blist_get(list, blist_len(list) - 1); }

#define blist_enqueue(list, item) do { blist_append(list, item); } while(0)

blist_iter *blist_iter_create(blist *iter);
int         blist_iter_index(blist_iter *iter);
void       *blist_iter_prev(blist_iter *iter);
void       *blist_iter_next(blist_iter *iter);
_Bool       blist_iter_has_prev(blist_iter *iter);
_Bool       blist_iter_has_next(blist_iter *iter);
_Bool       blist_iter_is_done(blist_iter *iter);
void       *blist_iter_get(blist_iter *iter);
void        blist_iter_insert(blist_iter *iter, void *item, _Bool after);
void       *blist_iter_remove(blist_iter *iter, _Bool go_next);
void        blist_iter_jump_first(blist_iter *iter);
void        blist_iter_jump_last(blist_iter *iter);
void        blist_iter_destroy(blist_iter **iter);

#define blist_iter_insert_here(list, item) do { blist_iter_insert(list, item, 0) } while(0)

// Dict
bdict *bdict_create(void);
void   bdict_insert(bdict *dict, char *key, void *value);  // also counts as update
void  *bdict_get(bdict *dict, char *key);
void  *bdict_remove(bdict *dict, char *key);
void   bdict_destroy(bdict **dict);

bdict_iter *bdict_iter_create(void);
void       *bdict_iter_next(bdict_iter *iter);
_Bool       bdict_iter_is_done(bdict_iter *iter);
void        bdict_iter_destroy(bdict_iter **iter);

#endif
