##### **Large middleware like LVGL:**

1.start debug,open register window,look for SP,run through code see if it drops below stack limit

2.SET LARGER STACK SIZE!!! if not lvgl

3.lvgl children by default inherit parent obj properties; use static styles for reuse, lv\_obj\_set\_style\_... use more ram for every obj

4\.



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

/\*1: use custom malloc/free, 0: use the built-in `lv\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_mem\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_alloc()` and `lv\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_mem\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_free()`\*/

\#define LV\_MEM\_CUSTOM 1

\#if LV\_MEM\_CUSTOM == 0

    /\*Size of the memory available for `lv\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_mem\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_alloc()` in bytes (>= 2kB)\*/

    #define LV\_MEM\_SIZE (48U \* 1024U)          /\*\[bytes]\*/



    /\*Set an address for the memory pool instead of allocating it as a normal array. Can be in external SRAM too.\*/

    #define LV\_MEM\_ADR 0     /\*0: unused\*/

    /\*Instead of an address give a memory allocator that will be called to get a memory pool for LVGL. E.g. my\_malloc\*/

    #if LV\_MEM\_ADR == 0

        #undef LV\_MEM\_POOL\_INCLUDE

        #undef LV\_MEM\_POOL\_ALLOC

    #endif



\#else       /\*LV\_MEM\_CUSTOM\*/

    #define LV\_MEM\_CUSTOM\_INCLUDE "FreeRTOS.h"   /\*Header for the dynamic memory function\*/

    #define LV\_MEM\_CUSTOM\_ALLOC   pvPortMalloc

    #define LV\_MEM\_CUSTOM\_FREE    vPortFree

    #define LV\_MEM\_CUSTOM\_REALLOC pvPortRealloc --》 GitHub func underreview

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

                                    StackType\_t \*\*ppxIdleTaskStackBuffer,

                                    configSTACK\_DEPTH\_TYPE \*pulIdleTaskStackSize )

{

    /\* If the buffers to be provided to the Idle task are declared inside this

       function then they must be declared static - otherwise they will be allocated on

       the stack and so not exists after this function exits. \*/

    static StaticTask\_t xIdleTaskTCB;

    static StackType\_t uxIdleTaskStack\[ configMINIMAL\_STACK\_SIZE ];



    /\* Pass out a pointer to the StaticTask\_t structure in which the Idle task's

       state will be stored. \*/

    \*ppxIdleTaskTCBBuffer = \&xIdleTaskTCB;



    /\* Pass out the array that will be used as the Idle task's stack. \*/

    \*ppxIdleTaskStackBuffer = uxIdleTaskStack;



    /\* Pass out the size of the array pointed to by \*ppxIdleTaskStackBuffer.

       Note that, as the array is necessarily of type StackType\_t,

       configMINIMAL\_STACK\_SIZE is specified in words, not bytes. \*/

    \*pulIdleTaskStackSize = configMINIMAL\_STACK\_SIZE;

}



\#if configUSE\_TIMERS



void vApplicationGetTimerTaskMemory( StaticTask\_t \*\*ppxTimerTaskTCBBuffer,

                                     StackType\_t \*\*ppxTimerTaskStackBuffer,

                                     uint32\_t \*pulTimerTaskStackSize )

{

    /\* If the buffers to be provided to the Timer task are declared inside this

       function then they must be declared static - otherwise they will be allocated on

       the stack and so not exists after this function exits. \*/

    static StaticTask\_t xTimerTaskTCB;

    static StackType\_t uxTimerTaskStack\[ configTIMER\_TASK\_STACK\_DEPTH ];



    /\* Pass out a pointer to the StaticTask\_t structure in which the Timer

       task's state will be stored. \*/

    \*ppxTimerTaskTCBBuffer = \&xTimerTaskTCB;



    /\* Pass out the array that will be used as the Timer task's stack. \*/

    \*ppxTimerTaskStackBuffer = uxTimerTaskStack;



    /\* Pass out the size of the array pointed to by \*ppxTimerTaskStackBuffer.

       Note that, as the array is necessarily of type StackType\_t,

      configTIMER\_TASK\_STACK\_DEPTH is specified in words, not bytes. \*/

    \*pulTimerTaskStackSize = configTIMER\_TASK\_STACK\_DEPTH;

}

\#endif //configUSE\_TIMERS



\#endif // configSUPPORT\_STATIC\_ALLOCATION == 1



\#ifdef configUSE\_MALLOC\_FAILED\_HOOK

void vApplicationMallocFailedHook(void)

{

    taskDISABLE\_INTERRUPTS();

    for (;;);

}

\#endif



###### ffconf.h

 	#include "FreeRTOS.h"

 	#include "semphr.h"

 	。。。

 	#define	\_USE\_LFN	3

 	#define \_FS\_REENTRANT	1

 	#define \_FS\_TIMEOUT		1000

 	#define	\_SYNC\_t			SemaphoreHandle\_t







##### **Font generation https://lvgl.io/tools/fontconverter**

name awesomesyms

size 18

bpp 4 bit-per-pixel

fallback (none)

output format C file

