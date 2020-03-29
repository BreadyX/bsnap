#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <structures/list.c> // i am not crazy

// blist_iter_create     OK
// blist_iter_index      OK
// blist_iter_prev       OK
// blist_iter_next       OK
// blist_iter_has_prev   OK
// blist_iter_has_next   OK
// blist_iter_is_done    OK
// blist_iter_get        OK
// blist_iter_insert
// blist_iter_remove
// blist_iter_jump_first OK
// blist_iter_jump_last  OK
// blsit_iter_destroy    OK

void test_movement(blist_iter *iter)
{
	int *got;
	int *got2;

	if (blist_iter_index(iter) != 0)
		exit(1);

	for (int i = 0; !blist_iter_is_done(iter); i++) {
		got2 = blist_iter_get(iter);
		got = blist_iter_next(iter);
		if (*got != i || *got != *got2 || blist_iter_index(iter) - 1 != i)
			exit(1);
	}

	if (blist_iter_next(iter) || blist_iter_has_next(iter) ||
		!blist_iter_is_done(iter))
		exit(1);

	blist_iter_jump_first(iter);
	if (blist_iter_index(iter) != 0)
		exit(1);

	blist_iter_jump_last(iter);
	if (blist_iter_index(iter) != 9)
		exit(1);

	for(int i = 9; !blist_iter_is_done(iter); i--) {
		got = blist_iter_prev(iter);
		if (*got != i || blist_iter_index(iter) + 1 != i)
			exit(1);
	}

	if (blist_iter_prev(iter) || blist_iter_has_prev(iter) ||
		!blist_iter_is_done(iter))
		exit(1);

	blist_iter_jump_first(iter);
}

void test_mod(blist_iter *iter)
{
	int *m;

	m = malloc(sizeof(int));
	*m = 4;

	while (blist_iter_index(iter) != 4)
		blist_iter_next(iter);

	blist_iter_insert(iter, m, true);
	if (blist_iter_index(iter) != 4)
		exit(1);
	blist_iter_insert(iter, m, false);
	if (blist_iter_index(iter) != 5)
		exit(1);

	blist_iter_prev(iter);
	for (int i = 0; i < 3; i++) {
		m = blist_iter_next(iter);
		if (*m != 4)
			exit(1);
	}

	blist_iter_prev(iter);
	m = blist_iter_remove(iter, false);
	if (*m != 4 && blist_iter_index(iter) != 5)
		exit(1);

	m = blist_iter_remove(iter, true);
	if (*m != 4 && blist_iter_index(iter) != 5)
		exit(1);

}

int main(int argc, char **argv)
{
	int *m;
	blist *list = blist_create();

	for (int i = 0; i < 10; i++) {
		m = malloc(sizeof(int));
		*m = i;
		blist_append(list, m);
	}
	blist_iter *iter = blist_iter_create(list);

	test_movement(iter);  // 0 1 2 3 4 5 6 7 8 9 - iter: 0
	test_mod(iter);       // 0 1 2 3 4 4 4 5 6 7 8 9

	blist_iter_destroy(&iter);
	return 0;
}
