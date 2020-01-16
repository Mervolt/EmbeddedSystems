#ifndef GUIH
#define GUIH

#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_lcd.h"
#include "wm8994/wm8994.h"
#include "term_io.h"
#include "stm32746g_discovery_audio.h"
#include "pub/mp3dec.h"
#include "pub/mp3common.h"
#include "fatfs.h"
#include <sys/unistd.h>
#include "usb_host.h"
#include "stm32f7xx_hal.h"
#include "mp3.h"
TS_StateTypeDef TS_State;

#define LCD_X_SIZE RK043FN48H_WIDTH
#define LCD_Y_SIZE RK043FN48H_HEIGHT

typedef enum{
  PLAYER_M,
  MENU_M,
}active_menu;

typedef enum{
  PREV_B,
  NEXT_B,
  PAUSE_B,
  PLAY_B,
  STOP_B,
  VOL_UP_B,
  VOL_DOWN_B,
  NONE_B,
  NEW_B,
}button_pressed;

volatile button_pressed last_button_pressed;
volatile active_menu current_active_menu;

volatile int write_title;
volatile int draw_volume;
volatile int currently_read_bytes;
volatile int draw_menu_songs_title;

int progress_bar_status;
int actual_file;
int next_file_to_write;
int prev_file_to_write;
int chosen_file;
int menu_songs_amount;
int menu_songs_position;

uint8_t * prev_title;
uint8_t * next_title;

void draw_background();
int initialize_touchscreen();
int lcd_start();
void startResponsiveGUItask(void *argument);
void draw_title(uint8_t *title);
void draw_volume_bar();
void fill_progress_bar(int current_progress);
void clear_progress_bar();
void draw_menu();
void draw_player();
void draw_menu_songs_titles();
//void start_touch_task(void *argument);

float top_exit_menu_bar_height;
float song_menu_height;
float scroll_songs_menu_width;
float songs_menu_width;
float songs_menu_height;
float split_scroll_songs_line_y;

#endif
