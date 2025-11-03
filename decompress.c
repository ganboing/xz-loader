#include <stdio.h>
#include <string.h>
#include <xz.h>
#include "common.h"

void decompress(const uint8_t *in, size_t in_size, uint8_t *out, size_t out_size)
{
	struct xz_buf b;
	struct xz_dec *s;
	enum xz_ret ret;

	printf("Decompressing from %p,+%lx to %p,+%lx\n",
		in, (unsigned long)in_size, out, (unsigned long)out_size);
	s = xz_dec_init(XZ_SINGLE, 0);
	if (s == NULL)
		panic("Initialization failed\n");

	b.in = in;
	b.in_pos = 0;
	b.in_size = in_size;

	b.out = out;
	b.out_pos = 0;
	b.out_size = out_size;

	ret = xz_dec_run(s, &b);

	if (ret != XZ_STREAM_END)
		printf("Decompress failed, ret=%d\n", ret);

	xz_dec_end(s);
	return;
}

