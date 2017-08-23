//
//  adc.c
//  pod
//
//  Created by Eddie Hurtig on 7/23/17.
//
//

#include "adc.h"

#include <fcntl.h>     //Needed for SPI port
#include <sys/ioctl.h> //Needed for SPI port
#ifdef BBB
#include <linux/spi/spidev.h> //Needed for SPI port
#endif
#include <unistd.h> //Needed for SPI port

#define ADC_SPEED 1000000
#define ADC_BITS_PER_WORD 16
#define ADC_MODE SPI_MODE_2
#define ADC_BUS "/dev/spidev2.0"

#define ADC_DEMUX_A 117
#define ADC_DEMUX_B 49
#define ADC_DEMUX_C 20

uint16_t int_to_spi_channel(uint8_t channel) {
  uint8_t send[2] = {0, 0};
  // TODO: This is gross copypasta, it should be reduced
  if (channel == 0) {
    send[0] = 0x40;
    send[1] = 0x18;
  } else if (channel == 1) {
    send[0] = 0xC0;
    send[1] = 0x18;
  } else if (channel == 2) {
    send[0] = 0x40;
    send[1] = 0x19;
  } else if (channel == 3) {
    send[0] = 0xC0;
    send[1] = 0x19;
  } else if (channel == 4) {
    send[0] = 0x40;
    send[1] = 0x1A;
  } else if (channel == 5) {
    send[0] = 0xC0;
    send[1] = 0x1A;
  } else if (channel == 6) {
    send[0] = 0x40;
    send[1] = 0x1B;
  } else if (channel == 7) {
    send[0] = 0xC0;
    send[1] = 0x1B;
  } else if (channel == 8) {
    send[0] = 0x40;
    send[1] = 0x1C;
  } else if (channel == 9) {
    send[0] = 0xC0;
    send[1] = 0x1C;
  } else if (channel == 10) {
    send[0] = 0x40;
    send[1] = 0x1D;
  } else if (channel == 11) {
    send[0] = 0xC0;
    send[1] = 0x1D;
  } else if (channel == 12) {
    send[0] = 0x40;
    send[1] = 0x1E;
  } else if (channel == 13) {
    send[0] = 0xC0;
    send[1] = 0x1E;
  } else if (channel == 14) {
    send[0] = 0x40;
    send[1] = 0x1F;
  } else {
    send[0] = 0xC0;
    send[1] = 0x1F;
  }
  uint16_t ret = (uint16_t)((send[0] << 8) | send[1]);
  return ret;
}

int open_spi() {
#ifdef BBB
  int status_value = -1;

  //----- SET SPI MODE -----
  // SPI_MODE_0 (0,0) 	CPOL = 0, CPHA = 0, Clock idle low, data is clocked in
  // on rising edge, output data (change) on falling edge
  // SPI_MODE_1 (0,1) 	CPOL = 0, CPHA = 1, Clock idle low, data is clocked in
  // on falling edge, output data (change) on rising edge
  // SPI_MODE_2 (1,0) 	CPOL = 1, CPHA = 0, Clock idle high, data is clocked in
  // on falling edge, output data (change) on rising edge
  // SPI_MODE_3 (1,1) 	CPOL = 1, CPHA = 1, Clock idle high, data is clocked in
  // on rising, edge output data (change) on falling edge
  int spi_mode = ADC_MODE;

  //----- SET BITS PER WORD -----
  int spi_bitsPerWord = ADC_BITS_PER_WORD;

  //----- SET SPI BUS SPEED -----
  int spi_speed = ADC_SPEED; // 1000000 = 1MHz (1uS per bit)

  int spi_fd = open(ADC_BUS, O_RDWR);

  if (spi_fd < 0) {
    perror("Error - Could not open SPI device");
    return -1;
  }

  status_value = ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode);
  if (status_value < 0) {
    perror("Could not set SPIMode (WR)...ioctl fail");
    return -1;
  }

  status_value = ioctl(spi_fd, SPI_IOC_RD_MODE, &spi_mode);
  if (status_value < 0) {
    perror("Could not set SPIMode (RD)...ioctl fail");
    return -1;
  }

  status_value = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
  if (status_value < 0) {
    perror("Could not set SPI bitsPerWord (WR)...ioctl fail");
    return -1;
  }

  status_value = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord);
  if (status_value < 0) {
    perror("Could not set SPI bitsPerWord(RD)...ioctl fail");
    return -1;
  }

  status_value = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
  if (status_value < 0) {
    perror("Could not set SPI speed (WR)...ioctl fail");
    return -1;
  }

  status_value = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
  if (status_value < 0) {
    perror("Could not set SPI speed (RD)...ioctl fail");
    return -1;
  }

  return spi_fd;
#endif
  return -1;
}

int init_adc(adc_t *adc, int fd, const char *name, int num) {
  adc->name = name;
  adc->num = num;
  adc->fd = fd;

  return 0;
}

int set_gpio_for_adc(adc_t *adc) {
  int rc = 0;
  
  rc = (int)set_pin_value(ADC_DEMUX_A, (adc->num >> 0) & 0x1);
  if (rc < 0) {
    return rc;
  }
  
  rc = (int)set_pin_value(ADC_DEMUX_B, (adc->num >> 1) & 0x1);
  if (rc < 0) {
    return rc;
  }
  
  rc = (int)set_pin_value(ADC_DEMUX_C, (adc->num >> 2) & 0x1);
  if (rc < 0) {
    return rc;
  }
  return 0;
}

int read_adc(__unused adc_t *adc, __unused uint8_t channel) {
  assert(BUS_BUFFER_SIZE >= 4);
#ifdef BBB
  int new_channel = -1, attempts = 0;
  do {

    uint16_t send = int_to_spi_channel(channel);
    char data[2] = {send >> 8, send & 0xFF};

    struct spi_ioc_transfer spi[1];
    int i = 0;
    int retVal = -1;
    int spi_fd = adc->fd;

    // one spi transfer for each byte
    memset(&spi[i], 0, sizeof(spi[i]));

    spi[i].tx_buf = (unsigned long)(data + i); // transmit from "data"
    spi[i].rx_buf = (unsigned long)(data + i); // receive into "data"
    spi[i].len = 2;
    spi[i].delay_usecs = 0;
    spi[i].speed_hz = ADC_SPEED;
    spi[i].bits_per_word = ADC_BITS_PER_WORD;
    spi[i].cs_change = 0;

    retVal = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);

    if (retVal < 0) {
      perror("Error - Problem transmitting spi data..ioctl");
      return -1.0;
    }
    new_channel = ((int)data[1] & 0xF0) >> 4;

    printf("Recieved Channel: %d, expected %d\n", new_channel, channel);

    int value = (((int)data[1] & 0x0F) << 8) | (int)data[0];

    if (new_channel == channel) {
      printf("Got Valid Value: %d\n", value);
      return value;
    }
    attempts++;
  } while (attempts < 4);

  return -1;
#endif

  return 0;
}
