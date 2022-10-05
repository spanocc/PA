#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
//数字转为字符串
static int dig2str(char *str, int dig) {
    assert(str != NULL);
    char  buf[32];
    char *p = buf;
    if(dig == 0) *p++ = 0;
    else {
        while(dig / 10) {
            *p++ = (dig % 10) + '0';
            dig /= 10;
        }
        *p++ = (dig % 10) + '0';
    }
    *p = '\0';
    int len = strlen(buf);
    assert(len > 0);
    while(len--) {
        *str++ = buf[len];
    }
    return len;
}



int printf(const char *fmt, ...) {
    assert(fmt != NULL);
  panic("Not implemented");
}
//返回值为写入的字符数
int vsprintf(char *out, const char *fmt, va_list ap) {
    int ret = 0;
    int flag = 0;
    int arg_int;
    const char *arg_str;
    while(*fmt != '\0') {
        if(flag == 0) {
            if(*fmt == '%') flag = 1;
            else *out = *fmt;
            ret++;
        }
        else {
            flag = 0;
            switch(*fmt) {
                case 'd':
                    arg_int = va_arg(ap, int);
                    ret += dig2str(out, arg_int);
                    break;
                case 's':
                    arg_str = va_arg(ap, char *);
                    while(*arg_str != '\0') {
                        *out = *arg_str;
                        out++;
                        arg_str++;
                    }//不加空字符
                    ret += strlen(arg_str);
                    break;
                default: break;
            }
        }
        out++;
        fmt++;
    }
    return ret;
}

int sprintf(char *out, const char *fmt, ...) {
    assert(out != NULL && fmt != NULL);
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = vsprintf(out, fmt, ap);
    va_end(ap);

    return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
