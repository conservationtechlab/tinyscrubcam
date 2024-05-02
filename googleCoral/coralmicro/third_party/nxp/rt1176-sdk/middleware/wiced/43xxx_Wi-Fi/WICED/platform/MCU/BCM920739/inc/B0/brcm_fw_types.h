//==================================================================================================
//                        THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP
//--------------------------------------------------------------------------------------------------
//                               Copyright (c) 2011 Broadcom Corp.
//                                      ALL RIGHTS RESERVED
//==================================================================================================
//! \file
//!
//! Defines basic but size-specific types, and prohibits the use of certain constructs.
//
//==================================================================================================


#ifndef _BRCM_FW_TYPES_H_
#define _BRCM_FW_TYPES_H_


#include <stddef.h>
#include <assert.h>

//==================================================================================================
// Prefix header for build flags.  This will be created by the build process, in the build output
// folder, using flags extracted from the build/chips/chip_features.xml spreadsheet, and possibly
// overridden from the command line.
//==================================================================================================
#include "auto_flags.h"
// extern C core includes
#ifdef __cplusplus
extern "C" {
#endif

//==================================================================================================
// Types
//==================================================================================================

//! Unsigned 8-bit integer.
typedef unsigned char UINT8;

//! Signed 8-bit integer.
typedef signed char INT8;

//! Unsigned 16-bit integer.
typedef unsigned short int UINT16;

//! Signed 16-bit integer.
typedef signed short int INT16;

//! Unsigned 32-bit integer.
typedef unsigned long UINT32;
// NOTE: not long int, because on 64-bit compilers (and more to the point, coverity running on
// 64-bit linux), long int is treated as 64 bits.

//! Signed 32-bit integer.
typedef signed long INT32;
// NOTE: not long int, because on 64-bit compilers (and more to the point, coverity running on
// 64-bit linux), long int is treated as 64 bits.

//! Unsigned 64-bit integer.
typedef unsigned long long int UINT64;

//! Signed 64-bit integer.
typedef signed long long int INT64;

//! Byte type (unsigned 8-bit integer).
typedef unsigned char BYTE;

//! Boolean type in its most efficient form, for use in function arguments and return values.
typedef unsigned int BOOL32;

//! Boolean type in its most size-efficient form, for use in structures.
typedef unsigned char BOOL8;


//==================================================================================================
// Properties of the CPU
//==================================================================================================
#if defined(CPU_CM3) || defined(CPU_CM4)
    //! Flag indicating that the CPU is little endian.  That means that words that are more than 8
    //! bits have the 8 least significant bits in the lowest byte address of the word.  If it was a
    //! big endian CPU (having the most significant 8 bits in the first byte of a word),
    //! CPU_BIG_ENDIAN would have been defined.
    #define CPU_LITTLE_ENDIAN           1
    //no:   CPU_BIG_ENDIAN

    //! Even though the CM3 CPU doesn't directly allow unaligned access (e.g. reading a 32-bit word
    //! from an address that is not aligned to a 32-bit boundary) the AHB adaptor from the CM3 will
    //! perform multiple reads or writes to facilitate unaligned access.  The same is not true for
    //! ARM7, however.
    #define CPU_ALLOW_UNALIGNED_ACCESS  1
#else
    #error "Target CPU properties not defined"
#endif


//==================================================================================================
// Macro for access to hardware registers
//==================================================================================================

//! The REG32 macro is a convenient way to access a 32-bit hardware register.  It merely casts the
//! integer register address to a pointer to a volatile UINT32 and dereferences it.
#define REG32(address)  ( *(volatile UINT32*)(address) )


//==================================================================================================
// Constants
//==================================================================================================
#if !(defined TRUE && defined FALSE)

//! Macro representing a boolean true condition.  Note that it is appropriate to return TRUE, but
//! not appropriate to compare against it because any non-zero value can legitimately be used to
//! represent truth, especially in optimized code.  Instead of if(condition == TRUE) one should
//! simply use if(condition).
#define TRUE    1

//! Macro representing a boolean false condition.
#define FALSE   0

#endif


//==================================================================================================
// Qualifiers
//==================================================================================================

//! A pseudo-qualifier on a pointer argument indicating that the parameter is an input, not output.
//! If the pointer can't be const, then it must be INOUT or TAKES.  All pointer parameters must be
//! IN, OUT, INOUT, or TAKES.
#define IN const

//! A pseudo-qualifier on a pointer argument indicating that the parameter is an output, not input.
//! In such a case, the value before the call is irrelevant to the function and is ignored.  If the
//! function does use the input and modifies it, then the pointer should be INOUT.  All pointer
//! parameters must be IN, OUT, INOUT, or TAKES.
#define OUT

//! A pseudo-qualifier on a pointer argument indicating that the parameter is an input and output.
//! All pointer parameters must be IN, OUT, INOUT, or TAKES.  In a few cases, a function takes an
//! input which it modifies but INOUT doesn't make sense if the function never returns.  In such a
//! case, the logical conclusion is typically that the function TAKES the input permanently.
#define INOUT

//! A pseudo-qualifier on a pointer argument to take possession from the caller.  The function takes
//! possession of, and responsibility for freeing, the dynamically allocated block passed in a
//! parameter with this qualifier.  Such a function must indicate in its documentation what
//! allocation mechanism must have been used to allocate the block.  From the caller's perspective,
//! The object pointed to no longer exists after the call, since it may have been released already.
//! All pointer parameters must be IN, OUT, INOUT, or TAKES.  In a few cases, a function takes an
//! input which it modifies but INOUT doesn't make sense if the function never returns.  In such a
//! case, the logical conclusion is typically that the function TAKES the input permanently.
#define TAKES

//! A pseudo-qualifier on a pointer returned from a function to give ownership to the caller.  The
//! caller must takes possession of, and responsibility for freeing, the dynamically allocated block
//! returned by the function.  Such a function must indicate in its documentation what allocation
//! mechanism was used, and thus what mechanism must be used to free it.
#define GIVES

//! A pseudo-qualifier to make static functions and variables to non static.
#ifndef MAY_BE_STATIC
#define MAY_BE_STATIC
#endif

//==================================================================================================
// Ubiquitous utility macros
//==================================================================================================

//! Macro to divide numerator by denominator, rounding up.
#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP( n, d )                                    ( ( (n) + (d)-1 ) / (d) )
#endif

//! Macro to divide numerator by denominator, rounding to the nearest number.
#define DIV_ROUND_NEAREST( n, d )                               ( ( (n) + (d)/2 ) / (d) )

//! Macro to obtain the absolute value of an integer.
#define ABS(v)                                                  ( ( (v) < 0 ) ? ( 0-(v) ) : (v) )

//! Macro to obtain the minimum (smaller) of two numbers.
#ifndef MIN
#define MIN( a, b )                                             ( ( (a) < (b) ) ? (a) : (b) )
#endif

//! Macro to obtain the maximum (larger) of two numbers.
#ifndef MAX
#define MAX( a, b )                                             ( ( (a) > (b) ) ? (a) : (b) )
#endif

//! Macro to swap the endianness of a 16 bit value.
#define SWAP_ENDIAN_16(x)       ((((x) << 8) | (((x) >> 8) & 0xFF)))

//! Macro to swap the endianness of a 32 bit value.
#define SWAP_ENDIAN_32(x)  (    (((x) & 0xFF000000) >> 24) | (((x) & 0x00FF0000) >> 8) | \
                                (((x) & 0x0000FF00) << 8)  | (((x) & 0x000000FF) << 24)     )

