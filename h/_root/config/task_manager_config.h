/*LICENSE ********************************************************************
 * Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 * ***************************************************************************/
/*@@task_manager_config.h
 * ***********************************************************************************************
 * File:   task_manager_config.h
 * Author: M91406
 * 
 * Summary:
 * User configuration file for basic task scheduler parameters
 * 
 * Description:
 * Users can specify which timer drives the task scheduler and at which frequency. further
 * some fine tuning options are available to account for runtime influences of compiler 
 * optimizations on CPU meter and task time quota monitoring.
 * 
 * History:
 * 07/28/2017	File created
 * ***********************************************************************************************/

#ifndef _ROOT_TASK_MANAGER_CONFIGURATION_H_
#define	_ROOT_TASK_MANAGER_CONFIGURATION_H_

/* ***********************************************************************************************
 * DEFAULT INCLUDES
 * ***********************************************************************************************/

#include <stdint.h>
#include <math.h>
#include "mcal/mcal.h" // required to include p33SMPS_devices.h
#include "mcal/config/devcfg_oscillator.h"


/*@@USE_TASK_EXECUTION_CLOCKOUT_PIN
 * ***********************************************************************************************
 * Description:
 * CPU load monitoring for code execution profile analysis is supported by two features:
 * 
 *    - MCU debug pin indicating task execution periods
 *    - CPU time counter logged in history array
 * 
 * To use the debug pin features and monitor task execution timing on an oscilloscope, a
 * MCU pin needs to be available and initialized as output. The related port-latch register
 * bit needs to declared as TS_CLOCKOUT_PIN_WR (CLOCKOUT-WRITE).
 * 
 * e.g. #define TS_CLOCKOUT_PIN_WR  LATAbits.LATA0 when port-pin RA0 is used as generic debugging output pin
 * 
 * Settings:
 * Set USE_TASK_EXECUTION_CLOCKOUT_PIN = 1 to enable the debug pin task time output feature.
 * Set USE_TASK_EXECUTION_CLOCKOUT_PIN = 0 to disable the debug pin task time output feature.
 *
 * If USE_TASK_EXECUTION_CLOCKOUT_PIN = 1 but no clock-output pin is defined, a build error will 
 * be triggered.
 * 
 * See also:
 * CLKOUT_WR
 * ***********************************************************************************************/
#define USE_TASK_EXECUTION_CLOCKOUT_PIN     1   // Enable/Disable Task Scheduler Clock Output

#if (USE_TASK_EXECUTION_CLOCKOUT_PIN == 1)
    #define TS_CLOCKOUT_PIN_WR              DBGPIN_WR   // Specify Clock Output Pin port latch register
    #define TS_CLOCKOUT_PIN_INIT_OUTPUT     DBGPIN_INIT_OUTPUT   // Specify Clock Output Pin configuration
    #define USE_DETAILED_CLOCKOUT_PATTERN   1           // Enable/Disable detailed clock pattern
#endif

/*@@USE_TASK_MANAGER_TIMING_DEBUG_ARRAYS
 * ***********************************************************************************************
 * Description:
 * CPU load and task execution time monitoring can be enabled internally during debug mode
 * by setting this option =1.
 * 
 * When enabled, two data arrays are used to log most recent CPU meter and task execution time
 * meter results. This function collects data continuously by filling the arrays from index 0
 * to n, rolls over and continues to add data from index 0. A software breakpoint needs to be 
 * placed to stop code execution and inspect the collected data arrays.
 * 
 * CPU_LOAD_DEBUG_BUFFER_LENGTH determines the length of the data arrays.
 * 
 * See also:
 * CPU_LOAD_DEBUG_BUFFER_LENGTH
 * ***********************************************************************************************/

#if __DEBUG
#define USE_TASK_MANAGER_TIMING_DEBUG_ARRAYS 1
#else
#define USE_TASK_MANAGER_TIMING_DEBUG_ARRAYS 0
#endif

/*@@Task Manager Heartbeat Configuration
 * ***********************************************************************************************
 * Description:
 * The task manager base frequency is configured here. The task manager requires a system timer 
 * to run. Each loop execution is triggered by a timer interrupt flag bit. 
 * 
 * TASK_MGR_TIME_STEP:  defines the tick period in [sec].
 * TASK_MGR_PERIOD:     Macro; calculates the actual timer period based on the user defined 
 *                      oscillator frequency.
 * TASK_MGR_TIMER_INDEX: index of the system timer used by the task manager
 * TASK_MGR_TIMER_COUNTER_REGISTER: Timer counter register
 * TASK_MGR_TIMER_PERIOD_REGISTER: Timer period register
 * TASK_MGR_TIMER_ISR_FLAG_REGISTER: Timer interrupt flag bit
 * TASK_MGR_TIMER_ISR_FLAG_BIT_MASK: Timer interrupt flag bit mask (filtering the particular  
 *                                   flag bit within an SFR)
 * 
 * See also:
 * (none)
 * ***********************************************************************************************/

#define TASK_MGR_TIME_STEP                  (float)(100.0e-6)     // Schedule time step in [sec]
#define TASK_MGR_PERIOD                     (uint16_t)((float)FCY * (float)TASK_MGR_TIME_STEP)

