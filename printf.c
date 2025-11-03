/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "common.h"

#define PAD_RIGHT 1
#define PAD_ZERO 2
#define PAD_ALTERNATE 4
#define PAD_SIGN 8

#define va_start(v, l) __builtin_va_start((v), l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
typedef __builtin_va_list va_list;

static void printc(char ch, int flags)
{
	putchar(ch);
}

static int prints(const char *string, int width, int flags)
{
	int pc = 0;
	width -= strlen(string);
	if (!(flags & PAD_RIGHT)) {
		for (; width > 0; --width) {
			printc(flags & PAD_ZERO ? '0' : ' ', flags);
			++pc;
		}
	}
	for (; *string; ++string) {
		printc(*string, flags);
		++pc;
	}
	for (; width > 0; --width) {
		printc(' ', flags);
		++pc;
	}

	return pc;
}

#define PRINT_BUF_LEN 64

static int printi(long long i, int width, int flags, int type)
{
	int pc = 0;
	char *s, sign = 0, letbase, print_buf[PRINT_BUF_LEN];
	unsigned long long u, b, t;

	b = 10;
	letbase = 'a';
	if (type == 'o')
		b = 8;
	else if (type == 'x' || type == 'X' || type == 'p' || type == 'P') {
		b = 16;
		letbase &= ~0x20;
		letbase |= type & 0x20;
	}

	u = i;
	sign = 0;
	if (type == 'i' || type == 'd') {
		if ((flags & PAD_SIGN) && i > 0)
			sign = '+';
		if (i < 0) {
			sign = '-';
			u = -i;
		}
	}

	s  = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	if (!u) {
		*--s = '0';
	} else {
		while (u) {
			t = u % b;
			u = u / b;
			if (t >= 10)
				t += letbase - '0' - 10;
			*--s = t + '0';
		}
	}

	if (flags & PAD_ZERO) {
		if (sign) {
			printc(sign, flags);
			++pc;
			--width;
		}
		if (i && (flags & PAD_ALTERNATE)) {
			if (b == 16 || b == 8) {
				printc('0', flags);
				++pc;
				--width;
			}
			if (b == 16) {
				printc('x' - 'a' + letbase, flags);
				++pc;
				--width;
			}
		}
	} else {
		if (i && (flags & PAD_ALTERNATE)) {
			if (b == 16)
				*--s = 'x' - 'a' + letbase;
			if (b == 16 || b == 8)
				*--s = '0';
		}
		if (sign)
			*--s = sign;
	}

	return pc + prints(s, width, flags);
}

static int print(const char *format, va_list args)
{
	bool flags_done;
	int width, flags, pc = 0;
	char type, scr[2];

	for (; *format != 0; ++format) {
		width = flags = 0;
		if (*format == '%') {
			++format;
			if (*format == '\0')
				break;
			if (*format == '%')
				goto literal;
			/* Get flags */
			flags_done = false;
			while (!flags_done) {
				switch (*format) {
				case '-':
					flags |= PAD_RIGHT;
					break;
				case '+':
					flags |= PAD_SIGN;
					break;
				case '#':
					flags |= PAD_ALTERNATE;
					break;
				case '0':
					flags |= PAD_ZERO;
					break;
				case ' ':
				case '\'':
					/* Ignored flags, do nothing */
					break;
				default:
					flags_done = true;
					break;
				}
				if (!flags_done)
					++format;
			}
			if (flags & PAD_RIGHT)
				flags &= ~PAD_ZERO;
			/* Get width */
			for (; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if (*format == 's') {
				char *s = va_arg(args, char *);
				pc += prints(s ? s : "(null)",
					     width, flags);
				continue;
			}
			if ((*format == 'd') || (*format == 'i')) {
				pc += printi(va_arg(args, int),
					     width, flags, *format);
				continue;
			}
			if ((*format == 'u') || (*format == 'o')
					 || (*format == 'x') || (*format == 'X')) {
				pc += printi(va_arg(args, unsigned int),
					     width, flags, *format);
				continue;
			}
			if ((*format == 'p') || (*format == 'P')) {
				pc += printi((uintptr_t)va_arg(args, void*),
					     width, flags, *format);
				continue;
			}
			if (*format == 'l') {
				type = 'i';
				if (format[1] == 'l') {
					++format;
					if ((format[1] == 'u') || (format[1] == 'o')
							|| (format[1] == 'd') || (format[1] == 'i')
							|| (format[1] == 'x') || (format[1] == 'X')) {
						++format;
						type = *format;
					}
					pc += printi(va_arg(args, long long),
						width, flags, type);
					continue;
				}
				if ((format[1] == 'u') || (format[1] == 'o')
						|| (format[1] == 'd') || (format[1] == 'i')
						|| (format[1] == 'x') || (format[1] == 'X')) {
					++format;
					type = *format;
				}
				if ((type == 'd') || (type == 'i'))
					pc += printi(va_arg(args, long),
					     width, flags, type);
				else
					pc += printi(va_arg(args, unsigned long),
					     width, flags, type);
				continue;
			}
			if (*format == 'c') {
				/* char are converted to int then pushed on the stack */
				scr[0] = va_arg(args, int);
				scr[1] = '\0';
				pc += prints(scr, width, flags);
				continue;
			}
		} else {
literal:
			printc(*format, flags);
			++pc;
		}
	}

	return pc;
}

int printf(const char *format, ...)
{
	va_list args;
	int retval;

	va_start(args, format);
	retval = print(format, args);
	va_end(args);

	return retval;
}

void panic(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	print(format, args);
	va_end(args);

	for (;;) {
#if defined(__i386) || defined(__x86_64)
		__asm__ __volatile__("pause");
#elif defined(__riscv)
		__asm__ __volatile__("wfi");
#endif
	}
}