//! Macro to swap the endianness of a 64 bit value.
#define SWAP_ENDIAN_64(x)                                               \
    (   (((x) >> 56) & 0x00000000000000FFULL)  | (((x) >> 40) & 0x000000000000FF00ULL) | \
        (((x) >> 24) & 0x0000000000FF0000ULL)  | (((x) >> 8)  & 0x00000000FF000000ULL) | \
        (((x) << 8)  & 0x000000FF00000000ULL)  | (((x) << 24) & 0x0000FF0000000000ULL) | \
        (((x) << 40) & 0x00FF000000000000ULL)  | (((x) << 56) & 0xFF00000000000000ULL)      )

#if CPU_LITTLE_ENDIAN

//! Macro to convert a 16 bit little endian value to the CPU's endianness. For a little endian
//! processor, the value is the same.
#define LITTLE_TO_CPU_ENDIAN_16(x)          (x)

//! Macro to convert a 16 bit value from CPU endianness to little endian. For a little endian
//! processor, the value is the same.
#define CPU_TO_LITTLE_ENDIAN_16(x)          (x)

//! Macro to convert a 16 bit big endian value to the CPU's endianness. For a little endian
//! processor, swap the two bytes.
#define BIG_TO_CPU_ENDIAN_16(x)             SWAP_ENDIAN_16(x)

