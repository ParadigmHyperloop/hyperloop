# libhw

_Hardware Abstractions and Simulation_

This library is compiled as a static archive and then linked against by [core](../core/).
We use this library to provide abstractions for the hardware we have on the vehicle, 

ex. Valves are abstracted with API enpoints like `open_solenoid` and `close_solenoid`.
Clients need only initialize the `solenoid_t` struct once with the required information
and then never worry about hardware interactions again. 2/2 Valve states are abstracted 
as `Open`, `Opening`, `Closed`, `Closing`, and `Fault`. Valve override and lockout 
functionality is provided as well.

Simulation can be achieved though this interface as well by shimming in simulation versions
of the various hardware interface functions we can virtualize the pod's hardware (ADCs, 
SSR Boards, etc.)