0xF001, 0xF002, 0xF004, 0xF005, 0xF007, 0xF008, 0xF009, 0xF00A, 0xF00B, 0xF00C, 0xF00D, 0xF00E, 0xF010, 0xF011, 0xF012, 0xF013, 0xF015, 0xF017, 0xF019, 0xF01C, 0xF01E, 0xF021, 0xF022, 0xF023, 0xF025, 0xF026, 0xF027, 0xF028, 0xF02D, 0xF02E, 0xF030, 0xF031, 0xF032, 0xF033, 0xF035, 0xF036, 0xF037, 0xF038, 0xF039, 0xF03A, 0xF03B, 0xF03C, 0xF03D, 0xF03E, 0xF041, 0xF042, 0xF043, 0xF044, 0xF048, 0xF049, 0xF04A, 0xF04B, 0xF04C, 0xF04D, 0xF04E, 0xF050, 0xF051, 0xF052, 0xF053, 0xF054, 0xF055, 0xF056, 0xF057, 0xF058, 0xF059, 0xF05A, 0xF05B, 0xF05E, 0xF060, 0xF061, 0xF062, 0xF063, 0xF064, 0xF065, 0xF066, 0xF067, 0xF068, 0xF069, 0xF06A, 0xF06B, 0xF06C, 0xF06D, 0xF06E, 0xF070, 0xF071, 0xF073, 0xF074, 0xF075, 0xF076, 0xF077, 0xF078, 0xF079, 0xF07B, 0xF07C, 0xF080, 0xF084, 0xF085, 0xF086, 0xF08D, 0xF091, 0xF093, 0xF095, 0xF09B, 0xF09C, 0xF09E, 0xF0A0, 0xF0A1, 0xF0A4, 0xF0A5, 0xF0A6, 0xF0A7, 0xF0A8, 0xF0A9, 0xF0AA, 0xF0AB, 0xF0AC, 0xF0AD, 0xF0AE, 0xF0B0, 0xF0B1, 0xF0B2, 0xF0C1, 0xF0C2, 0xF0C4, 0xF0C5, 0xF0C6, 0xF0C7, 0xF0C8, 0xF0C9, 0xF0CA, 0xF0CB, 0xF0CC, 0xF0CD, 0xF0CE, 0xF0D7, 0xF0D8, 0xF0D9, 0xF0DA, 0xF0DB, 0xF0DC, 0xF0DD, 0xF0DE, 0xF0E2, 0xF0E8, 0xF0EA, 0xF0EB, 0xF0F2, 0xF0F3, 0xF0FE, 0xF100, 0xF101, 0xF102, 0xF103, 0xF104, 0xF105, 0xF106, 0xF107, 0xF108, 0xF109, 0xF10A, 0xF10B, 0xF10D, 0xF10E, 0xF110, 0xF111, 0xF118, 0xF119, 0xF11A, 0xF11B, 0xF11C, 0xF120, 0xF121, 0xF122, 0xF124, 0xF125, 0xF126, 0xF127, 0xF128, 0xF129, 0xF12A, 0xF12B, 0xF12C, 0xF12D, 0xF130, 0xF131, 0xF133, 0xF137, 0xF138, 0xF139, 0xF13A, 0xF13E, 0xF141, 0xF142, 0xF144, 0xF146, 0xF14A, 0xF14B, 0xF14D, 0xF14E, 0xF150, 0xF151, 0xF152, 0xF15B, 0xF15C, 0xF15D, 0xF15E, 0xF160, 0xF161, 0xF162, 0xF163, 0xF164, 0xF165, 0xF185, 0xF186, 0xF187, 0xF188, 0xF193, 0xF1B8, 0xF1C0, 0xF1C2, 0xF1C3, 0xF1C4, 0xF1C5, 0xF1C6, 0xF1C7, 0xF1C8, 0xF1C9, 0xF1DE, 0xF1E0, 0xF1E6, 0xF1EB, 0xF1EC, 0xF1F6, 0xF1F8, 0xF1FA, 0xF1FE, 0xF200, 0xF201, 0xF240, 0xF241, 0xF242, 0xF243, 0xF244, 0xF245, 0xF246, 0xF249, 0xF251, 0xF252, 0xF253, 0xF254, 0xF267, 0xF26C, 0xF271, 0xF272, 0xF273, 0xF274, 0xF276, 0xF28B, 0xF28D, 0xF292, 0xF294, 0xF2A4, 0xF2A8, 0xF2C7, 0xF2C8, 0xF2C9, 0xF2CA, 0xF2CB, 0XF2D0, 0xF2EA, 0xF2ED, 0xF2F1, 0xF2F2, 0xF2F5, 0xF2F6, 0xF2F9, 0xF302, 0xF303, 0xF304, 0xF328, 0xF337, 0xF338, 0xF362, 0xF368, 0xF3C1, 0xF3C5, 0xF40D, 0xF46C, 0xF46D, 0xF4B3, 0xF4FE, 0xF4FF, 0xF500, 0xF503, 0xF565, 0xF56D, 0xF56E, 0xF56F, 0xF573, 0xF574, 0xF576, 0xF58D, 0xF58E, 0xF58F, 0xF590, 0xF5AC, 0xF5CB, 0xF658, 0xF65D, 0xF65E, 0xF6A9, 0xF6C3, 0xF6C4, 0xF6DD, 0xF6E3, 0xF6FF, 0xF70C, 0xF715, 0xF73B, 0xF73C, 0xF73D, 0xF740, 0xF743, 0xF75F, 0xF7C2, 0xF7C4, 0xF841, 0xF8A2



