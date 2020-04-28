#include <stdio.h>
#include <stdlib.h>

#include "structures/blist.h"
#include "structures/blist.c"       // i am not crazy
#include "structures/global_defs.c" // i am not crazy

// blist_create  OK
// blsit_len     OK
// blist_append  OK
// blist_insert  OK
// blist_index   OK
// blist_get     OK
// blist_remove  OK
// blist_clear   OK
// blist_destroy OK

static inline _Bool test_item(void *first, void *second)
{
	return *((int *)first) == *((int *)second);
}

void print_full_without_iter(char *pre, blist *list)
{
	struct blist_item_s *item;

	printf("%s", pre);
	item = list->first;
	for (int i = 0; i < list->size; i++) {
		printf("%d ", *((int *)item->item));
		item = item->next;
	}
	printf("\n");
}

void test_insertion(blist *list) {
	int *to_add;
	struct blist_item_s *item;

	// APPEND
	for (int i = 0; i < 10; i++) {
		to_add = malloc(sizeof(int));
		*to_add = i;
		blist_append(list, to_add);
	}
	item = list->first;

	for (int i = 0; i < 10; i++) {
		if (*((int*) item->item) != i)
			exit(1);
		item = item->next;
	}
	print_full_without_iter("Append: ", list);

	// PUSH
	to_add = malloc(sizeof(int));
	*to_add = -1;
	blist_push(list, to_add);
	item = list->first;
	if (*((int *)item->item) != -1)
		exit(1);
	print_full_without_iter("Push: ", list);

	// GENERIC INSERT
	to_add = malloc(sizeof(int));
	*to_add = 4;
	blist_insert(list, to_add, 5);
	item = list->first;
	for (int i = 0; i < 6; i++)
		item = item->next;
	if (*((int *)item->item) != 4)
		exit(1);
	print_full_without_iter("Generic insert: ", list);

	// INVALID INSERT
	blist_insert(list, NULL, 69);
}

void test_deletion(blist *list)
{
	int *removed;
	struct blist_item_s *item;

	// POP
	removed = blist_pop(list);
	free(removed);
	item = list->first;
	if (*((int *)item->item) != 0)
		exit(1);
	print_full_without_iter("Pop: ", list);

	// DROP
	removed = blist_drop(list);
	free(removed);
	item = list->last;
	if (*((int *)item->item) != 8)
		exit(1);
	print_full_without_iter("Drop: ", list);

	// GENERIC REMOVE
	removed = blist_remove(list, 4);
	free(removed);
	item = list->first;
	for (int i = 0 ; i < 4; i++)
		item = item->next;
	if (*((int *)item->item) != 4)
		exit(1);
	print_full_without_iter("Generic remove: ", list);

	// INVALID REMOVE
	removed = blist_remove(list, 69);
}

void test_get(blist *list)
{
	int *got;

	// GET
	got = blist_get(list, 4);
	if (*got != 4)
		exit(1);
	print_full_without_iter("Get: ", list);

	// PEEK
	got = blist_peek(list);
	if (*got != 0)
		exit(1);
	print_full_without_iter("Peek: ", list);

	// PEEK_LAST
	got = blist_peek_last(list);
	if (*got != 8)
		exit(1);
	print_full_without_iter("Peek last: ", list);

	// INDEX
	got = malloc(sizeof(int));
	*got = 5;
	if (blist_index(list, got, test_item) != 5)
		exit(1);
	free(got);
	print_full_without_iter("Index: ", list);

	// INVALID GET
	got = blist_get(list, 69);
	if (got)
		exit(1);

	// INVALID INDEX
	if (blist_index(list, NULL, NULL) != -1)
		exit(1);
}

int main(int argc, char **argv)
{
	blist *list = blist_create(0, NULL);
	if (!list)
		return 1;

	test_insertion(list); // -1 0 1 2 3 4 4 5 6 7 8 9
	test_deletion(list);  // 0 1 2 3 4 5 6 7 8
	test_get(list);       // 0 1 2 3 4 5 6 7 8

	blist_clear(list);
	blist_destroy(list);
	return 0;
}


