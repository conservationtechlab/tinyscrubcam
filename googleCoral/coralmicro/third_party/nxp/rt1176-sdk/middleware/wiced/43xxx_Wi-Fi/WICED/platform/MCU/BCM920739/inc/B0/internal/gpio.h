#ifndef _GPIO_H_
#define _GPIO_H_


#include "brcm_fw_types.h"

//! TYPE DEFINITIONS

typedef void GPIO_ISR_CALLBACK(void);

typedef struct
{
    volatile UINT32  DataDirection;
    volatile UINT32  IntSense;
    volatile UINT32  IntBothEdges;
    volatile UINT32  IntEvent;
    volatile UINT32  IntEnableMask;
    volatile UINT32  RawIntStatus;
    volatile UINT32  MaskedIntStatus;
    volatile UINT32  IntClear;
    volatile UINT32  ModeCtrlSel;
} GPIO_REG_INTERFACE;

typedef enum
{
    EDGE_RISING,
    EDGE_FALLING,
    EDGES_BOTH,
    LEVEL_LOW,
    LEVEL_HIGH
} INT_TRIGGER_TYPE;


//! FUNCTION DECLARATIONS

extern void gpio_interrupt_config( UINT32 gpio, INT_TRIGGER_TYPE trigger );

//! GPIO interrupt handler, called by ISR.
extern void gpio_interrupt(UINT32 block);

//! Register/Unregister GPIO interrupt callback
extern void gpio_register_interrupt_handler(UINT32 gpio, GPIO_ISR_CALLBACK* cbk);

//! Enable/Disable interrupt for a GPIO
extern void gpio_interrupt_enable (UINT32 gpio, BOOL32 enable);

//! Set a GPIO Value to 1 or 0
extern void gpio_set(UINT32 gpio, BOOL32 value);

//! Get value of a single GPIO
extern UINT32 gpio_get( UINT32 gpioNum );


extern void gpio_WriteReg( UINT32 gpioNum, UINT32 regOffset, BOOL32 level);
extern void gpio_ReadReg( UINT32 gpioBlockNum, UINT32 regOffset, UINT32 *dataPtr );
extern void gpio_block_get( UINT32 gpioBlockNum, UINT8 *pGpio );


//! MACRO DEFINITIONS.

#define   gpio_block0_get( dataPtr)                            gpio_block_get( 0, dataPtr )
#define   gpio_blockA_get( dataPtr)                            gpio_block_get( 1, dataPtr )
#define   gpio_blockC_get( dataPtr)                            gpio_block_get( 3, dataPtr )
#define   gpio_blockD_get( dataPtr)                            gpio_block_get( 4, dataPtr )

#define   gpio_blockA_set( gpioNum, value )                    gpio_set( (8+gpioNum),  value )
#define   gpio_blockC_set( gpioNum, value )                    gpio_set( (24+gpioNum), value )
#define   gpio_blockD_set( gpioNum, value )                    gpio_set( (32+gpioNum), value )

#define   gpio_set_direction( gpioNum, value )                 gpio_WriteReg( gpioNum,      0, value )
#define   gpioA_set_direction( gpioNum, value )                gpio_WriteReg( (8+gpioNum),  0, value )
#define   gpioC_set_direction( gpioNum, value )                gpio_WriteReg( (24+gpioNum), 0, value )
#define   gpioD_set_direction( gpioNum, value )                gpio_WriteReg( (32+gpioNum), 0, value )

#define   gpio_interrupt_sense( gpioNum, value )               gpio_WriteReg( gpioNum,      1, value )
#define   gpioA_interrupt_sense( gpioNum, value )              gpio_WriteReg( (8+gpioNum),  1, value )
#define   gpioC_interrupt_sense( gpioNum, value )              gpio_WriteReg( (24+gpioNum), 1, value )
#define   gpioD_interrupt_sense( gpioNum, value )              gpio_WriteReg( (32+gpioNum), 1, value )

#define   gpio_interrupt_both_edges( gpioNum, value )          gpio_WriteReg( gpioNum,      2, value )
#define   gpioA_interrupt_both_edges( gpioNum, value )         gpio_WriteReg( (8+gpioNum),  2, value )
#define   gpioC_interrupt_both_edges( gpioNum, value )         gpio_WriteReg( (24+gpioNum), 2, value )
#define   gpioD_interrupt_both_edges( gpioNum, value )         gpio_WriteReg( (32+gpioNum), 2, value )
                     
