// Created by John Åkerblom 2013

#include "checksum.h"

#include <stdio.h>
#include <stdlib.h>

#define OFFSET_CHECKSUM_START	0x0C
#define OFFSET_CHECKSUM_END		OFFSET_CHECKSUM_START + 0x04

long checksum_fix(FILE *f)
{
    unsigned char data      = 0;
    unsigned int i			= 0;
	unsigned int overflow	= 0;
    long  checksum          = 0;
    size_t size             = 0;

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);

	for (i = 0; i < size; ++i) {
		checksum <<= 1;

		fread(&data, sizeof(data), 1, f);
		
		// Don't add value of existing checksum
		if (OFFSET_CHECKSUM_START <= i && OFFSET_CHECKSUM_END > i) {
			data = 0;	
		}

		checksum += data + overflow;

        overflow = (checksum < 0);
	}

	rewind(f);

	fseek(f, OFFSET_CHECKSUM_START, SEEK_CUR);

	fwrite(&checksum, sizeof(checksum), 1, f);

	return checksum;
}