//! Macro to convert a 16 bit value from CPU endianness to big endian. For a little endian
//! processor, swap the two bytes.
#define CPU_TO_BIG_ENDIAN_16(x)             SWAP_ENDIAN_16(x)

//! Macro to convert a 32 bit little endian value to the CPU's endianness. For a little endian
//! processor, the value is the same.
#define LITTLE_TO_CPU_ENDIAN_32(x)          (x)

//! Macro to convert a 32 bit value from CPU endianness to little endian. For a little endian
//! processor, the value is the same.
#define CPU_TO_LITTLE_ENDIAN_32(x)          (x)

//! Macro to convert a 32 bit big endian value to the CPU's endianness. For a little endian
//! processor, swap the four bytes.
#define BIG_TO_CPU_ENDIAN_32(x)             SWAP_ENDIAN_32(x)

//! Macro to convert a 32 bit value from CPU endianness to big endian. For a little endian
//! processor, swap the four bytes.
#define CPU_TO_BIG_ENDIAN_32(x)             SWAP_ENDIAN_32(x)

//! Macro to convert a 64 bit little endian value to the CPU's endianness. For a little endian
//! processor, the value is the same.
#define LITTLE_TO_CPU_ENDIAN_64(x)          (x)

//! Macro to convert a 64 bit value from CPU endianness to little endian. For a little endian
//! processor, the value is the same.
#define CPU_TO_LITTLE_ENDIAN_64(x)          (x)

//! Macro to convert a 64 bit big endian value to the CPU's endianness. For a little endian
//! processor, swap the four bytes.
#define BIG_TO_CPU_ENDIAN_64(x)             SWAP_ENDIAN_64(x)

//! Macro to convert a 64 bit value from CPU endianness to big endian. For a little endian
//! processor, swap the four bytes.
#define CPU_TO_BIG_ENDIAN(x)                SWAP_ENDIAN_64(x)

#else   // CPU_LITTLE_ENDIAN - must be big-endian

//! Macro to convert a 16 bit little endian value to the CPU's endianness. For a big endian
//! processor, swap the two bytes.
#define LITTLE_TO_CPU_ENDIAN_16(x)          SWAP_ENDIAN_16(x)

//! Macro to convert a 16 bit value from CPU endianness to little endian. For a big endian
//! processor, swap the two bytes.
#define CPU_TO_LITTLE_ENDIAN_16(x)          SWAP_ENDIAN_16(x)

//! Macro to convert a 16 bit big endian value to the CPU's endianness. For a big endian
//! processor, the value is the same.
#define BIG_TO_CPU_ENDIAN_16(x)             (x)

//! Macro to convert a 16 bit value from CPU endianness to big endian. For a big endian
//! processor, the value is the same.
#define CPU_TO_BIG_ENDIAN_16(x)             (x)

//! Macro to convert a 32 bit little endian value to the CPU's endianness. For a big endian
//! processor, swap the four bytes.
#define LITTLE_TO_CPU_ENDIAN_32(x)          SWAP_ENDIAN_32(x)

//! Macro to convert a 32 bit value from CPU endianness to little endian. For a big endian
//! processor, swap the four bytes.
#define CPU_TO_LITTLE_ENDIAN_32(x)          SWAP_ENDIAN_32(x)

//! Macro to convert a 32 bit big endian value to the CPU's endianness. For a big endian
//! processor, the value is the same.
#define BIG_TO_CPU_ENDIAN_32(x)             (x)

//! Macro to convert a 32 bit value from CPU endianness to big endian. For a big endian
//! processor, the value is the same.
#define CPU_TO_BIG_ENDIAN_32(x)             (x)

//! Macro to convert a 64 bit little endian value to the CPU's endianness. For a big endian
//! processor, swap the four bytes.
#define LITTLE_TO_CPU_ENDIAN_64(x)          SWAP_ENDIAN_64(x)

//! Macro to convert a 64 bit value from CPU endianness to little endian. For a big endian
//! processor, swap the four bytes.
#define CPU_TO_LITTLE_ENDIAN_64(x)          SWAP_ENDIAN_64(x)

//! Macro to convert a 64 bit big endian value to the CPU's endianness. For a big endian
//! processor, the value is the same.
#define BIG_TO_CPU_ENDIAN_64(x)             (x)

//! Macro to convert a 64 bit value from CPU endianness to big endian. For a big endian
//! processor, the value is the same.
#define CPU_TO_BIG_ENDIAN_64(x)             (x)

