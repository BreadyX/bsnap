#ifndef __STRUCTURES_GLOBAL_DEF_H__
#define __STRUCTURES_GLOBAL_DEF_H__

typedef _Bool (*equals)(void*, void*);
typedef int   (*compare)(void*, void*);
typedef void  (*delete_obj)(void*);

_Bool default_equals(void *first, void *second);
int   default_compare(void *first, void *second);
void  default_delete_obj(void *obj);

#endif
