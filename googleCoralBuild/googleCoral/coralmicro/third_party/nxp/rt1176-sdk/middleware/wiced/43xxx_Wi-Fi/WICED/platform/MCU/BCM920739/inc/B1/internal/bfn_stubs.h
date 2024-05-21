#ifndef _BFN_STUBS_H_
#define _BFN_STUBS_H_
#ifdef __cplusplus
extern "C"
{
#endif

void pmu_selectEnhancedLPOSource( void ); // jacek sent email on 10/18 about what to do

// note: this structure must match typdef PMU_LPO_CLK_SOURCE
typedef enum MPAF_PMU_LPO_CLK_SOURCE_TAG
{
    MPAF_LPO_CLK_INTERNAL,    // default
    MPAF_LPO_CLK_EXTERNAL,
    MPAF_LPO_CLK_CRYSTAL,
    MPAF_LPO_NO_SELECTED,
    MPAF_LPO_32KHZ_OSC,
    MPAF_LPO_MIA_LPO,
} MPAF_LPO_CLK_SOURCE;

// Joby may want to move this to devicelpm. Driver will then use it.
enum
{
    // in cr_wake_int_en_4_adr
    MPAF_PMU_WAKE_SOURCE_GPIO = ( 1 << 0 ),
    // Wake source GPIO and LHL are the same
    MPAF_PMU_WAKE_SOURCE_LHL = MPAF_PMU_WAKE_SOURCE_GPIO,
    // in cr_wake_int_en_2_adr
    MPAF_PMU_WAKE_SOURCE_KEYSCAN = ( 1 << 30 ),
    // cr_wake_int_en_2_adr
    MPAF_PMU_WAKE_SOURCE_QUAD = (int) ( 1UL << 31 )
};

typedef UINT32 (*CFA_GET_TIME_TO_SLEEP_FP)( void );

#ifdef __cplusplus
}
#endif

#endif
