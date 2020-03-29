#include <stdlib.h>

#include "structures.h"

struct blist_item_s {
	void *item;
	struct blist_item_s *next;
	struct blist_item_s *prev;
};

struct blist_iter_s {
	blist *list;

	int index;
	struct blist_item_s *current;
	// struct blist_item_s *next;
	// struct blist_item_s *prev;
};

struct blist_s {
	int size;

	struct blist_item_s *first;
	struct blist_item_s *last;
};

extern _Bool blist_empty(blist *list);
extern void  blist_push(blist *list, void *item);
extern void *blist_pop(blist *list);
extern void *blist_drop(blist *list);
extern void *blist_peek(blist *list);
extern void *blist_peek_last(blist *list);

extern _Bool blist_iter_is_done(blist_iter *iter);

static struct blist_item_s *blist_walk_to_forwards(blist *list, int to);
static struct blist_item_s *blist_walk_to_backwards(blist *list, int to);
static void blist_insert_item(blist *list, struct blist_item_s *prev_insert,
		struct blist_item_s *insert);
static void *blist_remove_item(blist *list, struct blist_item_s *item);

static void blist_iter_init(blist *list, blist_iter *iter);

static inline _Bool stock_equals(void *first, void *second) {
	return first == second;
}
static inline struct blist_item_s *blist_get_item(blist *list, int index) {
	return (index >= list->size / 2) ? blist_walk_to_backwards(list, index) :
		blist_walk_to_forwards(list, index);
}

struct blist_item_s *blist_walk_to_forwards(blist *list, int to)
{
	if (!list || to < 0 || to >= list->size)
		return NULL;

	struct blist_item_s *item = list->first;
	for (int i = 0; item; i++) {
		if (i == to)
			return item;
		item = item->next;
	}
	return NULL;
}

struct blist_item_s *blist_walk_to_backwards(blist *list, int to)
{
	if (!list || to < 0 || to >= list->size)
		return NULL;

	struct blist_item_s *item = list->last;
	for (int i = list->size - 1; item; i--) {
		if (i == to)
			return item;
		item = item->prev;
	}
	return NULL;
}

blist *blist_create(void)
{
	return calloc(1, sizeof(blist));
}

int blist_len(blist *list)
{
	return list ? list->size : 0;
}

int blist_index(blist *list, void *to_get, _Bool (*eq)(void*, void*))
{
	if (!list)
		return -1;

	struct blist_item_s *item = list->first;

	eq = eq ? eq : stock_equals;
	for (int i = 0; item; i++) {
		if (eq(to_get, item->item))
			return i;
		item = item->next;
	}
	return -1;
}

void *blist_get(blist *list, int index)
{
	if (!list)
		return NULL;

	struct blist_item_s *item;

	item = blist_get_item(list, index);
	return item ? item->item : NULL;
}

void blist_append(blist *list, void *item)
{
	if (!list)
		return;
	
	struct blist_item_s *new;
	struct blist_item_s *last = list->last;
	
	new = calloc(1, sizeof(struct blist_item_s));
	if (!new)
		return;
	new->item = item;

	if (last) {
		new->prev = last;
		last->next = new;
		list->last = new;
	} else {
		list->first = list->last = new;
	}
	list->size++;
}

void blist_insert(blist *list, void *item, int index)
{
	if (!list)
		return;

	struct blist_item_s *prev_insert;
	struct blist_item_s *insert;

	insert = calloc(1, sizeof(struct blist_item_s));
	if (!insert)
		return;
	insert->item = item;

	prev_insert = blist_get_item(list, index);
	blist_insert_item(list, prev_insert, insert);
}

void blist_insert_item(blist *list, struct blist_item_s *prev_insert,
		struct blist_item_s *insert)
{
	if (prev_insert) {
		insert->next = prev_insert;
		insert->prev = prev_insert->prev;
		if (prev_insert->prev)
			prev_insert->prev->next = insert;
		else
			list->first = insert;
		prev_insert->prev = insert;
		list->size++;
	} else if (list->size == 0) {
		list->first = list->last = insert;
		list->size++;
	}
}

void *blist_remove(blist *list, int index)
{
	if (!list)
		return NULL;

	struct blist_item_s *item;

	item = blist_get_item(list, index);
	return blist_remove_item(list, item);
}