#define TASK_MGR_TIMER_INDEX                1
#define TASK_MGR_TIMER_COUNTER_REGISTER     TMR1
#define TASK_MGR_TIMER_PERIOD_REGISTER      PR1
#define TASK_MGR_TIMER_ISR_FLAG_REGISTER    IFS0

#if defined (__P33SMPS_CK1__) || defined (__P33SMPS_CK2__) || defined (__P33SMPS_CK5__)
  #define TASK_MGR_TIMER_ISR_FLAG_BIT_MASK        0b0000000000000010
#elif defined (__P33SMPS_CH2__) || defined (__P33SMPS_CH5__)
  #define TASK_MGR_TIMER_ISR_FLAG_BIT_MASK        0b0000000000000010
#elif defined (__P33SMPS_EP2__) || defined (__P33SMPS_EP5__) || defined (__P33SMPS_EP7__)
  #define TASK_MGR_TIMER_ISR_FLAG_BIT_MASK        0b0000000000001000
#elif defined (__P33SMPS_FJ__) || defined (__P33SMPS_FJA__) || defined (__P33SMPS_FJC__)
  #define TASK_MGR_TIMER_ISR_FLAG_BIT_MASK        0b0000000000001000
#else
  #error === selected device family could not be indentified or is not supported by the task manager  ===
#endif
    
/*@@CPU Meter Configuration
 * ***********************************************************************************************
 * Description:
 * CPU metering requires additional informatoin about the main while loop execution time.
 * While the CPU Load Meter is waiting for the sysem timer to overrun, it increments a counter.
 * To put the final counter value into relation ot "free" CPU cycles, we need to know how many cycles
 * ONE waiting loop takes. This highly depends on the level of code optimization set in the Project
 * Properties.
 * 
 * To ensure this setting is never missed, we add another common macro to 
 *  - Project Properties -> XC16 -> Define common macros
 *  - Add one of the following macros which fits your Code Optimization Level setting
 * 
 *      * __CODE_OPT_LEVEL_0__
 *      * __CODE_OPT_LEVEL_1__
 *      * __CODE_OPT_LEVEL_2__
 *      * __CODE_OPT_LEVEL_s__
 *      * __CODE_OPT_LEVEL_3__
 * 
 * Should you use a special configuration of your code optimizer, please use macro
 * __CODE_OPT_LEVEL_USR__ and measure the number of cycles of the CPU Load Meter Counter Loop
 * using the MPLAB X Stopwatch.
 * 
 * Note:
 * Changes in compiler versions may also have an influence on the number of CPU cycles required 
 * for one loop execution. It is recommended to verify the listed settings every time the compiler 
 * has been upgraded or this framework is applied to a new device family.
 * 
 * See also:
 * (none)
 * ***********************************************************************************************/
#if __XC16_VERSION > 1040   // Example: v1.36 is represented by 1036
    #pragma message "=== The CPU Load Meter has not been tested with the recent compiler version ==="
    // If this message occurs in the output window, please verify the constants 
    // TASK_MGR_CPU_LOAD_NOMBLK by using the MPLAB X stopwatch (see comment above)
#endif 

#if defined (__P33SMPS_CH__) || defined (__P33SMPS_CK__)
// CPU Load Monitor Counter Loop Performance Settings on dsPIC33CH MP Devices

    #ifdef __CODE_OPT_LEVEL_0__
        #define TASK_MGR_CPU_LOAD_NOMBLK            28      // Number of cycles for one TxIF-wait while loop iteration at code optimization #0
    #endif
    #ifdef __CODE_OPT_LEVEL_1__
        #define TASK_MGR_CPU_LOAD_NOMBLK            20      // Number of cycles for one TxIF-wait while loop iteration at code optimization #1
    #endif
    #ifdef __CODE_OPT_LEVEL_2__
        #define TASK_MGR_CPU_LOAD_NOMBLK            23      // Number of cycles for one TxIF-wait while loop iteration at code optimization #2
    #endif
    #ifdef __CODE_OPT_LEVEL_s__
        #define TASK_MGR_CPU_LOAD_NOMBLK            23      // Number of cycles for one TxIF-wait while loop iteration at code optimization #s
    #endif
    #ifdef __CODE_OPT_LEVEL_3__
        #define TASK_MGR_CPU_LOAD_NOMBLK            23      // Number of cycles for one TxIF-wait while loop iteration at code optimization #3
    #endif
    #ifdef __CODE_OPT_LEVEL_USR__
        #define TASK_MGR_CPU_LOAD_NOMBLK            21      // Number of cycles for one TxIF-wait while loop iteration at user configured code optimization 
    #endif

#endif

#define TASK_MGR_CPU_LOAD_FACTOR            (uint16_t)(((float)(1000.000)/(float)(TASK_MGR_PERIOD))*pow(2, 16))
#define TASK_MGR_ISR_PRIORITY               1       // Timer ISR priority
#define TASK_MGR_ISR_STATE                  0       // Timer ISR state (0=disabled, 1=enabled)
    
 /* ***********************************************************************************************
 * PROTOTYPES
 * ***********************************************************************************************/


#endif	/* _ROOT_TASK_MANAGER_CONFIGURATION_H_ */

// EOF
