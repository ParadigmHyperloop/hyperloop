#ifndef OPENLOOP_POD_EMERGENCY_H
#define OPENLOOP_POD_EMERGENCY_H
#include "../pod.h"
#include "states.h"

/**
 * WARNING! TAKE EXTREME CARE IN WHERE YOU USE THIS! THIS WILL KILL THE PROG!
 *
 * Recommend instead:
 *
 *  set_pod_mode(Emergency, "Explain what happened");
 *
 * or the wrapper macro for set_pod_mode(Emergency) which adds file/line info
 *
 *  DECLARE_EMERGENCY("Explain what happened");
 *
 * Declare an immediate panic and exit. This will kill the controller, Logging
 * server, command server, and the entire process.
 *
 * The panic flow is as follows
 *   - Print the given panic notes to stderr ONLY
 *   - Issue a POD_SIGPANIC to own pid or just exit(POD_EX_PANIC)
 *   - The safety wrapper script should immediately attempt to set the CTRL_OK
 *     GPIO to LOW using the linux /sys/class/gpio tree signalling the
 *     Emergency Board that the controller crashed
 *   - The safety wrapper script will attempt to deenergize the Ebrake
 *     solenoids if possible but the Emergency Board should also open the
 *     Emergency brake solenoids as well when CTRL_OK drops LOW
 */
void pod_panic(int subsystem, char *file, int line, char *notes, ...);

/**
 * Set the given caution bit in the pod indicating a cautionary Alert
 */
void set_caution(pod_warning_t caution, pod_t *pod);

/**
 * Set the given warning bit in the pod indicating a warning Alert
 */
void set_warn(pod_warning_t warning, pod_t *pod);

/**
 * Clear the given caution bit in the pod cancelling a cautionary Alert
 */
void clear_caution(pod_warning_t caution, pod_t *pod);

/**
 * Clear the given warning bit in the pod cancelling a warning Alert
 */
void clear_warn(pod_warning_t warning, pod_t *pod);

/**
 * Check if the given caution bit is set in the pod
 */
bool has_caution(pod_caution_t caution, pod_t *pod);

/**
 * Check if the given warning bit is set in the pod
 */
bool has_warning(pod_warning_t warning, pod_t *pod);

#endif
