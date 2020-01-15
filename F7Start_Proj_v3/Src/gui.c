#include "gui.h"

int is_button_y_axis(){
    return (TS_State.touchY[0] > 0.7 * LCD_Y_SIZE) && (TS_State.touchY[0] < 0.7 * LCD_Y_SIZE + 0.1 * LCD_X_SIZE);
}

int is_top_button_y_axis(){
     return (TS_State.touchY[0] > 0.04 * LCD_Y_SIZE) && (TS_State.touchY[0] < 0.27 * LCD_Y_SIZE + 0.1 * LCD_X_SIZE);
}

int is_volume_down_x_axis(){
    return (TS_State.touchX[0] > 0.03*LCD_X_SIZE) && (TS_State.touchX[0] < 0.17*LCD_X_SIZE);
}

int is_volume_up_x_axis(){
    return (TS_State.touchX[0] > 0.83*LCD_X_SIZE) && (TS_State.touchX[0] < 0.97*LCD_X_SIZE);
}

int is_menu_x_axis(){
    return (TS_State.touchX[0] > 0.2*LCD_X_SIZE) && (TS_State.touchX[0] < 0.8*LCD_X_SIZE);
}

int is_play_button_x_axis(){
    return (TS_State.touchX[0] > 0.45*LCD_X_SIZE) && (TS_State.touchX[0] < 0.55*LCD_X_SIZE);
}

int is_stop_button_x_axis(){
    return (TS_State.touchX[0] > 0.28*LCD_X_SIZE) && (TS_State.touchX[0] < 0.38*LCD_X_SIZE);
}

int is_pause_button_x_axis(){
    return (TS_State.touchX[0] > 0.63*LCD_X_SIZE) && (TS_State.touchX[0] < 0.73*LCD_X_SIZE);
}

int is_nextsong_button_x_axis(){
    return (TS_State.touchX[0] > 0.79*LCD_X_SIZE) && (TS_State.touchX[0] < 0.90*LCD_X_SIZE);
}

int is_prevsong_button_x_axis(){
    return (TS_State.touchX[0] > 0.10*LCD_X_SIZE) && (TS_State.touchX[0] < 0.21*LCD_X_SIZE);
}
int is_songs_y_axis(){
    return (TS_State.touchY[0] > top_exit_menu_bar_height*LCD_Y_SIZE ) ;
}
int is_songs_x_axis(){
    return (TS_State.touchX[0] < songs_menu_width * LCD_X_SIZE);
}
int is_scroll_songs_x_axis(){
    return (TS_State.touchX[0] > songs_menu_width * LCD_X_SIZE);
}
int is_scroll_songs_down_axis(){
    return (TS_State.touchY[0] > (top_exit_menu_bar_height + songs_menu_height/2)*LCD_Y_SIZE ) ;
}
int is_scroll_songs_up_axis(){
    return (TS_State.touchY[0] < (top_exit_menu_bar_height + songs_menu_height/2)*LCD_Y_SIZE ) ;
}
int is_exit_menu_y_axis(){
    return (TS_State.touchY[0] < top_exit_menu_bar_height*LCD_Y_SIZE ); 
}

int which_song_from_menu_selected(){
    return (int)((TS_State.touchY[0] - top_exit_menu_bar_height*LCD_Y_SIZE) / (song_menu_height*LCD_Y_SIZE));
}


