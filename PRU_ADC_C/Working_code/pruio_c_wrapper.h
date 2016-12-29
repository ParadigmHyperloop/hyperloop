/** \file pruio_c_wrapper.h
\brief The main header code of the C wrapper for libpruio.

This file provides the declarations of the \ref PruIo class member
functions in C syntax. It also can be used to create language bindings
for non-C languages to use libpruio in polyglot applications.

\note The includes pruio.h and pruio_pins.h are not shown in this
      documentation because they confuse Doxygen. It mixes up
      references with the original headers pruio/pruio.bi and
      pruio/pruio_pins.bi.

Licence: LGPLv2 (http://www.gnu.org/licenses/lgpl-2.0.html)

Copyright 2014 by Thomas{ dOt ]Freiherr[ At ]gmx[ DoT }net

*/

//#include "../pruio/pruio.hp"
#define PRUIO_BALL_AZ   109
#define PRUIO_BALL_OFFS 256

#define PRUIO_GPIO_AZ   3
#define PRUIO_GPIO_OFFS 64
#define PRUIO_GPIO_DATA 24
#define PRUIO_GPIO_BLCK 32

#define PRUIO_ADC_OFFS  512
#define PRUIO_ADC_DATA  228
#define PRUIO_ADC_EMPTY 12
#define PRUIO_ADC_BUFF  16
#define PRUIO_TRG_PARA  4

#define PRUIO_IRPT 25

#define PRUIO_MSG_INIT_OK   4294967295
#define PRUIO_MSG_EXIT_OK   4294967294
#define PRUIO_MSG_ADC_ERRR  4294967293
#define PRUIO_MSG_IO_OK     4294967292
#define PRUIO_MSG_MM_WAIT   4294967291
#define PRUIO_MSG_MM_TRG1   4294967290
#define PRUIO_MSG_MM_TRG2   4294967289
#define PRUIO_MSG_MM_TRG3   4294967288
#define PRUIO_MSG_MM_TRG4   4294967287


//#include "../pruio/pruio.bi" (transformed)
//! tell pruss_intc_mapping.bi that we use ARM33xx
#define AM33XX

//! the PRUSS driver library
#include "prussdrv.h"
//! PRUSS driver interrupt settings
#include "pruss_intc_mapping.h"

//! the channel for PRU messages (must match PRUIO_IRPT)
#define PRUIO_CHAN   CHANNEL5
//! the mask to enable PRU interrupts (must match PRUIO_IRPT)
#define PRUIO_MASK   PRU_EVTOUT5_HOSTEN_MASK
//! the event for PRU messages (mapping, must match PRUIO_IRPT)
#define PRUIO_EMAP   PRU_EVTOUT5
//! the event for PRU messages (must match PRUIO_IRPT)
#define PRUIO_EVNT   PRU_EVTOUT_5

//! the default setting for avaraging
#define PRUIO_DEF_AVRAGE   4
//! the default value for open delay in channel settings
#define PRUIO_DEF_ODELAY   183
//! the default value for sample delay in channel settings
#define PRUIO_DEF_SDELAY   0
//! the default PRU subsystem to use
#define PRUIO_DEF_PRUSS    1
//! the default number of samples to use (configures single mode)
#define PRUIO_DEF_SAMPLS   0
//! the default step mask (steps 1 to 8 for AIN0 to AIN7, no charge step)
#define PRUIO_DEF_STPMSK   0x1FE /* &b111111110 */
//! the default timer value (sampling rate)
#define PRUIO_DEF_TIMERV   0
//! the default bit mode (4 = 16 bit encoding)
#define PRUIO_DEF_LSLMOD   4
//! the default clock divisor (0 = full speed AFE = 2.4 MHz)
#define PRUIO_DEF_CLKDIV   0

