/*
ADC Pin output
io[1] = P9_39
io[2] = P9_40
io[3] = P9_37
io[4] = P9_38
io[5] = P9_33
io[6] = P9_36
io[7] = P9_35
*/   
   
#include <stdio.h>
#include <unistd.h>
#include "pruio_c_wrapper.h"
#include "pruio_pins.h"

   int main(int argc, const char *argv[]) { 
      PruIo *io = pruio_new(0, 0x98, 0, 1);
      if (io->Errr) {
         printf("Initialisation failed (%s)\n", io->Errr);
         return 1;
      }

      if(pruio_config(io, 0, 0x1FE, 0, 4, 0)){
         printf("Config failed (%s)\n", io->Errr); 
         return 1;
      }
		
      int a = 0;
      int i = 0;
      while(i < 10){
         //printf("\r%12o  %12o  %12o  %12o  %4X %4X %4X %4X %4X %4X %4X %4X\n", io->Gpio[0].Stat, io->Gpio[1].Stat, io->Gpio[2].Stat, io->Gpio[3].Stat, io->Value[1], io->Value[2], io->Value[3], io->Value[4], io->Value[5], io->Value[6], io->Value[7], io->Value[8]);
		 printf("\r%8i %8i %8i %8i %8i %8i %8i %8i\n", io->Value[1], io->Value[2], io->Value[3], io->Value[4], io->Value[5], io->Value[6], io->Value[7], io->Value[8]);
		 fflush(STDIN_FILENO);
         usleep(1000);
		 i = i + 1;
       }


      pruio_destroy(io);

           return 0;
   }
   