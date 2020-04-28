#include <stdlib.h>
#include <errno.h>

#include "structures/blist.h"
#include "utils.h"

struct blist_item_s {
	void *item;
	struct blist_item_s *next;
	struct blist_item_s *prev;
};

struct blist_iter_s {
	blist *list;

	int index;
	struct blist_item_s *current;
};

struct blist_s {
	unsigned char flags;
	int size;

	struct blist_item_s *first;
	struct blist_item_s *last;

	delete_obj deleter;
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

static inline struct blist_item_s *blist_get_item(blist *list, int index) {
	return (index >= list->size / 2) ? blist_walk_to_backwards(list, index) :
		blist_walk_to_forwards(list, index);
}

/*
 * blist
 */
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

blist *blist_create(unsigned char flags, delete_obj deleter)
{
	blist *to_ret = calloc(1, sizeof(blist));
	if (!to_ret) {
		to_ret->flags = flags;
		if (flags & BLIST_DELETE_OBJS)
			to_ret->deleter = deleter ? deleter : default_delete_obj;
		else
			to_ret->deleter = NULL;
	}
	return to_ret;
}

int blist_len(blist *list)
{
	return list ? list->size : 0;
}

int blist_index(blist *list, void *to_get, equals eq)
{
	check_param_ptr(list, EINVAL, -1);

	struct blist_item_s *item = list->first;

	eq = eq ? eq : default_equals;
	for (int i = 0; item; i++) {
		if (eq(to_get, item->item))
			return i;
		item = item->next;
	}
	errno = EINVAL;
	return -1;
}

void *blist_get(blist *list, int index)
{
	check_param_ptr(list, EINVAL, NULL);

	struct blist_item_s *item;

	item = blist_get_item(list, index);
	if (!item) {
		errno = EINVAL;
		return NULL;
	}
	return item->item;
}

void blist_append(blist *list, void *item)
{
	check_param_ptr(list, EINVAL, );

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
	check_param_ptr(list, EINVAL, );

	struct blist_item_s *prev_insert;
	struct blist_item_s *insert;

	insert = calloc(1, sizeof(struct blist_item_s));
	if (!insert)
		return;
	insert->item = item;

	prev_insert = blist_get_item(list, index);
	if (prev_insert)
		blist_insert_item(list, prev_insert, insert);
	else {
		errno = EINVAL;
		free(insert);
	}
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
	check_param_ptr(list, EINVAL, NULL);

	struct blist_item_s *item;
	void *val;

	item = blist_get_item(list, index);
	if (item) {
		val = blist_remove_item(list, item);
		if (list->flags & BLIST_DELETE_OBJS && list->deleter) {
			list->deleter(val);
			return NULL;
		} else
			return val;
	} else {
		errno = EINVAL;
		return NULL;
	}
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
	check_param_ptr(list, EINVAL, );

	struct blist_item_s *i = list->first;
	struct blist_item_s *i_next;

	while(i) {
		i_next = i->next;
		if (list->flags & BLIST_DELETE_OBJS && list->deleter)
			list->deleter(i->item);
		free(i);
		i = i_next;
	}
	list->size = 0;
	list->first = list->last = NULL;
}

void blist_destroy(blist *list)
{
	check_param_ptr(list, EINVAL, );

	blist_clear(list);
	free(list);
}

/*
 * Iter
 */
blist_iter *blist_iter_create(blist *list)
{
	check_param_ptr(list, EINVAL, NULL);

	blist_iter *iter = malloc(sizeof(blist_iter));

	if (iter)
		blist_iter_init(list, iter);
	return iter;
}

void blist_iter_init(blist *list, blist_iter *iter) {
	iter->index = 0;
	iter->list = list;
	iter->current = list->first;
}

int blist_iter_index(blist_iter *iter)
{
	if (iter)
		return iter->index;
	else {
		errno = EINVAL;
		return -1;
	}
}

void *blist_iter_next(blist_iter *iter)
{
	void *to_ret = NULL;

	check_param_ptr(iter, EINVAL, NULL);

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

	check_param_ptr(iter, EINVAL, NULL);

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
	check_param_ptr(iter, EINVAL, NULL);
	if (iter && iter->current)
		return iter->current->item;
	return NULL;
}

void blist_iter_insert(blist_iter *iter, void *item, _Bool after)
{
	check_param_ptr(iter, EINVAL, );

	struct blist_item_s *insert;

	insert = calloc(1, sizeof(struct blist_item_s));
	if (!insert) {
		errno = ENOMEM;
		return;
	}
	insert->item = item;

	if (after)
		blist_insert_item(iter->list, iter->current->next, insert);
	else {
		blist_insert_item(iter->list, iter->current, insert);
		iter->index++;
	}
}

void *blist_iter_remove(blist_iter *iter, _Bool go_next)
{
	check_param_ptr(iter, EINVAL, NULL);

	if (iter->current)
		return NULL;

	struct blist_item_s *to_go = NULL;
	void *to_ret = NULL;

	if (go_next)
		to_go = iter->current->next ? iter->current->next : iter->current->prev;
	else
		to_go = iter->current->prev ? iter->current->prev : iter->current->next;

	to_ret = blist_remove_item(iter->list, iter->current);
	iter->current = to_go;

	if (iter->list->flags & BLIST_DELETE_OBJS && iter->list->deleter) {
		iter->list->deleter(to_ret);
		return NULL;
	} else
		return to_ret;

}

void blist_iter_jump_first(blist_iter *iter)
{
	check_param_ptr(iter, EINVAL, );
	blist_iter_init(iter->list, iter);
}

void blist_iter_jump_last(blist_iter *iter)
{
	check_param_ptr(iter, EINVAL, );
	iter->index = iter->list->size == 0 ? 0 : iter->list->size - 1;
	iter->current = iter->list->last;
}

void blist_iter_destroy(blist_iter *iter)
{
	check_param_ptr(iter, EINVAL, );
	free(iter);
}

