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

/* color constants */
#define BG_COLOR ((uint32_t)0xFF241593)
#define OUTER_BUTTON_COLOR ((uint32_t)0xFF056CFF)
#define OUTER_SHADE_COLOR ((uint32_t)0xFF1E3454)
#define INNER_BUTTON_COLOR ((uint32_t)0xFF0805FF)
#define INNER_SHADE_COLOR ((uint32_t)0xFF0F0E3B)
#define LCD_X_SIZE RK043FN48H_WIDTH
#define LCD_Y_SIZE RK043FN48H_HEIGHT

/* button sizes */
int big_button_radius;
int medium_button_radius;
int small_button_radius;
int big_button_Y;
int medium_button_Y;
int small_button_Y;
int big_button_distance;
int medium_button_distance;
int small_button_distance;
int stop_button_X;
int play_button_X;
int skip_left_X;
int skip_right_X;
int minus_button_X;
int plus_button_X;


typedef enum{
  PREV_B,
  NEXT_B,
  PAUSE_B,
  PLAY_B,
  STOP_B,
  VOL_UP_B,
  VOL_DOWN_B,
  NONE_B,
  MENU_B,
}button_pressed;

volatile button_pressed last_button_pressed;

volatile int write_title;
volatile int draw_volume;
volatile int currently_read_bytes;

int progress_bar_status;
int actual_file;
int next_file_to_write;
int prev_file_to_write;

uint8_t * prev_title;
uint8_t * next_title;


/* GUI functions */
void initialize_button_values();
void draw_previous_button();
void draw_stop_button();
void draw_play_button();
void draw_pause_button();
void draw_next_button();
void draw_minus_button();
void draw_plus_button();
void draw_title();
void draw_background();
void draw_fill_bar(float part);
int initialize_touchscreen();
int lcd_start();
void startResponsiveGUItask(void *argument);
void draw_title(uint8_t *title);
int is_button_y_axis();
int is_play_button_x_axis();
int is_stop_button_x_axis();
int is_pause_button_x_axis();
int is_prevsong_button_x_axis();
int is_nextsong_button_x_axis();
void draw_volume_bar();
void fill_progress_bar(int current_progress);
void clear_progress_bar();
//void start_touch_task(void *argument);

#endif
