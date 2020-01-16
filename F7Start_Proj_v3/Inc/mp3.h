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

#define FILE_BUFFER_SIZE 8192
#define DMA_BUFFER_SIZE 8192

#define NUMBER_OF_FILES 20
#define FILE_NAME_LENGTH 100

char file_data_buffer[FILE_BUFFER_SIZE];
short intermediate_data_buffer[DMA_BUFFER_SIZE / 2];
char dma_audio_buffer[DMA_BUFFER_SIZE];

char *file_data_buffer_ptr;
short *intermediate_data_buffer_ptr;
int intermediate_data_buffer_offs;
int audio_bytes_amount;
uint8_t dma_audio_buffer_offs;

uint8_t volume;
HMP3Decoder hMP3Decoder;
MP3FrameInfo mp3FrameInfo;

extern ApplicationTypeDef Appli_state;
char FILES[NUMBER_OF_FILES][FILE_NAME_LENGTH];
int FILE_COUNTER;
int CURRENT_FILE;
FIL file;
int bitrate;
int file_frame_amount;
int current_frame_counter;

/*length test*/
int callback_count_for_length;
int frame_sync_find_count;

int offset_for_length;
FIL file_for_length;
char file_data_buffer_for_length[FILE_BUFFER_SIZE];
char* file_data_buffer_ptr_for_length;
int audio_bytes_amount_for_length;
HMP3Decoder hMP3Decoder_for_length;
MP3FrameInfo mp3FrameInfo_for_length;
short* intermediate_data_buffer_ptr_for_length;
int intermediate_data_buffer_offs_for_length;
short intermediate_data_buffer_for_length[DMA_BUFFER_SIZE / 2];
FRESULT is_anything_left_for_length;


int FILE_SIZES[NUMBER_OF_FILES];


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
