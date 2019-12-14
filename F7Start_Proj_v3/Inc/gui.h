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

//Point points[10];

/* GUI functions */
void initialize_button_values();
void draw_previous_button();
void draw_stop_button();
void draw_play_button();
void draw_pause_button();
void draw_next_button();
void draw_minus_button();
void draw_plus_button();
void draw_volume();
void draw_title();
void draw_background();
void draw_fill_bar(float part);
int initialize_touchscreen();
int lcd_start();
void startResponsiveGUItask(void *argument);
int is_button_y_axis();
int is_play_button_x_axis();
int is_stop_button_x_axis();
int is_pause_button_x_axis();
int is_prevsong_button_x_axis();
int is_nextsong_button_x_axis();
//void start_touch_task(void *argument);

/* player state enum */
/*typedef enum
{
  STOP_PRESSED,
  STOPPED,
  PLAY_PRESSED,
  PLAYING,
  PAUSE_PRESSED,
  PAUSED,
  RESUME_PRESSED,
  PREV_PRESSED_PLAYING,
  NEXT_PRESSED_PLAYING,
  PREV_PRESSED_STOPPED,
  NEXT_PRESSED_STOPPED,
  PREV_PRESSED_PAUSED,
  NEXT_PRESSED_PAUSED,
  VOL_UP_PRESSED,
  VOL_DOWN_PRESSED,
} player_states;
*/
/* GUI <-> audio processing communication */
/*
volatile player_states player_state;
volatile int redraw_title;
*/


#endif
