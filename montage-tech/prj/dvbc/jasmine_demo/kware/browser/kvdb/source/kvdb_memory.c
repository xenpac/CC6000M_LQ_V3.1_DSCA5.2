
/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
 #include <stdio.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_mutex.h"
#include "mtos_misc.h"

#include "kvdb_memory.h"
#define KDVB_DEBUG_PRINTF
#define KVDB_DPRINTF_DEBUG
#ifdef KDVB_DEBUG_PRINTF
#define kdevb_debug OS_PRINTF
#else
#define kdevb_debug(...) do{}while(0)

#endif
void *kvdb_porting_malloc(int size)
{
  void *pointer = NULL;

    pointer = (void *)mtos_malloc(size);
    MT_ASSERT(pointer != NULL);
    if (pointer)
    {
        memset(pointer,0,size);
    }
    else
    	kvdb_porting_dprintf("[ipanel porting malloc]malloc fail.\n");

    return pointer;
}

void kvdb_porting_free(void *ptr)
{
    if(ptr)
     mtos_free(ptr);
}

/******************8:program interface**************************************/
 typedef char    *ck_va_list;
 typedef unsigned int  CK_NATIVE_INT;
 typedef unsigned int  ck_size_t;
#define  CK_AUPBND         (sizeof (CK_NATIVE_INT) - 1)
#define CK_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
#define CK_VA_END(ap)         (void)0  /*ap = (ck_va_list)0 */
#define CK_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (CK_BND (A,CK_AUPBND))))
 extern int ck_vsnprintf(char *buf, ck_size_t size, const char *fmt, ck_va_list args);

int kvdb_porting_dprintf(const char *fmt, ...)
{
#ifdef KVDB_DPRINTF_DEBUG

     //ADS_DRV_PRINTF("#######desai debug ######\n");

    ck_va_list p_args = NULL;
    unsigned int  printed_len = 0;
    char    printk_buf[200];

    CK_VA_START(p_args, fmt);
    printed_len = ck_vsnprintf(printk_buf, sizeof(printk_buf), (char *)fmt, p_args);

    CK_VA_END(p_args);
    kdevb_debug(printk_buf);
    kdevb_debug("\n");
#endif
 //   LOG_PRINTF(fmt);
return SUCCESS;
    //int ret = 0;
	#if 0
	if (dprint_level>0)
	{
		va_list args;
		va_start(args,fmt);
		#if 0
		ret = vprintf(fmt,args);
        #else
		char loa_memory[400];
		vsprintf(loa_memory,fmt,args);
		libc_printf("%s\n",loa_memory);
		#endif
		va_end(args);
	}	
    return ret;
	#endif
}


static unsigned int start_stb_time = 0;/*取启动时刻的毫秒数*/

unsigned int kvdb_porting_time_ms()
{
   mtos_task_sleep(7);
	return  ((mtos_ticks_get()  - start_stb_time)*10);
    
}


void* kvdb_porting_memcpy(void *dest, void *src, unsigned int len)
{
     unsigned long dwCount;
     unsigned char *DestBuf,*SrcBuf;
     MT_ASSERT(dest !=NULL);
     MT_ASSERT(src != NULL);
     DestBuf=(unsigned char *)dest,
     SrcBuf=(unsigned char *)src;
     for(dwCount=0;dwCount<len;dwCount++)
         *(DestBuf+dwCount)=*(SrcBuf+dwCount);
     return (void *)dest;
}

 void kvdb_porting_memset(void * dest, int c, unsigned int len)
{
	unsigned long dwCount;
	unsigned char *DestBuf;
    MT_ASSERT(dest != NULL);
	DestBuf=(unsigned char *)dest;

	for(dwCount=0;dwCount<len;dwCount++)
		*(DestBuf+dwCount)=c;

}


int kvdb_porting_memcmp( void *buf1, void *buf2,unsigned int  len)
{
	unsigned long dwCount;
	unsigned char* cmp1,*cmp2;
  MT_ASSERT(buf1 != NULL);
  MT_ASSERT(buf2 != NULL);
	cmp1=(unsigned char*)buf1;
	cmp2=(unsigned char*)buf2;

	for(dwCount=0;dwCount<len;dwCount++)
	{
		if(*(cmp1+dwCount)<*(cmp2+dwCount))
			return -1;	
		else if(*(cmp1+dwCount)>*(cmp2+dwCount))
			return 1;
	}
	return 0;	
}

