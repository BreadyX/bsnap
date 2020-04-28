#ifndef __UTILS_H__
#define __UTILS_H__

#define check_param_ptr(ptr, no, ret) do { if (!ptr) { errno = no; return ret; } } while(0)

#endif
