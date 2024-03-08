/***************************************************************************//**
* \file system_psoc6m_cm4.h
* \version 1.0
*
* \brief Device system header file.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

/**
* \defgroup group_startup System Startup
* \{
* The startup code is responsible for the device memory map definition, the weak functions implementations for the
* interrupt vectors, copying interrupt vectors and data section from the ROM to the RAM, and for the heap and stack
* memory configuration.
*
* \section group_startup_configuration Configuration Considerations
*
* \subsection group_startup_flash_memory_layout Flash Layout
* For the dual-core devices, the flash memory is equally divided between cores. For the single-core devices, the 8 KB of
* flash is reserved for the hidden Cortex-M0+ application. This proportion can be changed as described in the following
* section.
*
* To change flash layout, do the following:
* - Pass the -D CY_CORTEX_M4_APPL_ADDR=0x10040000 to the Cortex-M0+ application compiler, where the 0x10040000 is the
*   intended start of the Cortex-M4 application;
* - Change the Cortex-M4 application placement address in the Cortex-M4 linker file to the same address.
*
* \subsection group_startup_ram_memory_layout RAM Layout
* For the dual-core devices, the RAM memory is equally divided between cores. For the single-core devices, the 8 KB of
* RAM is reserved for the hidden Cortex-M0+ application. This proportion can be changed as described in the following
* section.
*
* \subsection group_startup_heap_stack_config Heap and Stack Configuration
* By default, the stack size is set to 0x00001000 and heap size is set to 0x00000400. To change the stack, pass
* "-D __STACK_SIZE=0x000000400" to the compiler, where 0x000000400 is the desired stack size. To change heap pass
* "-D __HEAP_SIZE=0x000000100" to the compiler, where the 0x000000100 desired heap size.
*
* \section group_startup_more_information More Information
* After a power-on-reset (POR), the boot process is handled by the boot code from the on-chip ROM that is always
* executed by the Cortex-M0+ core. The boot code passes the control to the Cortex-M0+ startup code located in Flash.
* For the dual-core devices, the Cortex-M0+ startup code performs the device initialization and calls Cortex-M0+ main()
* function. The Cortex-M4 core is disabled by default and can be enabled using the \ref CySysEnableCM4() function.
* See \ref group_startup_cm4_functions for more details.
*
* \section group_startup_MISRA MISRA Compliance
*  The drivers violates the following MISRA-C:2004 rules:
*
* <table class="doxtable">
*   <tr>
*       <th>MISRA Rule</th>
*       <th>Rule Class (Required / Advisory)</th>
*       <th>Rule Description</th>
*       <th>Description of Deviation(s)</th>
*   </tr>
*   <tr>
*       <td>8.8</td>
*       <td>Required</td>
*       <td>An external object or function shall be declared in one and only one file.</td>
*       <td>The cm0p_image array is not used within project, so is defined without previous declaration.</td>
*   </tr>
* </table>
*
*
* \section group_startup_changelog Changelog
*   <table class="doxtable">
*   <tr>
*       <th>Version</th>
*       <th>Changes</th>
*       <th>Reason for Change</th>
*    </tr>
*   <tr>
*       <td>1.0</td>
*       <td>Initial version</td>
*       <td></td>
*   </tr>
* </table>
*
*
* \defgroup group_startup_macro Macro
* \defgroup group_startup_functions Functions
* \defgroup group_startup_cm4_functions Cortex-M4 Functions
* \defgroup group_startup_globals Global Variables
* \}
*/


#ifndef _SYSTEM_PSOC6M_CM4_H_
#define _SYSTEM_PSOC6M_CM4_H_

#define CY_CM0     ((defined(__GNUC__)       &&  (__ARM_ARCH == 6) && (__ARM_ARCH_6M__ == 1)) || \
                    (defined (__ICCARM__)     &&  (__CORE__ == __ARM6M__))  || \
                    (defined(__ARMCC_VERSION) &&  (__TARGET_ARCH_THUMB == 3)))

#if (!CY_CM0)

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** startup_m4 driver identifier */
#define CY_STARTUP_M4_ID        ((uint32_t)((uint32_t)((0x0Fu) & 0x3FFFu) << 18u))


/*******************************************************************************
* Inter Process Communication (IPC)
*******************************************************************************/

#define CY_IPC_CYPIPE_CLIENT_CNT        (8u)                /* Number of the defined clients */

#define CY_IPC_EP_MAX                   (CPUSS_IPC_IPC_NR)  /* Number of IPC structures */
#define CY_IPC_EP_CYPIPE_CM0_ADDR       (0u)
#define CY_IPC_EP_CYPIPE_CM4_ADDR       (1u)
#if (CY_CM0)
    #define CY_IPC_EP_CYPIPE_ADDR       CY_IPC_EP_CYPIPE_CM0_ADDR
