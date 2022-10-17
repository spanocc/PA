#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
    assert(s != NULL);
    int ret = 0;
    while(*s != '\0') {
        ret++;
        s++;
    }
    return ret;
}

char *strcpy(char *dst, const char *src) {
    assert(dst != NULL && src != NULL);
    char *p1 = dst;
    const char *p2 = src;
    while(*p2 != '\0') {
        *p1++ = *p2++;              
    }
    *p1 = '\0';
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
    assert(dst != NULL && src != NULL);  
    strcpy(dst + strlen(dst), src);
    return dst;
}

int strcmp(const char *s1, const char *s2) {   
    assert(s1 != NULL && s2 != NULL); 
    while(*s1 == *s2) {
        if(*s1 == '\0' && *s2 == '\0') return 0;
        if(*s1 == '\0') return -1;
        if(*s2 == '\0') return 1;
        s1++;
        s2++;
    }
    return (*s1 > *s2) ? 1 : -1;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
    assert(s != NULL);
    char *p = s;
    while(n--) {
        *p = c;
        p++;
    }
    return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
    assert(out != NULL && in != NULL);
    char *p1 = out;
    const char *p2 = in;
    while(n--) {
        *p1 = *p2;
        p1++;
        p2++;
    }
    return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    assert(s1 != NULL && s2 != NULL);
    assert((s1 + n < s2) || (s2 + n < s1)); //检测重叠
    const char *p1 = s1, *p2 = s2;
    while(n--) {
        if(*p1 != *p2) return *p1 - *p2;
        p1++;
        p2++;
    }
    return 0;
}

#endif