#endif


//==================================================================================================
// Ubiquitous debugging macros
//==================================================================================================

//! Macro to catch asserts during compilation time
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

#ifdef DEBUG_FRAMEWORK

    #define FATAL(msg)              dbfw_assert_fatal(FID, __LINE__)
    #define FATAL1(msg, arg1)       dbfw_assert_fatal1(FID, __LINE__, arg1)
    #define FATAL2(msg, arg1, arg2) dbfw_assert_fatal2(FID, __LINE__, arg1, arg2)

    extern void dbfw_assert_fatal(UINT16 fid, UINT16 line );
    extern void dbfw_assert_fatal1(UINT16 fid, UINT16 line, UINT32 arg1);
    extern void dbfw_assert_fatal2(UINT16 fid, UINT16 line, UINT32 arg1, UINT32 arg2);

#if DBFW_ASSERT_N_ERROR_ENABLE==1
    #define ASSERT(expr)            ( (expr) ? (void)0 : dbfw_assert_alert(FID, __LINE__) )
    #define ASSERT1( expr, arg1 )   ( (expr) ? (void)0 : dbfw_assert_alert1( FID, __LINE__, arg1 ) )
    #define ASSERT2( expr, arg1, arg2 )  ( (expr) ? (void)0 : dbfw_assert_alert2( FID, __LINE__, arg1, arg2 ) )
    #define ERROR(msg)              dbfw_assert_error(FID, __LINE__)
    #define ERROR1(msg, arg1)       dbfw_assert_error1(FID, __LINE__, arg1)
    #define ERROR2(msg, arg1, arg2) dbfw_assert_error2(FID, __LINE__, arg1, arg2)

    extern void dbfw_assert_alert(UINT16 fid, UINT16 line );
    extern void dbfw_assert_alert1(UINT16 fid, UINT16 line, UINT32 arg1);
    extern void dbfw_assert_alert2(UINT16 fid, UINT16 line, UINT32 arg1, UINT32 arg2);

    extern void dbfw_assert_error(UINT16 fid, UINT16 line );
    extern void dbfw_assert_error1(UINT16 fid, UINT16 line, UINT32 arg1);
    extern void dbfw_assert_error2(UINT16 fid, UINT16 line, UINT32 arg1, UINT32 arg2);

#else
    #define ASSERT(expr)            
    #define ASSERT1( expr, arg1 )   
    #define ASSERT2( expr, arg1, arg2 )
    #define ERROR(msg)              
    #define ERROR1(msg, arg1)       
    #define ERROR2(msg, arg1, arg2) 
#endif

#else

//! In debug builds, asserts that the expression e must be true.  If the expression is not true,
//! locks out interrupts and outputs verbose diagnostic information.
#if DEBUG
    #define FATAL(msg)                  debug_FatalErrorMessage2( __FILE__, __LINE__, msg, NULL )
    #define FATAL1(msg, arg1)           debug_FatalErrorMessage2( __FILE__, __LINE__, msg, NULL )
    #define FATAL2(msg, arg1, arg2)     debug_FatalErrorMessage2( __FILE__, __LINE__, msg, NULL )
    #define ASSERT(expr)    ( (expr) ? (void)0 : debug_FatalError( __FILE__, __LINE__ ) )
    #define ASSERT1( expr, arg1 ) ( (expr) ? (void)0 : debug_FatalErrorValue( __FILE__, __LINE__, arg1 ) )
    #define ASSERT2( expr, arg1, arg2 ) ( (expr) ? (void)0 : debug_FatalErrorValue( __FILE__, __LINE__, arg1 ) )
    #define ERROR(msg)                  debug_FatalErrorMessage2( __FILE__, __LINE__, msg, NULL )
    #define ERROR1( msg, arg1 )         debug_FatalErrorMessage2( __FILE__, __LINE__, msg, NULL )
    #define ERROR2( msg, arg1, arg2 )   debug_FatalErrorMessage2( __FILE__, __LINE__, msg, NULL )
#else
    #define FATAL(msg) 
    #define FATAL1(msg, arg1)
    #define FATAL2(msg, arg1, arg2)
    #define ASSERT(expr)          
    #define ASSERT1(expr, arg1)      
    #define ERROR(msg)                  
    #define ERROR2( msg1, arg1, arg2 )  
#endif  // ifdef DEBUG

#endif  // ifdef DEBUG_FRAMEWORK

//! Implemenation behind the ASSERT macro.
extern void debug_FatalError( IN char* file, int line );

