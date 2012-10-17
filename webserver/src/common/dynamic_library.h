#ifndef WS_DYNAMIC_LIBRARY_H
#define WS_DYNAMIC_LIBRARY_H


typedef void *dyn_lib_t;


dyn_lib_t dyn_lib_open(char const *file_name);
void dyn_lib_close(dyn_lib_t lib);
void *dyn_lib_find(dyn_lib_t lib, char const *symbol);


#endif