void startResponsiveGUItask(void *argument){
  write_title=0;
  lcd_start();
  initialize_touchscreen();
  draw_background();
  while(1){
      if(current_active_menu==PLAYER_M){
        if(write_title){
          draw_title(FILES[CURRENT_FILE] + 3);
          write_title=0;
        }
        if(draw_volume){
          draw_volume_bar();
          draw_volume=0;
        }
        fill_progress_bar(currently_read_bytes);
      }else{
          if(draw_menu_songs_title){
            draw_menu_songs_titles();
             draw_menu_songs_title=0;
          }
           
      }
      

      vTaskDelay(200);
      
    if (BSP_TS_GetState(&TS_State) != TS_OK){
        while (1) {}
    }
    if (TS_State.touchDetected){

        if(current_active_menu==PLAYER_M){//player
            if(is_button_y_axis()){
                if(is_play_button_x_axis()){
                    last_button_pressed=PLAY_B;
                }
                if(is_stop_button_x_axis()){
                    last_button_pressed=STOP_B;
                }
                if(is_pause_button_x_axis()){
                    last_button_pressed=PAUSE_B;
                }
                if(is_nextsong_button_x_axis()){
                
                last_button_pressed= NEXT_B;
                }
                if(is_prevsong_button_x_axis()){
                    last_button_pressed=PREV_B;
                }
            }else if(is_top_button_y_axis()){
                if(is_volume_down_x_axis()){
                    last_button_pressed =VOL_DOWN_B;
                }
                if(is_menu_x_axis()){
                    current_active_menu=MENU_M;
                    menu_songs_position=0;
                    chosen_file=0;
                    draw_menu_songs_title=1;
                    draw_menu();
                }
                if(is_volume_up_x_axis()){
                    last_button_pressed = VOL_UP_B;
                }
            }
        }else{//menu
            if(is_songs_y_axis()){

                xprintf("SONGS Y AXIS\n");
                if(is_songs_x_axis()){

                    xprintf("SONGS X AXIS\n");
                    chosen_file = which_song_from_menu_selected() + menu_songs_position;
                    xprintf("SONGS selected: %d out of %d, pos: %d\n",chosen_file,FILE_COUNTER,
                        menu_songs_position);
                    if(chosen_file < FILE_COUNTER){
                        last_button_pressed=NEW_B;
                        current_active_menu=PLAYER_M;
                        draw_player();
                    }
                }
                if(is_scroll_songs_x_axis()){
                        xprintf("NO SONGS X AXIS\n");
                    if(is_scroll_songs_down_axis()){
                        if(menu_songs_position+ menu_songs_amount < FILE_COUNTER ){
                                draw_menu_songs_title=1;
                                menu_songs_position+=1;
                        }
                    }
                    if(is_scroll_songs_up_axis()){
                        if(menu_songs_position>0){
                            draw_menu_songs_title=1;
                            menu_songs_position-=1;
                        }
                    }
                }
            }
            if(is_exit_menu_y_axis()){
                            xprintf("X-%f, Y-%f\n",TS_State.touchX[0],TS_State.touchY[0]);

                current_active_menu=PLAYER_M;
                draw_player();
            }
        }
    }
  }
}

void draw_player(void){
    draw_background();
    draw_title(FILES[CURRENT_FILE] + 3);
    write_title=0;
    draw_volume_bar();
    draw_volume=0;
    fill_progress_bar(currently_read_bytes);
}

void draw_menu(void){
    //reset background
    BSP_LCD_SelectLayer(0);
    BSP_LCD_Clear(LCD_COLOR_LIGHTBLUE);
    //reset foreground
    BSP_LCD_SelectLayer(1);
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

    BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);

    //top exit bar
    BSP_LCD_FillRect(0,0,LCD_X_SIZE,top_exit_menu_bar_height*LCD_Y_SIZE );
    //next/prev songs 
    BSP_LCD_FillRect(songs_menu_width*LCD_X_SIZE,top_exit_menu_bar_height*LCD_Y_SIZE,
        scroll_songs_menu_width*LCD_X_SIZE,songs_menu_height*LCD_Y_SIZE+1);
    

    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawHLine(songs_menu_width*LCD_X_SIZE,split_scroll_songs_line_y*LCD_Y_SIZE,
        scroll_songs_menu_width*LCD_X_SIZE-1);

    //lines between songs names
    int i=0;
    for(;i<menu_songs_amount;i++){
        BSP_LCD_DrawHLine(0,(top_exit_menu_bar_height+song_menu_height*i)*LCD_Y_SIZE -1,
            songs_menu_width*LCD_X_SIZE);
    }

    draw_menu_songs_title=1;
}

void draw_menu_songs_titles(){

    //erase previous songs names
     BSP_LCD_SelectLayer(1);
     BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
     int i=0;
     for(;i<menu_songs_amount;i++){
         BSP_LCD_FillRect(0,(top_exit_menu_bar_height+song_menu_height*i)*LCD_Y_SIZE,
            scroll_songs_menu_width*LCD_X_SIZE-2,song_menu_height*LCD_Y_SIZE-2);
     }
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    for(i=0;i<menu_songs_amount;i++){
       
        BSP_LCD_DisplayStringAt(0,(top_exit_menu_bar_height+song_menu_height*i)*LCD_Y_SIZE,
            FILES[menu_songs_position+i]+3,CENTER_MODE);
    }
}