//! Implemenation behind the ASSERT1 macro.
extern void debug_FatalErrorValue( IN char* file, int line, unsigned int value );

//! Implemenation behind the FATAL and FATAL2 macros.
extern void debug_FatalErrorMessage2(   IN char* file, int line, IN char* msg1, IN char* msg2 );
    
//==================================================================================================
// Toolchain-specific stuff
//==================================================================================================

//! ARM RVDS-specific struct qualifier for structs inside a pragma pack(1) / pragma pack() block.
//! It must be applied to ALL structs or unions in such a block of declarations as follows:
//! BYTE_PACKED struct or BYTE_PACKED union.
#if TOOLCHAIN_rvds
    #define BYTE_PACKED __packed

    #if ALWAYS_ON_MEMORY_SUPPORT
        //! ARM RVDS-specific attribute to place in always-on-memory. This must be applied to all
        //! RW/ZI that is expected to be retained in 'persistant' memory. This memory will be custom
        //! scatter-loaded.
        #define PLACE_IN_ALWAYS_ON_RAM  __attribute__((section("always_on_ram_var")))
        #define PLACE_IN_ALWAYS_ON_UNINIT_RAM  __attribute__((section("always_on_uninit_ram_var")))
        #define PLACE_IN_LIMITED_RAM PLACE_IN_ALWAYS_ON_RAM  
        #define PLACE_IN_ALWAYS_ON_TEXT  __attribute__((section(".aon_text")))
    #else
        //! If always on memory is not supported, we don't need this at all, place anywhere.
        #define PLACE_IN_ALWAYS_ON_RAM
        #define PLACE_IN_ALWAYS_ON_UNINIT_RAM
        #define PLACE_IN_LIMITED_RAM
    #endif
#elif defined(TOOLCHAIN_wiced)
	#define BYTE_PACKED

	#if ALWAYS_ON_MEMORY_SUPPORT
		//! ARM RVDS-specific attribute to place in always-on-memory. This must be applied to all
		//! RW/ZI that is expected to be retained in 'persistant' memory. This memory will be custom
		//! scatter-loaded.
		#define PLACE_IN_ALWAYS_ON_RAM  __attribute__((section("always_on_ram_var")))
		#define PLACE_IN_ALWAYS_ON_UNINIT_RAM  __attribute__((section("always_on_uninit_ram_var")))
		#define PLACE_IN_LIMITED_RAM PLACE_IN_ALWAYS_ON_RAM
	#else
		//! If always on memory is not supported, we don't need this at all, place anywhere.
		#define PLACE_IN_ALWAYS_ON_RAM
		#define PLACE_IN_ALWAYS_ON_UNINIT_RAM
		#define PLACE_IN_LIMITED_RAM
	#endif
#endif


//==================================================================================================
// Prohibitions
//==================================================================================================

//! bool should not be used, so it is defined so as to generate a compilation error.  This is in
//! order to enforce the practice of using BOOL32 for efficiency in function arguments or return
//! values, and BOOL8 for size efficiency in structures.  The same is true of BOOL and BOOLEAN,
//! which are ambiguous in the size vs. efficiency tradeoff and should not be used.
//#ifndef __cplusplus
//#define bool        DO NOT USE bool
//#endif
//! BOOL should not be used, so it is defined so as to generate a compilation error.  This is in
//! order to enforce the practice of using BOOL32 for efficiency in function arguments or return
//! values, and BOOL8 for size efficiency in structures.  The same is true of bool and BOOLEAN,
//! which are ambiguous in the size vs. efficiency tradeoff and should not be used.
//#define BOOL        DO NOT USE BOOL

//! BOOLEAN should not be used, so it is defined so as to generate a compilation error.  This is in
//! order to enforce the practice of using BOOL32 for efficiency in function arguments or return
//! values, and BOOL8 for size efficiency in structures.  The same is true of bool and BOOL, which
//! are ambiguous in the size vs. efficiency tradeoff and should not be used.
//#define BOOLEAN     DO NOT USE BOOLEAN

#undef assert
//! assert should not be used, so it is defined so as to generate a compilation error.  This is
//! because in the event of failure, we want to invoke our own debug facilities for robust
//! diagnostic output.  Use ASSERT instead.
#define assert      DO NOT USE assert

#ifndef  INLINE 
#define INLINE inline
#endif


#ifdef __cplusplus
}
#endif

#endif // _BRCM_FW_TYPES_H_

