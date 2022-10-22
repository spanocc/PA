#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
//数字转为字符串
//返回字符串长度
int dig2str(char *str, long long dig) {  
    assert(str != NULL);

    char  buf[32];
    int is_neg = 0;
    char *p = buf;
 
   	//if(dig == -2147483648)  strcpy(buf, "8463847412-"); //对INT_MIN特判，因为int范围内没有214783648
    //else {
    	if(dig < 0) {
    		is_neg = 1;
    		dig = -dig;
    	}
        while(dig / 10) {
            *p++ = (dig % 10) + '0';
            dig /= 10;
        }
        *p++ = (dig % 10) + '0';
        if(is_neg) *p++ = '-';
        *p = '\0';
    //}
   
    
    int len = strlen(buf);
    assert(len > 0);
    while(len--) {
        *str++ = buf[len];
    }
    return strlen(buf);
}

int dig2str16(char*str, uintptr_t dig) { //uintptr_t 有可移植性 在64(32)位机器上是 64(32)位
    char c[32] = "0123456789abcdef";
    char  buf[32];
    char *p = buf;
    while(dig / 16) {
        *p++ = c[(dig % 16)];
        dig /= 16;
    }
    *p++ = c[(dig % 16)];
    //*p++ = 'x'; *p++ = '0';
    *p = '\0';
    int len = strlen(buf);
    assert(len > 0);
    while(len--) {
        *str++ = buf[len];
    }        // printf("\n%s\n",buf);
    return strlen(buf);
}


//可以通过申请一个缓冲区，把要打印的字符先放一部分到缓冲区里，然后打印，再处理后面的部分。
int printf(const char *fmt, ...) {
    assert(fmt != NULL);
    char buf[655360];
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = vsprintf(buf, fmt, ap);
    va_end(ap);

    assert(ret >= 65536);

    buf[ret] = '\0';
    char *p = buf;
    while(*p != '\0') putch(*p++);

    return ret;
}



//返回值为写入的字符数
int vsprintf(char *out, const char *fmt, va_list ap) {
    int ret = 0;
    int flag = 0;
    int arg_int;
    int dig_len;
    uintptr_t arg_ptr;
    const char *arg_str;
    while(*fmt != '\0') {
        if(flag == 0) {
            if(*fmt == '%') flag = 1;
            else {
                *out++ = *fmt;
                ret++;
            }
        }
        else {
            flag = 0;
            switch(*fmt) {
                case 'd':
                    arg_int = va_arg(ap, int);
                    dig_len = dig2str(out, arg_int);
                    out += dig_len;
                    ret += dig_len;
                    break;
                case 's':
                    arg_str = va_arg(ap, char *);
                    ret += strlen(arg_str);
                    while(*arg_str != '\0') {
                        *out++ = *arg_str++;
                    }//不加空字符
                    break;
                case 'x':
                    arg_int = va_arg(ap, int);
                    dig_len = dig2str16(out, arg_int);
                    out += dig_len;
                    ret += dig_len;
                    break;
                case 'p':
                    arg_ptr = (uintptr_t)va_arg(ap, char*);
                    dig_len = dig2str16(out, arg_ptr);
                    out += dig_len;
                    ret += dig_len;
                    break;

                default: break;
            }
        }
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

    out[ret] = '\0';   //printf("my sprintf");

    return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
