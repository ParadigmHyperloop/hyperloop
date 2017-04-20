//
//  main.c
//  tparse
//
//  Created by Eddie Hurtig on 4/15/17.
//
//

#include <stdio.h>
#include <telemetry.h>

void packet_as_json(telemetry_packet_t *p, void *outfunc(char *fmt, ...)) {
  outfunc("{ uint8_t version;
          uint16_t size;
          // state
          uint8_t state;
          // Solenoids
          uint32_t solenoids;
          uint64_t timestamp;
          // IMU
          float position_x;
          float position_y;
          float position_z;
          
          float velocity_x;
          float velocity_y;
          float velocity_z;
          
          float acceleration_x;
          float acceleration_y;
          float acceleration_z;
          
          // Distance sensors
          float corners[N_CORNER_DISTANCE];  // 4
          float wheels[N_WHEEL_DISTANCE];    // 3
          float lateral[N_LATERAL_DISTANCE]; // 3
          
          // Pressures
          float hp_pressure;                           // 1
          float reg_pressure[N_REG_PRESSURE];          // 4
          float clamp_pressure[N_CLAMP_PRESSURE];      // 2
          float skate_pressure[N_SKATE_PRESSURE];      // 2
          float lateral_pressure[N_LAT_FILL_PRESSURE]; // 2
          
          // Thermocouples
          float hp_thermo;                          // 1
          float reg_thermo[N_REG_THERMO];           // 4
          float reg_surf_thermo[N_REG_SURF_THERMO]; // 4
          float power_thermo[N_POWER_THERMO];       // 4
          float clamp_thermo[N_CLAMP_PAD_THERMO];   // 2
          float frame_thermo;                       // 1
          
          // Batteries
          float voltages[N_BATTERIES]; // 3
          float currents[N_BATTERIES]; // 3
          
          // Photo
          float rpms[N_WHEEL_PHOTO]; // 3
          uint32_t stripe_count;}");
  
  
}
int main(int argc, const char * argv[]) {
  // insert code here...
  if (argc != 2) {
    printf("Usage: %s <path>\n", getprogname());
  }
  char *filename = argv[1];
  
  int fd = open(filename, O_RDONLY);
  
  if (fd < 0) {
    perror("Failed to open path: ");
    exit(1);
  }
  
  telemetry_packet_t *buf = malloc(sizeof(telemetry_packet_t));
  
  int count = 0;
  int rd;
  printf("[");
  
  while ((rd = read(fd, buf, sizeof(telemetry_packet_t))) == sizeof(telemetry_packet_t)) {
    if (count > 0) {
      printf(",");
    }
    
    packet_as_json(buf, printf);
    
    count++;
  }
  
  return 0;
}


