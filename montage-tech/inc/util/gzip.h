#ifndef _GZIP_H_
#define _GZIP_H_

int gzip_compress (u8 *p_dest, u32 *p_dest_len, 
                   const u8 *p_source, u32 source_len, 
                   u8 *p_cache, u32 cache_size);

int gzip_uncompress(u8 *p_dest, u32 *p_dest_len, 
                    const u8 *p_source, u32 source_len,
                    u8 *p_cache, u32 cache_size);

#endif

