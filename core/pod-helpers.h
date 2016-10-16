/**
 * Helper Functions for checking pod state
 */
#include "pod.h"

/**
 * Determine if the Emergency Brakes are engaged based on their braking presure
 */
bool emergencyBrakesEngaged(pod_state_t *state);

/**
 * Determine if the Primary Brakes are engaged based on their braking presure
 */
bool primaryBrakesEngaged(pod_state_t *state);

/**
 * Determine if the pod is stopped
 */
bool podIsStopped(pod_state_t *state);
