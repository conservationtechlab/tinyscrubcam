#ifndef CLOCK_TIMER_H
#define CLOCK_TIMER_H

#include "brcm_fw_types.h"
//#include "special_hw/clockhw.h"
//#include "foundation/msgqueue.h"

//==================================================================================================
// Types and constants
//==================================================================================================

//! Identifier for a particular branch of the clock tree.
typedef UINT8 clock_id_t;

//! Reserved clock mode to represent an uninitialized state.
#define CLOCK_MODE_UNKNOWN      0

//! Reserved clock mode to represent a disabled state.  If a given branch of the clock tree does
//! not support disabling it, then the code which manages that branch should treat this value as
//! being synonymous with the lowest power consuming mode of the actual options.
#define CLOCK_MODE_DISABLED     1



//! Timer call back prototype definition.
struct NOTIFY_TIME;
typedef void (*NotifyTimeCallback)(struct NOTIFY_TIME* which_notify);

/*******************************************Timer functions*********************************************/
//! Structure used to register for (and receive) notification when a specific time arrives.  The
//! target time value is based on the timebase defined by clock_SystemTimeMicroseconds64.
/*******************************************Timer functions*********************************************/
#define OSAPI_TIMER_PERIODIC        1
#define OSAPI_TIMER_WAKEUP          2
#define OSAPI_TIMER_ACTIVE          4
#define OSAPI_TIMER_PRESERVE_CB     8

//! Timer structure
typedef struct OSAPI_TIMER_STRUCT
{
    struct OSAPI_TIMER_STRUCT *  next;  /* Next timer in list */
    // overTimeInUs tells the user how long in micorsecond has passed beyond the targeted time
    VOID          (*callback)(INT32 registrantData, UINT32 overTimeInUs);   /* timer expiration callback function */
    UINT32        flags;                /* Flags for timer*/
    UINT32        interval;             /* Periodical time out inteval, in 1 us unit */
    INT32         arg;                  /* parameter for expiration function */
    UINT64        target_time;          /* Target time for timer expire, in us */
} OSAPI_TIMER;

//! Create a periodic timer.
//! Interval is in 1 us unit.
extern void osapi_createPeriodicTimer(OSAPI_TIMER *timer,
                  void (*timerCallback)(INT32 arg, UINT32 overTimeInUs),
                  INT32  arg,
                  UINT32  interval);

//! Create a timer.
void osapi_createTimer(OSAPI_TIMER *timer, void (*timerCallback)(INT32 arg, UINT32 overTimeInUs), INT32 arg);

//! Activates an OSAPI timer.
//! If this timer can be a wakeup source, we should call
//! osapi_setTimerWakeupSource after osapi_createTimer and before
//! osapi_activateTimer. Interval here is in micro seconds.
extern void osapi_activateTimer(OSAPI_TIMER* timer, UINT32 interval);


//! Reports if timer is running/pending or not.
#define osapi_is_timer_running(timer) ((timer)->flags&OSAPI_TIMER_ACTIVE)

//! Marks the timer as being a wake up source.
//! This may be called before the call to osapi_activateTimer() to mark
//! the timer as a wake up source.
extern void osapi_setTimerWakeupSource( OSAPI_TIMER* timer, UINT32 can_be_wakeup_source);

//! Remove the timer from timer list.
//! It will return 0 if successful
//! It will return 1 if timeout callback already called.
//! It will return 2 otherwise.
extern UINT32 osapi_deactivateTimer(OSAPI_TIMER* timer);

//! Query timer for remain time until timeout.
//! This function will query current timer, return microseconds from now until it timeout
extern UINT32 osapi_getTimerRemain(OSAPI_TIMER* timer);


//==================================================================================================
// Functions
//==================================================================================================
void clock_InitTimebase(void);


//! Delay us seconds
//! This function can only be used after system clock switching
void clock_DelayMicroseconds(UINT32 us);


