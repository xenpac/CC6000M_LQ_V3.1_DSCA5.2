#ifndef __EXT_MEM_INTERFACE_H_
#define __EXT_MEM_INTERFACE_H_
#ifdef __cplusplus
extern "C" {
#endif

void mem_init_from_demux_mp(unsigned char * p_ext_buf_addr, unsigned int ext_buf_size);
void * calloc_from_demux_mp(unsigned int n, unsigned int size);
void *malloc_from_demux_mp(unsigned int size);
void * realloc_from_demux_mp(void * old_ptr, unsigned int size);
void free_from_demux_mp(void * ptr);
void mem_release_from_demux_mp();

#ifdef __cplusplus
}
#endif

#endif
