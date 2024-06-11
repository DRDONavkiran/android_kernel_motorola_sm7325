#ifdef CONFIG_ZLIB_DEFLATE
#include <linux/zlib.h>

#include "lime.h"

/* Balance high compression level and memory footprint. */
#define DEFLATE_WBITS       11  /* 8KB */
#define DEFLATE_MEMLEVEL    5   /* 12KB */

static struct z_stream_s zstream;

static void *next_out;
static size_t avail_out;

extern int deflate_begin_stream(void *out, size_t outlen)
{
    int size;

    size = zlib_deflate_workspacesize(DEFLATE_WBITS, DEFLATE_MEMLEVEL);
    zstream.workspace = kzalloc(size, GFP_NOIO);
    if (!zstream.workspace) {
        return -ENOMEM;
    }

    if (zlib_deflateInit2(&zstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                          DEFLATE_WBITS,
                          DEFLATE_MEMLEVEL,
                          Z_DEFAULT_STRATEGY) != Z_OK) {
        kfree(zstream.workspace);
        return -EINVAL;
    }

    next_out = out;
    avail_out = outlen;

    zstream.next_out = next_out;
    zstream.avail_out = avail_out;

    return 0;
}

int deflate_end_stream(void)
{
    zlib_deflateEnd(&zstream);
    kfree(zstream.workspace);
    return 0;
}

ssize_t deflate(const void *in, size_t inlen)
{
    int flush, ret;

    if (in && inlen > 0)
        flush = Z_NO_FLUSH;
    else
        flush = Z_FINISH;

    if (zstream.avail_out != 0) {
        zstream.next_in = in;
        zstream.avail_in = inlen;
    }

    zstream.next_out = next_out;
    zstream.avail_out = avail_out;

    ret = zlib_deflate(&zstream, flush);

    if (ret != Z_OK && !(flush == Z_FINISH && ret == Z_STREAM_END)) {
        DBG("Deflate error: %d", ret);
        return -EIO;
    }

    return avail_out - zstream.avail_out;
}

#endif

MODULE_LICENSE("GPL");
