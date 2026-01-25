##### **Large middleware like LVGL:**

1.start debug

2.open register window

3.look for SP

4.run through code see if it drops below stack limit

5.SET LARGER STACK SIZE!!!



##### **RTOS:**

put SCV, PendSV handler into the stmxxxx\_it.c

SystickHandler in delay.c



##### **启动文件**

###### .s中的栈：

1.CPU 上电/复位时

初始化 .bss / .data

调用 SystemInit()、main()

2.Early ISR / 异常处理

上电前或 RTOS 启动前的中断

Cortex-M 上 ISR 默认用 MSP

3\.裸机代码 / 非 RTOS 线程函数

所有没有自己独立栈的函数调用

静态函数、全局函数、库函数在 RTOS 任务启动前



###### .s中的堆：

libc malloc / free

new / delete

printf

scanf

fopen



###### .s的栈与堆与RTOS的静态分配的堆不相干；真正限制上限的仍然是SRAM物理大小，所以SRAM=.s栈堆+中间件堆+。。。





##### **lv\_conf.h**

/\*1: use custom malloc/free, 0: use the built-in `lv\_mem\_alloc()` and `lv\_mem\_free()`\*/

\#define LV\_MEM\_CUSTOM 1

\#if LV\_MEM\_CUSTOM == 0

&nbsp;   /\*Size of the memory available for `lv\_mem\_alloc()` in bytes (>= 2kB)\*/

&nbsp;   #define LV\_MEM\_SIZE (48U \* 1024U)          /\*\[bytes]\*/



&nbsp;   /\*Set an address for the memory pool instead of allocating it as a normal array. Can be in external SRAM too.\*/

&nbsp;   #define LV\_MEM\_ADR 0     /\*0: unused\*/

&nbsp;   /\*Instead of an address give a memory allocator that will be called to get a memory pool for LVGL. E.g. my\_malloc\*/

&nbsp;   #if LV\_MEM\_ADR == 0

&nbsp;       #undef LV\_MEM\_POOL\_INCLUDE

&nbsp;       #undef LV\_MEM\_POOL\_ALLOC

&nbsp;   #endif



\#else       /\*LV\_MEM\_CUSTOM\*/

&nbsp;   #define LV\_MEM\_CUSTOM\_INCLUDE "FreeRTOS.h"   /\*Header for the dynamic memory function\*/

&nbsp;   #define LV\_MEM\_CUSTOM\_ALLOC   pvPortMalloc

&nbsp;   #define LV\_MEM\_CUSTOM\_FREE    vPortFree

&nbsp;   #define LV\_MEM\_CUSTOM\_REALLOC pvPortRealloc

\#endif     /\*LV\_MEM\_CUSTOM\*/



##### FATFS

###### include syscall.c

\#include "FreeRTOS.h"

\#include "semphr.h"

\#include "task.h"

...

uncomment the FreeRTOS parts



###### port.c:

\#if ( configSUPPORT\_STATIC\_ALLOCATION == 1 )

void vApplicationGetIdleTaskMemory( StaticTask\_t \*\*ppxIdleTaskTCBBuffer,

&nbsp;                                   StackType\_t \*\*ppxIdleTaskStackBuffer,

&nbsp;                                   configSTACK\_DEPTH\_TYPE \*pulIdleTaskStackSize )

{

&nbsp;   /\* If the buffers to be provided to the Idle task are declared inside this

&nbsp;      function then they must be declared static - otherwise they will be allocated on

&nbsp;      the stack and so not exists after this function exits. \*/

&nbsp;   static StaticTask\_t xIdleTaskTCB;

&nbsp;   static StackType\_t uxIdleTaskStack\[ configMINIMAL\_STACK\_SIZE ];



&nbsp;   /\* Pass out a pointer to the StaticTask\_t structure in which the Idle task's

&nbsp;      state will be stored. \*/

&nbsp;   \*ppxIdleTaskTCBBuffer = \&xIdleTaskTCB;



&nbsp;   /\* Pass out the array that will be used as the Idle task's stack. \*/

&nbsp;   \*ppxIdleTaskStackBuffer = uxIdleTaskStack;



&nbsp;   /\* Pass out the size of the array pointed to by \*ppxIdleTaskStackBuffer.

&nbsp;      Note that, as the array is necessarily of type StackType\_t,

&nbsp;      configMINIMAL\_STACK\_SIZE is specified in words, not bytes. \*/

&nbsp;   \*pulIdleTaskStackSize = configMINIMAL\_STACK\_SIZE;

}



\#if configUSE\_TIMERS



void vApplicationGetTimerTaskMemory( StaticTask\_t \*\*ppxTimerTaskTCBBuffer,

&nbsp;                                    StackType\_t \*\*ppxTimerTaskStackBuffer,

&nbsp;                                    uint32\_t \*pulTimerTaskStackSize )

{

&nbsp;   /\* If the buffers to be provided to the Timer task are declared inside this

&nbsp;      function then they must be declared static - otherwise they will be allocated on

&nbsp;      the stack and so not exists after this function exits. \*/

&nbsp;   static StaticTask\_t xTimerTaskTCB;

&nbsp;   static StackType\_t uxTimerTaskStack\[ configTIMER\_TASK\_STACK\_DEPTH ];



&nbsp;   /\* Pass out a pointer to the StaticTask\_t structure in which the Timer

&nbsp;      task's state will be stored. \*/

&nbsp;   \*ppxTimerTaskTCBBuffer = \&xTimerTaskTCB;



&nbsp;   /\* Pass out the array that will be used as the Timer task's stack. \*/

&nbsp;   \*ppxTimerTaskStackBuffer = uxTimerTaskStack;



&nbsp;   /\* Pass out the size of the array pointed to by \*ppxTimerTaskStackBuffer.

&nbsp;      Note that, as the array is necessarily of type StackType\_t,

&nbsp;     configTIMER\_TASK\_STACK\_DEPTH is specified in words, not bytes. \*/

&nbsp;   \*pulTimerTaskStackSize = configTIMER\_TASK\_STACK\_DEPTH;

}

\#endif //configUSE\_TIMERS



\#endif // configSUPPORT\_STATIC\_ALLOCATION == 1



\#ifdef configUSE\_MALLOC\_FAILED\_HOOK 

void vApplicationMallocFailedHook(void)

{

&nbsp;   taskDISABLE\_INTERRUPTS();

&nbsp;   for (;;);

}

\#endif



###### ffconf.h

&nbsp;	#include "FreeRTOS.h"

&nbsp;	#include "semphr.h"

&nbsp;	。。。

&nbsp;	#define	\_USE\_LFN	3

&nbsp;	#define \_FS\_REENTRANT	1

&nbsp;	#define \_FS\_TIMEOUT		1000

&nbsp;	#define	\_SYNC\_t			SemaphoreHandle\_t