#define   gpio_interrupt_event( gpioNum, value )               gpio_WriteReg( gpioNum,      3, value )
#define   gpio_interrupt_mask( gpioNum, value )                gpio_WriteReg( gpioNum,      4, value )
#define   gpio_interrupt_clear( gpioNum )                      gpio_WriteReg( gpioNum,      7, TRUE )

#define   gpioA_register_interrupt_handler( gpioNum, callBack ) gpio_register_interrupt_handler( (8+gpioNum), callBack )
#define   gpioC_register_interrupt_handler( gpioNum, callBack ) gpio_register_interrupt_handler( (24+gpioNum), callBack )
#define   gpioD_register_interrupt_handler( gpioNum, callBack ) gpio_register_interrupt_handler( (32+gpioNum), callBack )

#define   gpio_enable_interrupt( gpioNum, enable )             gpio_interrupt_enable( gpioNum, enable )
#define   gpioA_enable_interrupt( gpioNum, enable )            gpio_interrupt_enable( (8+gpioNum), enable )
#define   gpioC_enable_interrupt( gpioNum, enable )            gpio_interrupt_enable( (24+gpioNum), enable )
#define   gpioD_enable_interrupt( gpioNum, enable )            gpio_interrupt_enable( (32+gpioNum), enable )

#define    GPIO2_PCM_OUT       2
#define    GPIO2_PCM_IN        3

#define GPIO_DIRECTION_INPUT                FALSE
#define GPIO_DIRECTION_OUTPUT               TRUE
#define GPIO_INTERRUPT_EDGE_SENSE           FALSE
#define GPIO_INTERRUPT_LEVEL_SENSE          TRUE
#define GPIO_INTERRUPT_BOTH_EDGES_TRIGGER   TRUE
#define GPIO_INTERRUPT_ONE_EDGE             FALSE
#define GPIO_INTERRUPT_LEVEL_SENSE_HIGH     TRUE
#define GPIO_INTERRUPT_LEVEL_SENSE_LOW      FALSE
#define GPIO_INTERRUPT_RISING_EDGES         TRUE
#define GPIO_INTERRUPT_FALLING_EDGES        FALSE
#define GPIO_INTERRUPT_PIN_ENABLE           TRUE
#define GPIO_INTERRUPT_PIN_DISABLE          FALSE

#define    GPIO_PIN_0              0
#define    GPIO_PIN_1              1
#define    GPIO_PIN_2              2
#define    GPIO_PIN_3              3
#define    GPIO_PIN_4              4
#define    GPIO_PIN_5              5
#define    GPIO_PIN_6              6
#define    GPIO_PIN_7              7

#define    GPIO_PIN_A0              8
#define    GPIO_PIN_A1              9
#define    GPIO_PIN_A2              10
#define    GPIO_PIN_A3              11
#define    GPIO_PIN_A4              12
#define    GPIO_PIN_A5              13
#define    GPIO_PIN_A6              14
#define    GPIO_PIN_A7              15

#define    GPIO_PIN_B0              16
#define    GPIO_PIN_B1              17
#define    GPIO_PIN_B2              18
#define    GPIO_PIN_B3              19
#define    GPIO_PIN_B4              20
#define    GPIO_PIN_B5              21
#define    GPIO_PIN_B6              22
#define    GPIO_PIN_B7              23

#define    GPIO_PIN_C0              24
#define    GPIO_PIN_C1              25
#define    GPIO_PIN_C2              26
#define    GPIO_PIN_C3              27
#define    GPIO_PIN_C4              28
#define    GPIO_PIN_C5              29
#define    GPIO_PIN_C6              30
#define    GPIO_PIN_C7              31

#define    GPIO_PIN_D0              32
#define    GPIO_PIN_D1              33
#define    GPIO_PIN_D2              34
#define    GPIO_PIN_D3              35
#define    GPIO_PIN_D4              36
#define    GPIO_PIN_D5              37
#define    GPIO_PIN_D6              38
#define    GPIO_PIN_D7              39

#endif