//! System time in us, 64bits will not roll over.
UINT64 clock_SystemTimeMicroseconds64(void);
//! System time in us, 32bits will roll over every 1 hour 11 minutes.
UINT32 clock_SystemTimeMicroseconds32(void);

//! Function to return current 32 bit timestamp, to be called with interrupts locked or from ISR context.
UINT32 clock_SystemTimeMicroseconds32_nolock(void);

//! Update time base after sleep
void clock_UpdateTimebaseAfterSleep(UINT64 microseconds_elapsed_with_timers_off);

//! Enable Timers if stopped.
// if timers are stopped by g_clock_disableTimers=1 and this variable is set back to 0
// timers will be restored here, all active connections will be lost
extern void clock_enableTimersIfStopped(void);


//---------------------------------------------------------------------------------------------------------

typedef enum {          // this enumerated type is also used to index a table of required wait states
    CLOCK_DISABLE,      // for Thick Oxide Ram in slimboot.h.  If you modify this type, update the table
    CLOCK_1MHZ,         // in slimboot.h as well.  20150714 (sm)
    CLOCK_4MHZ,
    CLOCK_6MHZ,
    CLOCK_12MHZ,
    CLOCK_16MHZ,
    CLOCK_24MHZ,
    CLOCK_32MHZ,
    CLOCK_48MHZ,
    CLOCK_96MHZ,
    CLOCK_XTAL,
} CLOCK_FREQ;

typedef enum {
    CLOCK_REQ_TRANS,            // Set Transport clock to specific clock freqency.
    CLOCK_REQ_FM,               // Set FM clock  freqency.
    CLOCK_REQ_CPU_SET,          // Set CPU to specific clock frequency
    CLOCK_REQ_CPU_DEFINE_MIN,   // Define the min freq we wlil set the cpu to via needup/release calls.
    CLOCK_REQ_CPU_DEFINE_MAX,   // Define the max freq we wlil set the cpu to via needup/release calls.    
    CLOCK_REQ_CPU_NEED_UPTO,    // Update CPU to match new (usually more) clock frequency requirement
    CLOCK_REQ_CPU_RELEASE_FROM, // release previous clock freqency updating requirement.
} CLOCK_REQ;

//! Request clock.
//! This will make clock request based on req type.
extern UINT32 clock_Request(CLOCK_REQ req, CLOCK_FREQ freq);


//! Determines the reference crystal speed and initializes the timebase.
extern void clock_Init(void);

//! Special version of clock_DelayMicroseconds.  The long name is to deter abuse, as this function
//! bypasses the locking of clock manager state.  This should be used only from inside a clock
//! manager clock state transition pre or post callback.
extern void clock_DelayMicrosecondsWithClockManagerStateLocked(int microseconds);

//! Delays execution for the specified number of clock cycles.  Unless the required delay is
//! determined directly by the CPU clock, use of clock_DelayMicroseconds is probably a better
//! choice.  Please exercise appropriate discretion.
extern void clock_DelayCPUClockCycles(int cycles);

//! Returns whether the timebase has been initialized yet.  If not, none of the timebase-related
//! functions can be called.  In that case, the delay functions can still be called, and they will
//! simply assume worst case scenario to assure the required delay has elapsed before returning,
//! though likely longer.
extern BOOL32 clock_HasTimebaseBeenInitialized(void);

//! Gets the crystal frequency in KHz.  Before the crystal frequency has been established, the
//! function will return 0.  Clients to the clock manager should generally stick to use of
//! clock_freq.  Cases when the crystal frequency needs to be used explicitly should be rare.
//! Please exercise appropriate discretion.
extern UINT32 clock_CrystalFrequencyKHz(void);

//! Determines the current freqency that CPU is running.
extern UINT32 clock_CpuFrequencyKhz(void);

#ifdef PMU_SDS
//! Restored the timer saved before SDS
extern void clock_UpdateTimebaseAfterFB(void);
#endif

#endif

