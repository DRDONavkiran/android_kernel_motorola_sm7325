#include "lime.h"

ssize_t write_vaddr_disk(void *, size_t);

static struct file * f = NULL;

static int dio_write_test(char *path, int oflags)
{
    int ok;

    f = filp_open(path, oflags | O_DIRECT | O_SYNC, 0444);
    if (f && !IS_ERR(f)) {
        ok = write_vaddr_disk("DIO", 3) == 3;
        filp_close(f, NULL);
    } else {
        ok = 0;
    }

    return ok;
}

int setup_disk(char *path, int dio) {
    int oflags = O_WRONLY | O_CREAT | O_LARGEFILE | O_TRUNC;
    int err = 0;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    mm_segment_t fs;

    fs = get_fs();
    set_fs(KERNEL_DS);
#endif

    if (dio && dio_write_test(path, oflags)) {
        oflags |= O_DIRECT | O_SYNC;
    } else {
        DBG("Direct IO Disabled");
    }

    f = filp_open(path, oflags, 0444);

    if (!f || IS_ERR(f)) {
        DBG("Error opening file %ld", PTR_ERR(f));

        err = (f) ? PTR_ERR(f) : -EIO;
        f = NULL;
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    set_fs(fs);
#endif

    return err;
}

void cleanup_disk(void) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    mm_segment_t fs;

    fs = get_fs();
    set_fs(KERNEL_DS);
#endif

    if(f) filp_close(f, NULL);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    set_fs(fs);
#endif
}

ssize_t write_vaddr_disk(void * v, size_t is) {
    ssize_t s;
    loff_t pos;

    pos = f->f_pos;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    mm_segment_t fs;

    fs = get_fs();
    set_fs(KERNEL_DS);
    s = vfs_write(f, v, is, &pos);
    set_fs(fs);
#else
    s = kernel_write(f, v, is, &pos);
#endif

    if (s == is) {
        f->f_pos = pos;
    }

    return s;
}