#else
    #define CY_IPC_EP_CYPIPE_ADDR       CY_IPC_EP_CYPIPE_CM4_ADDR
#endif  /* (CY_CM0) */


#define CY_IPC_LOCK_COUNT               (128u)              /* Locks number. Must be a multiple of 32 */

/* IPC channels  */
#define CY_IPC_CHAN_SYSCALL_CM0         (0u)                /* System calls for the CM0 processor */
#define CY_IPC_CHAN_SYSCALL_CM4         (1u)                /* System calls for the 1st non-CM0 processor */
#if (CY_CM0)
    #define CY_IPC_CHAN_SYSCALL         CY_IPC_CHAN_SYSCALL_CM0
#else
    #define CY_IPC_CHAN_SYSCALL         CY_IPC_CHAN_SYSCALL_CM4
#endif  /* (CY_CM0) */
#define CY_IPC_CHAN_SYSCALL_DAP         (2u)                /* System call */
#define CY_IPC_CHAN_CRYPTO              (3u)                /* Crypto functions */
#define CY_IPC_CHAN_LOCK                (4u)                /* IPC Locks (System wide) */
#define CY_IPC_CHAN_CYPIPE_CM0          (5u)                /* Cypress Pipe, CM0 side */
#define CY_IPC_CHAN_CYPIPE_CM4          (6u)                /* Cypress Pipe, CM4 side */

/* IPC interrupts */
#define CY_IPC_INTR_SYSCALL_CM0         (0u)
#define CY_IPC_INTR_SYSCALL_CM4         (1u)
#define CY_IPC_INTR_SYSCALL_DAP         (2u)
#define CY_IPC_INTR_CRYPTO_M0           (3u)
#define CY_IPC_INTR_CRYPTO_M4           (4u)
#define CY_IPC_INTR_CYPIPE_CM0          (5u)
#define CY_IPC_INTR_CYPIPE_CM4          (6u)

/* IPC CM0 Interrupts  */
#if (CY_CM0)
    #define CY_IPC_INTR_CYPIPE_SRC      cpuss_interrupts_ipc5_IRQn
    #define CY_IPC_INTR_CYPIPE_CM0SRC   NvicMux28
    #define CY_IPC_INTR_CYPIPE_PRIO     (1u)
#else
    #define CY_IPC_INTR_CYPIPE_SRC      cpuss_interrupts_ipc6_IRQn
    #define CY_IPC_INTR_CYPIPE_CM0SRC   (240u)              /* Default value of CM0_INT_CTLx register */
    #define CY_IPC_INTR_CYPIPE_PRIO     (1u)
#endif

/* The System pipe configuration defines the IPC channel number, interrupt number and the pipe interrupt mask */
#define CY_IPC_CYPIPE_INTR_MASK         (uint32_t)((0x0001ul << CY_IPC_CHAN_CYPIPE_CM0) | (0x0001ul << CY_IPC_CHAN_CYPIPE_CM4) )

#if (CY_CM0)
    #define CY_IPC_CYPIPE_CONFIG        (uint32_t)( (CY_IPC_CYPIPE_INTR_MASK << 16) | (CY_IPC_INTR_CYPIPE_CM0 << 8u) | CY_IPC_CHAN_CYPIPE_CM0)
#else
    #define CY_IPC_CYPIPE_CONFIG        (uint32_t)( (CY_IPC_CYPIPE_INTR_MASK << 16) | (CY_IPC_INTR_CYPIPE_CM4 << 8u) | CY_IPC_CHAN_CYPIPE_CM4)
#endif


/**
* \addtogroup group_startup_functions
* \{
*/
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
/** \} group_startup_functions */

/** \cond */
extern void     Default_Handler (void);
extern uint32_t Cy_SaveIRQ(void);
extern void     Cy_RestoreIRQ(uint32_t saved);

extern void     Cy_SMIF_StartupCallBack(void);
extern void     Cy_SystemInit(void);
extern void     Cy_SystemInitIpc(void);
extern void     Cy_SystemInitFpuEnable(void);

extern uint32_t cy_delayFreqHz;
extern uint32_t cy_delayFreqKhz;
extern uint8_t  cy_delayFreqMhz;
extern uint32_t cy_delay32kMs;

extern uint32_t cy_BleEcoClockFreqHz;
extern uint32_t cy_Hfclk0FreqHz;
extern uint32_t cy_PeriClkFreqHz;
/** \endcond */

/**
* \addtogroup group_startup_macro
* \{
*/

/** \} group_startup_macro */


/** \addtogroup group_startup_globals
* \{
*/

extern uint32_t SystemCoreClock;

/** \} group_startup_globals */

#ifdef __cplusplus
}
#endif

#endif /* !CY_CM0 */
#endif /* _SYSTEM_PSOC6M_CM4_H_ */

/** \} group_startup */


/* [] END OF FILE */
