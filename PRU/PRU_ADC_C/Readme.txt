To run open working_code
	-make 
	-run bbb-io.c, to read ADC and GPIO valeus


from: http://www.freebasic.net/forum/viewtopic.php?f=14&t=22501
1. Install FreeBasic compiler in BBB

1.1. Download package from http://www.freebasic-portal.de/download ... k-283.html

Code: Select all
wget http://www.freebasic-portal.de/dlfiles/452/bbb_fbc-0.0.2.tar.xz


1.2. Uncompress and copy files

Code: Select all
 cd bbb_fbc-0.0.2
   cp usr/local/bin/fbc /usr/local/bin/
   cp -R usr/local/lib/freebasic /usr/local/lib/



2. Install pruss driver kit for freebasic and BBB.

2.1. Download and uncompress package from http://www.freebasic-portal.de/dlfiles/ ... .0.tar.bz2

2.2. Copy files

Code: Select all
   cd FB_prussdrv-0.0
   cp bin/libprussdrv.* /usr/local/lib
   ldconfig
   mkdir /usr/local/include/freebasic/BBB
   cp include/* /usr/local/include/freebasic/BBB
   cp bin/pasm/usr/local/bin
   cp bin/PRUSSDRV-00A0.dtbo /lib/firmware
   


2.3. Install am335x-pru-package 

Code: Select all
apt-get install am335x-pru-package


2.4. Activate the PRUSS by enabling the tree overlay. This must be done everytime, after each boot or before running your programs. 

Code: Select all
echo PRUSSDRV> /sys/devices/bone_capemgr.9/slots


3. Install libpruio

3.1. Download and uncompress package from http://www.freebasic-portal.de/dlfiles/ ... .2.tar.bz2

3.2. Copy files

Code: Select all
   cd libpruio-0.0.2
   cd src/c_wrapper/
   cp libpruio.so /usr/local/lib
   cp libpruio.a /usr/local/lib
   ldconfig
   cd ../pruio/
   cp pruio.bi /usr/local/include/freebasic/BBB
   cp pruio.hp /usr/local/include/freebasic/BBB
   cp pruio_pins.bi /usr/local/include/freebasic/BBB
   


4. Here's a simple example C program that uses the library

Code: Select all
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
      int i;
      while(1){
         printf"\r%12o  %12o  %12o  %12o  %4X %4X %4X %4X %4X %4X %4X %4X\n", io->Gpio[0].Stat, io->Gpio[1].Stat, io->Gpio[2].Stat, io->Gpio[3].Stat, io->Value[1], io->Value[2], io->Value[3], io->Value[4], io->Value[5], io->Value[6], io->Value[7], io->Value[8]);
         fflush(STDIN_FILENO);
         usleep(1000);
       }


      pruio_destroy(io);

           return 0;
   }
   


5. To compile it, here's a makefile:

Code: Select all
   all: bbb-io.c Makefile
        gcc -Wall -o bbb-io bbb-io.c /usr/local/lib/freebasic/fbrt0.o -lpruio -L"/usr/local/lib/freebasic/" -lfb -lpthread -lprussdrv -ltermcap -lsupc++ -Wno-unused-variable