#ifndef __STRUCTURES_BLIST_H__
#define __STRUCTURES_BLIST_H__

#include "structures/global_defs.h"

typedef struct blist_s blist;
typedef struct blist_iter_s blist_iter;

blist *blist_create(void);
int    blist_len(blist *list);
void   blist_append(blist *list, void *item);
void   blist_insert(blist *list, void *item, int index);
int    blist_index(blist *list, void *item, equals eq);
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

// add sort
// add splice
// add concat

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

#endif