//! convenience macro to configure an input pin without pullup or pulldown
#define PRUIO_IN    0x2F /*   &b00101111 */
//! convenience macro to configure an low input pin (with pulldown resistor)
#define PRUIO_IN_0  0x27 /*   &b00100111 */
//! convenience macro to configure an high input pin (with pullup resistor)
#define PRUIO_IN_1  0x37 /*   &b00110111 */
//! convenience macro to configure an output pin initial low
#define PRUIO_OUT0   0xF /*   &b00001111 */
//! convenience macro to configure an output pin initial high
#define PRUIO_OUT1  0x8F /*   &b10001111 */
//! convenience macro to re-configure a pin to its original (initial) state
#define PRUIO_PIN_RESET 0xFF /*  &b11111111 */

enum PRUIO_PIN_LOCKING{
  PRUIO_UNLOCK_CURR, //!< unlock a pin, don't change the configuration
  PRUIO_UNLOCK_ORIG, //!< unlock a pin, change to original (initial) configuration
  PRUIO_UNLOCK_NEW,  //!< unlock a pin, change to parameter configuration
  PRUIO_LOCK_CURR,   //!< lock a pin, don't change the configuration
  PRUIO_LOCK_ORIG,   //!< lock a pin, change to original (initial) configuration
  PRUIO_LOCK_NEW,    //!< lock a pin, change to parameter configuration
  PRUIO_LOCK_CHECK   //!< don't change pin locking, skip operation if pin is locked
};

typedef unsigned int uint32;   //!< 32 bit unsigned integer data type
typedef unsigned short uint16; //!< 16 bit unsigned integer data type
typedef unsigned char uint8;   //!< 8 bit unsigned integer data type
typedef int int32;              //!< 32 bit signed integer data type
typedef short int16;            //!< 16 bit signed integer data type
typedef signed char int8;      //!< 8 bit signed integer data type

//! user defined type for a step setting
struct AdcSteps{
  uint32 Confg, //!< context for configuration register
          Delay; //!< context for delay register
};

//! user defined type for a GPIO setting
struct GpioSet{
  uint32
    Addr,  //!< base address of GPIO device
    ClrM,  //!< mask to clear pins to low
    SetM,  //!< mask to set pins to high
    OutE,  //!< mask to enable pins for output
    Stat,  //!< current state of pins (mixed)
    StaO,  //!< current state of output pins
    fe1,   //!< future expansion
    fe2;
};

//! user defined type (class) for the driver structure
typedef struct PruIo{

  uint32
    Samples,   //!< number of samples
    TimerVal,  //!< timer value in [ns]
    StepMask;  //!< mask to specify active channels
  uint16
    LslMode,   //!< bit shift modus (0 to 4)
    ClockDiv;  //!< divisor for ADC input clock (register 0x4C)

  uint32
     AdcReg24   //!< ADC register IRQ_STATUS_RAW
    ,AdcReg28   //!< ADC register IRQ_STATUS
    ,AdcReg2C   //!< ADC register IRQENABLE_SET
    ,AdcReg30   //!< ADC register IRQENABLE_CLR
    ,AdcReg34   //!< ADC register IRQWAKEUP
    ,AdcReg38   //!< ADC register DMAENABLE_SET
    ,AdcReg3C   //!< ADC register DMAENABLE_CLR
    ,AdcReg40;  //!< ADC register CRTL (bits 0, 1 and 2 gets set by driver)

//! step configuration (charge step + 16 steps + idle step, by default steps 1 to 8 are used for AIN0 to AIN7)
  struct AdcSteps St_p[17 + 1];

  uint32* DRam;    //!< pointer to access PRU DRam
  void *ERam,      //!< pointer to read PRU external ram
     *BallOrg;     //!< buffer pointer for original Ball configuration
  uint32* AdcOrg,  //!< buffer pointer for original ADC configuration (= 0 in case of ADC down at startup)
         *GpioOrg; //!< buffer pointer for original GPIO configuration
  uint16* Value;   //!< pointer to read sampled ADC input data
  uint8* BallRef,  //!< pointer to ball reference (GPIO# for CPU pins or lock)
        *BallConf; //!< pointer to ball configuration (CPU pin muxing)
  char* Errr;      //!< pointer for error messages
  uint32
    EOffs,      //!< the offset of the external PRUSS-DDR
    ESize,      //!< the size of the external PRUSS-DDR
    CSize,      //!< the size of the original configuration block
    PruNo,      //!< the PRU number to use
    PruEvtout,  //!< the interrupt channel to send commands to PRU
    PruIRam,    //!< the PRU instruction ram to load
    PruDRam;    //!< the PRU data ram
  int16
    ArmPruInt,  //!< the interrupt to send
    ChAz;       //!< number of active channels

//! interrupt settings (we also set default interrupts, so that the other PRUSS can be used in parallel)
  struct __pruss_intc_initdata IntcInit;
  struct GpioSet* Gpio; //!< pointer to GPIO structure (current values)
  struct GpioSet GpioInit[PRUIO_GPIO_AZ + 1]; //!< initial GPIO definitions
  uint8 BallGpio[PRUIO_BALL_AZ + 1]; //!< list of GPIO numbers, corresponding to ball index

} PruIo;


