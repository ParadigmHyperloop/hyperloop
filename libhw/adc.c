//
//  adc.c
//  pod
//
//  Created by Eddie Hurtig on 7/23/17.
//
//

#include "adc.h"

void init_adc(adc_t *adc, bus_t *bus, const char *name, int num) {
  adc->bus = bus;
  adc->name = name;
  adc->num = num;
}

void read_adc(__unused adc_t *adc, __unused int channel, __unused ADCBlock callback) {
  assert(BUS_BUFFER_SIZE >= 4);
  
  
  
  
//  bus_enqueue(adc->bus, ^(bus_t *bus) {
//    if (bus->mem[0] != adc->num) {
//      set_pin_value(bus->mem[1], (adc->num >> 0) & 0x1);
//      set_pin_value(bus->mem[2], (adc->num >> 1) & 0x1);
//      set_pin_value(bus->mem[3], (adc->num >> 2) & 0x1);
//      bus->mem[0] = adc->num;
//    }
//    
//    write(bus->fd, &channel, 1);
//    
//    char response[2];
//    read(bus->fd, response, sizeof(response)/sizeof(char));
//    
//    adc_response_t result = {
//      .rc = response[0],
//      .raw = response[1]
//    };
//    
//    callback(result);
//  });
}