int lcd_start(void){
    /* LCD Initialization */
    if (BSP_LCD_Init() != LCD_OK)
    {
        return -1;
    }

    /* LCD Initialization */
    BSP_LCD_LayerDefaultInit(0, (unsigned int) 0xC0000000);
    BSP_LCD_LayerDefaultInit(1, (unsigned int) 0xC0000000 + (LCD_X_SIZE * LCD_Y_SIZE * 4));

    /* Enable the LCD */
    BSP_LCD_DisplayOn();

    /* Select the LCD Background Layer  */
    BSP_LCD_SelectLayer(0);

    /* Clear the Background Layer */
    BSP_LCD_Clear(LCD_COLOR_LIGHTBLUE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

    BSP_LCD_SetColorKeying(1, LCD_COLOR_WHITE);

    /* Select the LCD Foreground Layer  */
    BSP_LCD_SelectLayer(1);

    /* Clear the Foreground Layer */
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

    /* Configure the transparency for foreground and background :
     Increase the transparency */
    BSP_LCD_SetTransparency(0, 255);
    BSP_LCD_SetTransparency(1, 255);

    return 0;
}

int initialize_touchscreen(void){
    uint8_t status = 0;
    status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
    if (status != TS_OK)
    {
        return -1;
    }
    return 0;
}

void fill_progress_bar(int current_progress){
    float progress_helper = current_progress * 1.0 / FILE_SIZES[CURRENT_FILE];
    progress_helper = progress_helper * 100.0;
    progress_bar_status = progress_helper * LCD_X_SIZE / 100;
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    if(progress_bar_status > 478)
        progress_bar_status = 478;
    BSP_LCD_FillRect(1, 0.32*LCD_Y_SIZE + 1, progress_bar_status, 0.13*LCD_Y_SIZE - 2);
}

void clear_progress_bar(){
    currently_read_bytes = 0;
    BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
    BSP_LCD_FillRect(1, 0.32*LCD_Y_SIZE + 1, LCD_X_SIZE - 2, 0.13*LCD_Y_SIZE - 2);
}


void draw_title(uint8_t *title){
    actual_file = CURRENT_FILE;
    next_file_to_write = (CURRENT_FILE+1) % FILE_COUNTER;
    if(CURRENT_FILE == 0){
        prev_file_to_write = FILE_COUNTER - 1;
    }
    else{
        prev_file_to_write = (CURRENT_FILE - 1) % FILE_COUNTER;
    }
    next_title = FILES[next_file_to_write] + 3;
    prev_title = FILES[prev_file_to_write] + 3;


    //erase prev title
    BSP_LCD_SelectLayer(1);

    BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
    BSP_LCD_FillRect(0.2*LCD_X_SIZE + 1, 0.05*LCD_Y_SIZE + 1,0.6*LCD_X_SIZE - 2,0.2*LCD_Y_SIZE - 1);

    //redraw title frame CHECK_IF_NEEDED
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawRect(0.2*LCD_X_SIZE,0.05*LCD_Y_SIZE,0.6*LCD_X_SIZE,0.2*LCD_Y_SIZE);

    //redraw next song title frame
    BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
    BSP_LCD_FillRect(0.5*LCD_X_SIZE + 2, 0.5*LCD_Y_SIZE + 1,0.5*LCD_X_SIZE - 2 - 1, 0.17*LCD_Y_SIZE - 2);

    //redraw prev song title frame
    BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
    BSP_LCD_FillRect(0, 0.5*LCD_Y_SIZE + 1, 0.5*LCD_X_SIZE - 2, 0.17 *LCD_Y_SIZE - 2);

    BSP_LCD_SelectLayer(1);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

    BSP_LCD_DisplayStringAt(0, 0.15*LCD_Y_SIZE, title, CENTER_MODE);

    BSP_LCD_DisplayStringAt(0.55*LCD_X_SIZE, 0.57*LCD_Y_SIZE, next_title, LEFT_MODE);
    
    BSP_LCD_DisplayStringAt(0.05*LCD_X_SIZE, 0.57*LCD_Y_SIZE, prev_title, LEFT_MODE);

}

void draw_background(void){
    //reset background
    BSP_LCD_SelectLayer(0);
    BSP_LCD_Clear(LCD_COLOR_LIGHTBLUE);
    //reset foreground
    BSP_LCD_SelectLayer(1);
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);


  /* Select the LCD Background Layer  */
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    //title frame
  BSP_LCD_DrawRect(0.2*LCD_X_SIZE,0.05*LCD_Y_SIZE,0.6*LCD_X_SIZE,0.2*LCD_Y_SIZE);
 
  BSP_LCD_SelectLayer(0);
  draw_volume_bar();
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

  draw_volume=0;

    //split title bar
  BSP_LCD_DrawVLine(0.5*LCD_X_SIZE, 0.50*LCD_Y_SIZE, 0.17 *LCD_Y_SIZE);
  BSP_LCD_DrawVLine(0.5*LCD_X_SIZE + 1, 0.50*LCD_Y_SIZE, 0.17*LCD_Y_SIZE);
  BSP_LCD_DrawVLine(0.5*LCD_X_SIZE - 1, 0.50*LCD_Y_SIZE , 0.17*LCD_Y_SIZE);



	// prev button
  int16_t num_of_points_left= 3;
  pPoint pointsL =(pPoint) malloc( sizeof(Point) *num_of_points_left); 
  pointsL[0].X=0.10*LCD_X_SIZE;//middle of arrow 
  pointsL[0].Y=0.70*LCD_Y_SIZE + 0.05*LCD_X_SIZE;
  pointsL[1].X=0.16*LCD_X_SIZE;//top of arrow
  pointsL[1].Y=0.70*LCD_Y_SIZE;
  pointsL[2].X=0.16*LCD_X_SIZE;
  pointsL[2].Y=0.70*LCD_Y_SIZE + 0.1*LCD_X_SIZE;//bottom of arrow
  BSP_LCD_FillPolygon(pointsL, num_of_points_left);
  

  pointsL[0].X=0.15*LCD_X_SIZE;//middle of arrow 
  pointsL[0].Y=0.70*LCD_Y_SIZE + 0.05*LCD_X_SIZE;
  pointsL[1].X=0.21*LCD_X_SIZE;//top of arrow
  pointsL[1].Y=0.70*LCD_Y_SIZE;
  pointsL[2].X=0.21*LCD_X_SIZE;
  pointsL[2].Y=0.70*LCD_Y_SIZE + 0.1*LCD_X_SIZE;//bottom of arrow
  BSP_LCD_FillPolygon(pointsL, num_of_points_left);

  

  // stop button
  BSP_LCD_FillRect(0.28*LCD_X_SIZE,0.70*LCD_Y_SIZE,0.10*LCD_X_SIZE,0.10*LCD_X_SIZE);
  
  
  //play button
  int16_t num_of_points_triangle= 3;
  pPoint pointsT =(pPoint) malloc( sizeof(Point) *num_of_points_triangle); 
  pointsT[0].X=0.45*LCD_X_SIZE; //top of triangle
  pointsT[0].Y=0.70*LCD_Y_SIZE;
  pointsT[1].X=0.45*LCD_X_SIZE; //bottom of triangle
  pointsT[1].Y=0.70*LCD_Y_SIZE + 0.1*LCD_X_SIZE;
  pointsT[2].X=0.55*LCD_X_SIZE;//mid of triangle
  pointsT[2].Y=0.70*LCD_Y_SIZE + 0.05*LCD_X_SIZE;
  BSP_LCD_FillPolygon(pointsT, num_of_points_triangle);
  
  
  //pause button
  BSP_LCD_FillRect(0.63*LCD_X_SIZE, 0.70*LCD_Y_SIZE, 0.03*LCD_X_SIZE, 0.10*LCD_X_SIZE);
  
  BSP_LCD_FillRect(0.68*LCD_X_SIZE, 0.70*LCD_Y_SIZE, 0.03*LCD_X_SIZE, 0.10*LCD_X_SIZE);

  
  // next button
  pointsL[0].X=0.85*LCD_X_SIZE;//middle of arrow 
  pointsL[0].Y=0.70*LCD_Y_SIZE + 0.05*LCD_X_SIZE;
  pointsL[1].X=0.79*LCD_X_SIZE;//top of arrow
  pointsL[1].Y=0.70*LCD_Y_SIZE;
  pointsL[2].X=0.79*LCD_X_SIZE;
  pointsL[2].Y=0.70*LCD_Y_SIZE + 0.1*LCD_X_SIZE;//bottom of arrow
  BSP_LCD_FillPolygon(pointsL, num_of_points_left);
  
   
  pointsL[0].X=0.9*LCD_X_SIZE;//middle of arrow 
  pointsL[0].Y=0.70*LCD_Y_SIZE + 0.05*LCD_X_SIZE;
  pointsL[1].X=0.84*LCD_X_SIZE;//top of arrow
  pointsL[1].Y=0.70*LCD_Y_SIZE;
  pointsL[2].X=0.84*LCD_X_SIZE;
  pointsL[2].Y=0.70*LCD_Y_SIZE + 0.1*LCD_X_SIZE;//bottom of arrow
  BSP_LCD_FillPolygon(pointsL, num_of_points_left);


  //progress frame 
  BSP_LCD_DrawHLine(1, 0.32*LCD_Y_SIZE, LCD_X_SIZE - 2);
  BSP_LCD_DrawHLine(1, 0.32*LCD_Y_SIZE - 1, LCD_X_SIZE - 2);

  BSP_LCD_DrawHLine(1, 0.45*LCD_Y_SIZE, LCD_X_SIZE - 2);
  BSP_LCD_DrawHLine(1, 0.45*LCD_Y_SIZE - 1, LCD_X_SIZE - 2);

  //next and previous song frame
  BSP_LCD_DrawHLine(1, 0.50*LCD_Y_SIZE, LCD_X_SIZE - 2);
  BSP_LCD_DrawHLine(1, 0.50*LCD_Y_SIZE - 1, LCD_X_SIZE - 2);

  BSP_LCD_DrawHLine(1, 0.67*LCD_Y_SIZE, LCD_X_SIZE - 2);
  BSP_LCD_DrawHLine(1, 0.67*LCD_Y_SIZE - 1, LCD_X_SIZE - 2);


  //minus volume button
  BSP_LCD_DrawCircle(0.1*LCD_X_SIZE,0.15*LCD_Y_SIZE,0.07*LCD_X_SIZE);
  BSP_LCD_DrawCircle(0.1*LCD_X_SIZE,0.15*LCD_Y_SIZE,0.07*LCD_X_SIZE + 1);
  BSP_LCD_DrawCircle(0.1*LCD_X_SIZE,0.15*LCD_Y_SIZE,0.07*LCD_X_SIZE - 1);
  BSP_LCD_DrawHLine(0.04*LCD_X_SIZE,0.15*LCD_Y_SIZE,0.12*LCD_X_SIZE);
  BSP_LCD_DrawHLine(0.04*LCD_X_SIZE,0.15*LCD_Y_SIZE + 1,0.12*LCD_X_SIZE);
  BSP_LCD_DrawHLine(0.04*LCD_X_SIZE,0.15*LCD_Y_SIZE - 1,0.12*LCD_X_SIZE);

  //plus volume button
  BSP_LCD_DrawCircle(0.9*LCD_X_SIZE,0.15*LCD_Y_SIZE,0.07*LCD_X_SIZE);
  BSP_LCD_DrawCircle(0.9*LCD_X_SIZE,0.15*LCD_Y_SIZE,0.07*LCD_X_SIZE + 1);
  BSP_LCD_DrawCircle(0.9*LCD_X_SIZE,0.15*LCD_Y_SIZE,0.07*LCD_X_SIZE - 1);
  BSP_LCD_DrawHLine(0.84*LCD_X_SIZE,0.15*LCD_Y_SIZE,0.12*LCD_X_SIZE);
  BSP_LCD_DrawHLine(0.84*LCD_X_SIZE,0.15*LCD_Y_SIZE + 1,0.12*LCD_X_SIZE);
  BSP_LCD_DrawHLine(0.84*LCD_X_SIZE,0.15*LCD_Y_SIZE - 1,0.12*LCD_X_SIZE);
  BSP_LCD_DrawVLine(0.9*LCD_X_SIZE,0.04*LCD_Y_SIZE,0.12*LCD_X_SIZE);
  BSP_LCD_DrawVLine(0.9*LCD_X_SIZE + 1,0.04*LCD_Y_SIZE,0.12*LCD_X_SIZE);
  BSP_LCD_DrawVLine(0.9*LCD_X_SIZE - 1,0.04*LCD_Y_SIZE,0.12*LCD_X_SIZE);

  BSP_LCD_SelectLayer(1);
}

void draw_volume_bar(){
    float vol_ratio = (volume*1.0)/100.0;
    //volume bar frame
    BSP_LCD_SelectLayer(0);
    BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
    BSP_LCD_FillRect(0.2*LCD_X_SIZE,0.25*LCD_Y_SIZE,0.6*LCD_X_SIZE,0.03*LCD_Y_SIZE);

    BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
    BSP_LCD_FillRect(0.2*LCD_X_SIZE,0.25*LCD_Y_SIZE,0.6*LCD_X_SIZE*vol_ratio,0.03*LCD_Y_SIZE);

}

void draw_fill_bar(float part){

	BSP_LCD_SelectLayer(0);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	
	BSP_LCD_FillRect(0,0.35*LCD_Y_SIZE +1 ,part *LCD_X_SIZE +1 ,0.2*LCD_Y_SIZE -1);

}
