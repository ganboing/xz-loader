#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <xz.h>
#include "common.h"

uint8_t buf[PAYLOAD_SIZE];

int main()
{
	uint8_t *start = payload_start, *end = payload_end;

	xz_crc32_init();
	decompress(start, end - start, buf, PAYLOAD_SIZE);
	return 0;
}
