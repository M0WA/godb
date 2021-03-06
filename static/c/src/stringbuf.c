#include "stringbuf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

int stringbuf_resize(struct _StringBuf *buf,size_t newsize) {
	newsize++; //include terminating zero

	if(newsize < buf->size) {
		return 0; }

	char *oldbuf = buf->buf;
	size_t oldsize = buf->size;

	buf->size = newsize/buf->incsize;
	buf->size++;
	buf->size *= buf->incsize;

	if(buf->buf) {
		buf->buf = realloc(buf->buf,buf->size);
	} else {
		buf->buf = malloc(buf->size);
		buf->buf[0] = 0;
	}
	if(!buf->buf) {
		buf->buf = oldbuf;
		buf->size = oldsize;
		return 1;
	}
	return 0;
}

const char *stringbuf_get(const struct _StringBuf *buf) {
	return buf->buf;
}

char *stringbuf_buf(struct _StringBuf *buf) {
	return buf->buf;
}

size_t stringbuf_strlen(const struct _StringBuf *buf) {
	return (buf->buf ? strlen(buf->buf) : 0);
}

int stringbuf_append(struct _StringBuf *buf,const char *str) {
	size_t newlen = stringbuf_strlen(buf) + strlen(str);
	if(stringbuf_resize(buf,newlen)) {
		return 1; }
	strncat(buf->buf,str,newlen + 1);
	return 0;
}

int stringbuf_appendf(struct _StringBuf *buf,const char *fmt,...) {
	int size = 0;
    va_list ap;

    // determine space
    va_start(ap, fmt);
    size = vsnprintf(0, size, fmt, ap);
    va_end(ap);
    if (size < 0)
        return 1;

    // allocate enough space
    size_t newlen = stringbuf_strlen(buf) + size + 1;
	if(stringbuf_resize(buf,newlen)) {
		return 1; }

	char *cbuf = stringbuf_buf(buf);
	cbuf = &(cbuf[stringbuf_strlen(buf)]);

	// actually do vsnprintf
    va_start(ap, fmt);
	vsprintf(cbuf,fmt,ap);
	va_end(ap);
	return 0;
}

void stringbuf_init(struct _StringBuf *buf,size_t incsize){
	memset(buf,0,sizeof(struct _StringBuf));
	buf->incsize = incsize;
}

void stringbuf_destroy(struct _StringBuf *buf) {
	if(buf->buf) {
		free(buf->buf);	}
	memset(buf,0,sizeof(struct _StringBuf));
}
