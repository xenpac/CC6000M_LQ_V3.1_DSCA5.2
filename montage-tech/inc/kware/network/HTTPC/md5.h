#ifndef __MD5_H__
#define __MD5_H__

//#include <OSAL.h>

/* Structure to save state of computation between the single steps */
typedef struct _tagMD5Context {

    unsigned int A;
    unsigned int  B;
    unsigned int  C;
    unsigned int  D;

    unsigned int  total[2];
    unsigned int  buflen;
    char buffer[128];

} MD5Context;

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize structure containing state of computation.
   (RFC 1321, 3.3: Step 3)  */
void md5_init_ctx (MD5Context* ctx);

/* Starting with the result of former calls of this function (or the
   initialization function update the context for the next LEN bytes
   starting at BUFFER.
   It is necessary that LEN is a multiple of 64!!! */
void md5_process_block (const void *buffer, size_t len, MD5Context* ctx);

/* Starting with the result of former calls of this function (or the
   initialization function update the context for the next LEN bytes
   starting at BUFFER.
   It is NOT required that LEN is a multiple of 64.  */
void md5_process_bytes (const void *buffer, size_t len, MD5Context* ctx);

/* Process the remaining bytes in the buffer and put result from CTX
   in first 16 bytes following RESBUF.  The result is always in little
   endian byte order, so that a byte-wise output yields to the wanted
   ASCII representation of the message digest.

   IMPORTANT: On some systems it is required that RESBUF be correctly
   aligned for a 32 bits value.  */
void *md5_finish_ctx (void *resbuf, MD5Context* ctx);

/* Put result from CTX in first 16 bytes following RESBUF.  The result is
   always in little endian byte order, so that a byte-wise output yields
   to the wanted ASCII representation of the message digest.

   IMPORTANT: On some systems it is required that RESBUF is correctly
   aligned for a 32 bits value.  */
void *md5_read_ctx (void *resbuf, MD5Context* ctx);

/* Compute MD5 message digest for LEN bytes beginning at BUFFER.  The
   result is always in little endian byte order, so that a byte-wise
   output yields to the wanted ASCII representation of the message
   digest.  */
void *md5_buffer (const char *buffer, size_t len, void *resblock);

#ifdef __cplusplus
}
#endif

#endif
