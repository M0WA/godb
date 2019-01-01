#pragma once

#include <stddef.h>

typedef struct _StringBuf {
	size_t incsize;
	size_t size;
	char *buf;
} StringBuf;

char *stringbuf_buf(struct _StringBuf *buf);
const char *stringbuf_get(const struct _StringBuf *buf);
size_t stringbuf_strlen(const struct _StringBuf *buf);
int stringbuf_resize(struct _StringBuf *buf,size_t newsize);
int stringbuf_append(struct _StringBuf *buf,const char *str);
int stringbuf_appendf(struct _StringBuf *buf,const char *fmt,...);
void stringbuf_init(struct _StringBuf *buf,size_t incsize);
void stringbuf_destroy(struct _StringBuf *buf);
