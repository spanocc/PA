#include <fs.h>

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [3]         = {"/dev/events", 0, 0, events_read, invalid_write},
  [4]         = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [5]         = {"/dev/fb", 0, 0, invalid_read, fb_write},
#include "files.h"
};

#define FILE_NUM (int)(sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  int w = io_read(AM_GPU_CONFIG).width;  
  int h = io_read(AM_GPU_CONFIG).height; 
  file_table[5].size = w * h;
}

int fs_open(const char *pathname, int flags, int mode) {
  for(int i = 0; i < FILE_NUM; ++i) {
    if(!strcmp(file_table[i].name, pathname)) {
      file_table[i].open_offset = file_table[i].disk_offset;
      return i;
    }
  }
  assert(0);
}

size_t fs_read(int fd, void *buf, size_t len) {
  //assert(fd != FD_STDIN && fd != FD_STDOUT && fd != FD_STDERR);
  size_t tlen = (file_table[fd].open_offset + len <= file_table[fd].disk_offset + file_table[fd].size) ? len : (file_table[fd].disk_offset + file_table[fd].size - file_table[fd].open_offset);
  //tlen表示实际读入的字节数
  size_t ret = 0;
  if(file_table[fd].read == NULL) ret = ramdisk_read(buf, file_table[fd].open_offset, tlen); //普通文件
  else ret = file_table[fd].read(buf, file_table[fd].open_offset, len); //使用原长度

  file_table[fd].open_offset += tlen;
  return ret;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  //assert(fd != FD_STDIN);
  size_t tlen = (file_table[fd].open_offset + len <= file_table[fd].disk_offset + file_table[fd].size) ? len : (file_table[fd].disk_offset + file_table[fd].size - file_table[fd].open_offset);
  //tlen表示实际读入的字节数
  size_t ret = 0;
  if(file_table[fd].write == NULL) ret = ramdisk_write(buf, file_table[fd].open_offset, tlen);
  else ret = file_table[fd].write(buf, file_table[fd].open_offset, len);

  file_table[fd].open_offset += tlen;
  return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  size_t intr = offset;
  switch(whence) {
    case SEEK_SET:
      intr += file_table[fd].disk_offset;
      break;
    case SEEK_CUR:
      intr += file_table[fd].open_offset;
      break;
    case SEEK_END:
      intr += (file_table[fd].disk_offset + file_table[fd].size);
      break;
    default: assert(0);
  }
  if(intr > file_table[fd].disk_offset + file_table[fd].size) intr = file_table[fd].disk_offset + file_table[fd].size;
  file_table[fd].open_offset = intr;
  return intr - file_table[fd].disk_offset;
}

int fs_close(int fd) {
  return 0;
}