#ifndef MP3H
#define MP3H

#define TEST_FILE "example.mp3.mp3"

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
#include "gui.h"

/* buffer sizes */
#define FILE_BUFFER_SIZE 8192
#define DMA_BUFFER_SIZE 8192

/* file name sizes */
#define NUMBER_OF_FILES 20
#define FILE_NAME_LENGTH 100

/* buffers */
char file_data_buffer[FILE_BUFFER_SIZE];
short intermediate_data_buffer[DMA_BUFFER_SIZE / 2];
char dma_audio_buffer[DMA_BUFFER_SIZE];

/* buffer pointers and offsets */
char *file_data_buffer_ptr;
short *intermediate_data_buffer_ptr;
int intermediate_data_buffer_offs;
int audio_bytes_amount;
uint8_t dma_audio_buffer_offs;

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
int bitrate;
int file_frame_amount;
int current_frame_counter;


/* DMA progress enum */
enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF,
  BUFFER_OFFSET_FULL,
};

typedef enum{
  PAUSED,
  PLAYING,
  STOPPED,
}song_state;

volatile song_state current_song_state;

/* MP3 API functions */
int mp3_init();
int read_directory();
void play_directory();
int next_file();
int prev_file();
int start_reading_file();
int pause_audio();
int stop_audio();
int start_audio();
int resume_audio();
int prev_audio();
int next_audio();
int audio_volume_down(int delta);
int audio_volume_up(int delta);

#endif
