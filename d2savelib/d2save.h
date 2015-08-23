// Created by John Åkerblom 2013

#ifndef __D2SAVE_H_DEF__
#define __D2SAVE_H_DEF__

typedef void *d2save_t;

int d2save_open(d2save_t *pd2s, const char *filename);
int d2save_write(d2save_t d2s);
int d2save_free(d2save_t *pd2s);

// val is either 1 or 0
int d2save_set_died(d2save_t d2s,		int val);
int d2save_set_hardcore(d2save_t d2s,	int val);
int d2save_set_expansion(d2save_t d2s,	int val);

int d2save_get_died(d2save_t d2s,		int *val);
int d2save_get_hardcore(d2save_t d2s,	int *val);
int d2save_get_expansion(d2save_t d2s,	int *val);

int d2save_get_name(d2save_t d2s, char *name, int size);

#endif