/** \brief C wrapper to create a \ref PruIo instance.
\param Av avaraging for default steps (0 to 16, defaults to 0)
\param OpD open delay for default steps (0 to 0x3FFFF, defaults to 0x98)
\param SaD sample delay for default steps (0 to 255, defaults to 0)
\param Pru number of PRU subsystem to use (0 or 1, defaults to 1)
\returns A pointer for the new instance.

This function wraps the constructor, check \ref PruIo::PruIo() for
details.

Since the constructor reads the original devices configurations and the
destructor restores them, it's recommended to create and use just one
\ref PruIo instance at the same time.

*/
PruIo* pruio_new(uint8 Av, uint32 OpD, uint8 SaD, uint8 Pru);

/** \brief C wrapper to destroy a \ref PruIo instance.
\param Io The pointer of the instance.

This function wraps the destructor, check \ref PruIo::~PruIo() for
details. (Doxygen should create a link here?!)

*/
void pruio_destroy(PruIo* Io);

/** \brief C wrapper for function \ref PruIo::config().
\param Io The pointer of the \ref PruIo instance
\param Samp number of samples to fetch (defaults to zero)
\param Mask mask for active steps (defaults to all 8 channels active in steps 1 to 8)
\param Tmr timer value in [ns] to specify the sampling rate (defaults to zero, MM only)
\param Mds modus for output (defaults to 4 = 16 bit)
\param ClDv divisor for ADC input clock (defaults to 0 = full speed = 2.4 MHz)
\returns zero on success (otherwise a string with an error message)

This function wraps the member function config, check \ref
PruIo::config() for details.

*/
char* pruio_config(PruIo* Io, uint32 Samp, uint32 Mask, uint32 Tmr, uint16 Mds, uint16 ClDv);

/** \brief C wrapper for function \ref PruIo::adc_step().
\param Io The pointer of the \ref PruIo instance
\param Stp step index (0 = step 0 => charge step, 1 = step 1 (=> AIN0 by default),  ..., 17 = idle step)
\param ChN channel number to scan (0 = AIN0, 1 = AIN1, ...)
\param Av new value for avaraging (defaults to 4)
\param SaD new value for sample delay (defaults to 0)
\param OpD new value for open delay (defaults to 0x98)
\returns zero on success (otherwise a string with an error message)

This function wraps the member function adc_step, check \ref
PruIo::adc_step() for details.

*/
char* pruio_adc_step(PruIo* Io, uint8 Stp, uint8 ChN, uint8 Av, uint8 SaD, uint32 OpD);

/** \brief C wrapper for function \ref PruIo::gpio_get_config().
\param Io The pointer of the \ref PruIo instance
\param Ball the CPU ball number to describe
\returns a human-readable text string (internal string, never free it)

This function wraps the member function gpio_get_config, check \ref
PruIo::gpio_get_config() for details.

*/
char* pruio_gpio_get_config(PruIo* Io, uint8 Ball);

