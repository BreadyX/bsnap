#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

#define DICT_SIZE 1024

struct bitem_s {
	void *item;
	char *key;
	struct bitem *next;
};

struct blist_s {
	struct bitem_s *first;
	struct bitem_s *last;
};

struct bdict_s {
	struct blist_s *conflicts;
};

typedef bitem *bitem_s;
typedef blist *blist_s;
typedef bdict *bdict_s;

int hash_key(char *key);

// List/Queue/Stack
blist blist_create();
blist blist_append(blist list, void *item);
blist blist_push(blist list, void *item);
blist blist_pop(blist list, void *item);
int   blist_index(blist list, void *item);
bitem blist_get(blist list, int index);
blist blist_remove(blist list, int index);
void  blist_destroy(blist *list);

#endif
