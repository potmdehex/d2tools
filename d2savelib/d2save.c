// Created by John Åkerblom 2013

#include "d2save.h"
#include "checksum.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
	#pragma warning(disable:4996) // M$ fopen strncpy unsafe
#endif

#define MAX_NAME			15

#define OFFSET_CHARNAME		0x14
#define OFFSET_CHARSTATUS	0x24
#define STATUSBIT_HARDCORE	0x04
#define STATUSBIT_DIED		0x08
#define STATUSBIT_EXPANSION	0x32

struct _d2save
{
	char *filename;
	size_t size;
    unsigned char *buf;
};

int d2save_open(d2save_t *pd2s, const char *filename)
{
	struct _d2save *p	= calloc(1, sizeof(*p));
	FILE *f				= NULL;

	p->filename = _strdup(filename);

	if (NULL == (f = fopen(filename, "rb+"))) {
		return 1;
	}

	fseek(f, 0, SEEK_END);
	p->size = ftell(f);
	rewind(f);

    // Basic error check to see if save is at least big enough to be valid
    if (p->size < OFFSET_CHARSTATUS + 1) {
        fclose(f);
        *pd2s = NULL;
        return 1;
    }

	p->buf = calloc(p->size, 1);
	fread(p->buf, 1, p->size, f);

	fclose(f);

	*pd2s = p;

	return 0;
}

int d2save_write(d2save_t d2s)
{
	struct _d2save *p	= (struct _d2save *)d2s;
	FILE *f				= NULL;

	if (NULL == (f = fopen(p->filename, "wb+"))) {
		return 1;
	}

	fwrite(p->buf, 1, p->size, f);

	checksum_fix(f);

	fclose(f);

	return 0;
}

int d2save_free(d2save_t *pd2s)
{
	struct _d2save *p = *(struct _d2save **)pd2s;

	if (NULL == p) {
		return 1;
	}

	free(p->buf);
	free(p->filename);
	free(p);

	*pd2s = NULL;

	return 0;
}

static int _set_status(d2save_t d2s, int status_bit, int val)
{
	struct _d2save *p		= (struct _d2save *)d2s;
	unsigned char *status	= &p->buf[OFFSET_CHARSTATUS];
	
	// set the status bit if val is 1, otherwise unset it
	*status = (0 != val)? (*status | status_bit) : (*status & ~status_bit);

	return 0;
}

static int _get_status(d2save_t d2s, int status_bit, int *val)
{
	struct _d2save *p		= (struct _d2save *)d2s;
	unsigned char *status	= &p->buf[OFFSET_CHARSTATUS];

	*val = (*status & status_bit)? 1 : 0;

	return 0;
}

int d2save_set_died(d2save_t d2s, int val)
{
	return _set_status(d2s, STATUSBIT_DIED, val);
}

int d2save_set_hardcore(d2save_t d2s, int val)
{
	return _set_status(d2s, STATUSBIT_HARDCORE, val);
}

int d2save_set_expansion(d2save_t d2s, int val)
{
	return _set_status(d2s, STATUSBIT_EXPANSION, val);
}

int d2save_get_died(d2save_t d2s, int *val)
{
	return _get_status(d2s, STATUSBIT_DIED, val);
}

int d2save_get_hardcore(d2save_t d2s, int *val)
{
	return _get_status(d2s, STATUSBIT_HARDCORE, val);
}

int d2save_get_expansion(d2save_t d2s, int *val)
{
	return _get_status(d2s, STATUSBIT_EXPANSION, val);
}

int d2save_get_name(d2save_t d2s, char *name, int size)
{
	struct _d2save *p		= (struct _d2save *)d2s;
	char *n					= &p->buf[OFFSET_CHARNAME];

	memset(name, 0, size);
	strncpy(name, n, (size > MAX_NAME)? MAX_NAME : size);

	return 0;
}
