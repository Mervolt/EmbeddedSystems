#ifndef __MP3AUDIO_H
#define __MP3AUDIO_H

#include "stm32746g_discovery_ts.h"
#include "term_io.h"
#include "stm32746g_discovery_audio.h"
#include "pub/mp3dec.h"
#include "pub/mp3common.h"
#include "fatfs.h"
#include <sys/unistd.h>
#include "usb_host.h"
#include "wm8994/wm8994.h"
#include "stm32f7xx_hal.h"
#include "screen_refresh.h"

/* buffer sizes */
#define FILE_BUFFER_SIZE 8192
#define DMA_BUFFER_SIZE 8192

/* file name sizes */
#define NUMBER_OF_FILES 20
#define FILE_NAME_LENGTH 100

/* buffers */
char file_buff[FILE_BUFFER_SIZE];
short processing_buff[DMA_BUFFER_SIZE / 2];
char dma_buff[DMA_BUFFER_SIZE];

/* buffer pointers and offsets */
char *file_buff_ptr;
short *processing_buff_ptr;
int processing_buff_offs;
int bytes_left;
uint8_t dma_buff_offs;

/* audio variables */
uint8_t volume;
HMP3Decoder hMP3Decoder;
MP3FrameInfo mp3FrameInfo;

/* file system variables */
extern ApplicationTypeDef Appli_state;
char FILES[NUMBER_OF_FILES][FILE_NAME_LENGTH];
int FILE_COUNTER;
int CURRENT_FILE;
FIL file;

/* DMA progress enum */
enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF,
  BUFFER_OFFSET_FULL,
};

/* MP3 API functions */
int init_mp3();
int read_directory();
void play_directory();
int next_file();
int prev_file();

#endif