void *blist_remove_item(blist *list, struct blist_item_s *item)
{
	void *to_ret = NULL;
	if (item) {
		to_ret = item->item;
		if (item->prev)
			item->prev->next = item->next;
		else
			list->first = item->next;

		if (item->next)
			item->next->prev = item->prev;
		else
			list->last = item->prev;

		free(item);
		list->size--;
	}
	return to_ret;
}

void blist_clear(blist *list)
{
	for (int i = 0; i < list->size; i++)
		blist_pop(list);
}

void blist_destroy(blist **list)
{
	if (!list || !(*list))
		return;

	blist_clear(*list);
	free(*list);
	list = NULL;
}

blist_iter *blist_iter_create(blist *list)
{
	if (!list)
		return NULL;

	blist_iter *iter = malloc(sizeof(blist_iter));

	if (iter) {
		blist_iter_init(list, iter);
	}
	return iter;
}

void blist_iter_init(blist *list, blist_iter *iter) {
	iter->index = 0;
	iter->list = list;
	iter->current = list->first;
	// iter->prev = NULL;
	// iter->next = iter->current ? iter->current->next : NULL;
}

int blist_iter_index(blist_iter *iter)
{
	return iter ? iter->index : -1;
}

void *blist_iter_next(blist_iter *iter)
{
	void *to_ret = NULL;

	if (!iter)
		return NULL;

	if (iter->current) {
		to_ret = iter->current->item;
		iter->current = iter->current->next;
		iter->index++;
	} else if (iter->index <= -1) {
		iter->current = iter->list->first;
		iter->index = 0;
	}

	return to_ret;
}

void *blist_iter_prev(blist_iter *iter)
{
	void *to_ret = NULL;

	if (!iter)
		return NULL;

	if (iter->current) {
		to_ret = iter->current->item;
		iter->current = iter->current->prev;
		iter->index--;
	} else if (iter->index >= iter->list->size) {
		iter->current = iter->list->last;
		iter->index = iter->list->size - 1;
	}

	return to_ret;
}

_Bool blist_iter_has_next(blist_iter *iter)
{
	if (iter && iter->current)
		return iter->current->next != NULL;
	return 0;
}

_Bool blist_iter_has_prev(blist_iter *iter)
{
	if (iter && iter->current)
		return iter->current->prev != NULL;
	return 0;
}

_Bool blist_iter_is_done(blist_iter *iter)
{
	return iter->current == NULL;
}

void *blist_iter_get(blist_iter *iter)
{
	if (iter && iter->current)
		return iter->current->item;
	return NULL;
}

void blist_iter_insert(blist_iter *iter, void *item, _Bool after)
{
	if (!iter)
		return;

	struct blist_item_s *insert;

	insert = calloc(1, sizeof(struct blist_item_s));
	if (!insert)
		return;
	insert->item = item;

	if (after)
		blist_insert_item(iter->list, iter->current->next, insert);
	else {
		blist_insert_item(iter->list, iter->current, insert);
		iter->index++;
	}

	// iter->next = iter->current->next;
	// iter->prev = iter->current->prev;
}

void *blist_iter_remove(blist_iter *iter, _Bool go_next)
{
	if (!iter || !iter->current)
		return NULL;

	struct blist_item_s *to_go = NULL;
	void *to_ret = NULL;

	if (go_next)
		to_go = iter->current->next ? iter->current->next : iter->current->prev;
	else
		to_go = iter->current->prev ? iter->current->prev : iter->current->next;

	to_ret = blist_remove_item(iter->list, iter->current);
	iter->current = to_go;

	return to_ret;

}

void blist_iter_jump_first(blist_iter *iter)
{
	if (!iter)
		return;
	blist_iter_init(iter->list, iter);
}

void blist_iter_jump_last(blist_iter *iter)
{
	if (!iter)
		return;
	iter->index = iter->list->size == 0 ? 0 : iter->list->size - 1;
	iter->current = iter->list->last;
}

void blist_iter_destroy(blist_iter **iter_p)
{
	if (iter_p && *iter_p) {
		free(*iter_p);
		*iter_p = NULL;
	}
}

