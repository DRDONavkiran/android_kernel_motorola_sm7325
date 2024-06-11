#ifndef __LIME_H_
#define __LIME_H_

#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/highmem.h>
#include <linux/pfn.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/err.h>
#include <linux/scatterlist.h>

#include <net/sock.h>
#include <net/tcp.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 6, 0)
#include <crypto/hash.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 11)
#include <linux/crypto.h>
#endif

#define LIME_RAMSTR "System RAM"
#define LIME_MAX_FILENAME_SIZE 256
#define LIME_MAGIC 0x4C694D45 //LiME

#define LIME_MODE_RAW 0
#define LIME_MODE_LIME 1
#define LIME_MODE_PADDED 2

#define LIME_METHOD_UNKNOWN 0
#define LIME_METHOD_TCP 1
#define LIME_METHOD_DISK 2

#define LIME_DIGEST_FAILED -1
#define LIME_DIGEST_COMPLETE 0
#define LIME_DIGEST_COMPUTE 1

#ifdef LIME_DEBUG
#define DBG(fmt, args...) do { printk("[LiME] "fmt"\n", ## args); } while (0)
#else
#define DBG(fmt, args...) do {} while(0)
#endif

#define RETRY_IF_INTERRUPTED(f) ({ \
    ssize_t err; \
    do { err = f; } while(err == -EAGAIN || err == -EINTR); \
    err; \
})

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
#define LIME_SUPPORTS_TIMING
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
#define LIME_USE_KMAP_ATOMIC
#endif

#ifdef CONFIG_ZLIB_DEFLATE
#define LIME_SUPPORTS_DEFLATE
#endif

//structures

typedef struct {
    unsigned int magic;
    unsigned int version;
    unsigned long long s_addr;
    unsigned long long e_addr;
    unsigned char reserved[8];
} __attribute__ ((__packed__)) lime_mem_range_header;



#endif //__LIME_H_