/** \brief C wrapper for function \ref PruIo::gpio_set().
\param Io The pointer of the \ref PruIo instance
\param Ball the CPU ball number to set
\param Modus the mode for the GPIO
\param Lokk the locking specification
\returns zero on success (otherwise a pionter to an error message)

This function wraps the member function gpio_set, check \ref
PruIo::gpio_set() for details.

*/
char* pruio_gpio_set(PruIo* Io, uint8 Ball, uint8 Modus, uint8 Lokk);

/** \brief C wrapper for function \ref PruIo::gpio_get().
\param Io The pointer of the \ref PruIo instance
\param Ball the CPU ball number to test
\returns GPIO state (otherwise -1, check \ref PruIo::Errr for an error message)

This function wraps the member function gpio_get, check \ref
PruIo::gpio_get() for details.

*/
uint32 pruio_gpio_get(PruIo* Io, uint8 Ball);

/** \brief C wrapper for function \ref PruIo::gpio_out().
\param Io The pointer of the \ref PruIo instance
\param Ball the CPU ball number to test
\param Modus the state to set (0 = low, high otherwise)
\returns zero on success (otherwise a pointer to an error message)

This function wraps the member function gpio_out, check \ref
PruIo::gpio_out() for details.

*/
char* pruio_gpio_out(PruIo* Io, uint8 Ball, uint8 Modus);

/** \brief C wrapper for function \ref PruIo::mm_trg_pin().
\param Io The pointer of the \ref PruIo instance
\param Ball the CPU ball number to test
\param GpioV the state to check (defaults to high = 1)
\param Skip the number of samples to skip (defaults to 0 = zero, max. 1023)
\returns the trigger configuration (or zero in case of an error, check \ref PruIo::Errr)

This function wraps the member function mm_trg_pin, check \ref
PruIo::mm_trg_pin() for details.

*/
uint32 pruio_mm_trg_pin(PruIo* Io, uint8 Ball, uint8 GpioV, uint16 Skip);

/** \brief C wrapper for function \ref PruIo::mm_trg_ain().
\param Io The pointer of the \ref PruIo instance
\param Stp the step number to use for trigger input
\param AdcV the sample value to match (positive check greater than, negative check less than)
\param Rela if AdcV is relative to the current input
\param Skip the number of samples to skip (defaults to 0 = zero, max. 1023)
\returns the trigger configuration (or zero in case of an error, check \ref PruIo::Errr)

This function wraps the member function mm_trg_ain, check \ref
PruIo::mm_trg_ain() for details.

*/
uint32 pruio_mm_trg_ain(PruIo* Io, uint8 Stp, int32 AdcV, uint8 Rela, uint16 Skip);

/** \brief C wrapper for function \ref PruIo::mm_trg_pre().
\param Io The pointer of the \ref PruIo instance
\param Stp the step number to use for trigger input
\param AdcV the sample value to match (positive check greater than, negative check less than)
\param Samp the number of samples for the pre-trigger
\param Rela if AdcV is relative to the current input
\returns the trigger configuration (or zero in case of an error, check \ref PruIo::Errr)

This function wraps the member function mm_trg_pre, check \ref
PruIo::mm_trg_pre() for details.

*/
uint32 pruio_mm_trg_pre(PruIo* Io, uint8 Stp, int32 AdcV, uint16 Samp, uint8 Rela);

/** \brief C wrapper for function \ref PruIo::mm_start().
\param Io The pointer of the \ref PruIo instance
\param Trg1 settings for first trigger (default = no trigger)
\param Trg2 settings for second trigger (default = no trigger)
\param Trg3 settings for third trigger (default = no trigger)
\param Trg4 settings for fourth trigger (default = no trigger)
\returns zero on success (otherwise a string with an error message)

This function wraps the member function mm_start, check \ref
PruIo::mm_start() for details.

*/
char* pruio_mm_start(PruIo* Io, uint32 Trg1, uint32 Trg2, uint32 Trg3, uint32 Trg4);
