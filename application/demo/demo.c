//#include "demo.h"

//#define TEST_MEM
#ifdef TEST_MEM
#include "common_def.h"  
#include "soc_osal.h"
#endif  
#define EXAMPLE_MEM_SIZE 100 
/*
void demo_prinf(void)
{
    osal_printk("demo_print\r\n");    
}
*/
void test_task(void)
{
#ifdef     TEST_MEM
/* 申请内存 */ 
 void* mem = osal_kmalloc(EXAMPLE_MEM_SIZE, NULL); 
 if(mem == NULL)
    {
        osal_printk("Malloc failed!\n");
    }
osal_printk("Using memory as expected!\n"); 
 /* 释放内存 */
osal_kfree(mem);
#endif


}
