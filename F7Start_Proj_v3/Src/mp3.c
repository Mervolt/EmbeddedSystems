#include "mp3.h"



int mp3_init(){
    //redraw_title = 0;

    volume = 20;

    if (BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE1,
                           volume,
                           AUDIO_FREQUENCY_44K)
        != AUDIO_OK)
    {
        xprintf("ERROR: Failed to configure the audio peripherals\n");
        return -1;
    }

    BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);

    hMP3Decoder = MP3InitDecoder();

    if (hMP3Decoder == 0)
    {
        xprintf("ERROR: Failed to initialize the MP3 decoder\n");
        return -1;
    }

    return 0;
}

int read_directory(char *path){
    FILE_COUNTER = 0;
    CURRENT_FILE = 0;
    pause_playing = STILL_PLAYING;
    reset_playing = CONTINUE_PLAYING;
    last_button_pressed=NONE_B;
    current_song_state=STOPPED;

    DIR dir;
    static FILINFO fno;

    FRESULT res = f_opendir(&dir, path);
    if (res != F_OK)
    {
        xprintf("ERROR: Failed to open directory %s\n", path);
        return -1;
    }
    res = f_readdir(&dir, &fno);
    if (res != F_OK)
    {
        xprintf("ERROR: Failed to read directory %s\n", path);
        return -1;
    }
    while (res == FR_OK && fno.fname[0] != 0 && FILE_COUNTER < NUMBER_OF_FILES)
    {
        for (int i = 0; fno.fname[i] != '\0'; i++)
        {
            if ((fno.fname[i] == '.') && (fno.fname[i + 1] == 'm')
                && (fno.fname[i + 2] == 'p') && (fno.fname[i + 3] == '3'))
            {
                memset(FILES[FILE_COUNTER], 0, FILE_NAME_LENGTH);
                snprintf(FILES[FILE_COUNTER], (i + 8), "%s%s", path, fno.fname);
                if (f_open(&file, FILES[FILE_COUNTER], FA_READ) == FR_OK)
                {
                    if (f_close(&file) == FR_OK)
                    {
                        FILE_COUNTER++;
                    } else
                    {
                        memset(FILES[FILE_COUNTER], 0, FILE_NAME_LENGTH);
                    }
                } else
                {
                    memset(FILES[FILE_COUNTER], 0, FILE_NAME_LENGTH);
                }
            }
        }
        res = f_readdir(&dir, &fno);
    }
    if (f_closedir(&dir) != F_OK)
    {
        xprintf("ERROR: Failed to close directory %s\n", path);
        return -1;
    }
    return 0;
}

int process_callback(int dma_offset){
    int bytes_read, offset;

    while (intermediate_data_buffer_offs < DMA_BUFFER_SIZE / 4)
    {
        /* count offset to skip frame header */
        offset = MP3FindSyncWord((unsigned char *) file_data_buffer_ptr, audio_bytes_amount);
        if (offset == -1)
        {
            audio_bytes_amount = 0;
            return 0;
        }
        /* decrease audio bytes by offset */
        audio_bytes_amount -= offset;
        /* set pointer to real audio data */
        file_data_buffer_ptr += offset;
        if (MP3Decode(hMP3Decoder,
                      (unsigned char **) &file_data_buffer_ptr,
                      (int *) &audio_bytes_amount,
                      intermediate_data_buffer_ptr,
                      0))
        {
            xprintf("ERROR: Failed to decode the next frame\n");
            return -1;
        }
        MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
        intermediate_data_buffer_offs += mp3FrameInfo.outputSamps;
        intermediate_data_buffer_ptr = intermediate_data_buffer + intermediate_data_buffer_offs;
    }

    /* copy audio from intermediate buffer to Direct Media Access buffer */
    memcpy(dma_audio_buffer + dma_offset, intermediate_data_buffer, DMA_BUFFER_SIZE / 2);
    memcpy(file_data_buffer, file_data_buffer_ptr, audio_bytes_amount);//skip nagłówek of mp3 frame - not sure
    memcpy(intermediate_data_buffer,
           &intermediate_data_buffer[DMA_BUFFER_SIZE / 4],
           (intermediate_data_buffer_offs - DMA_BUFFER_SIZE / 4) * 2);

    file_data_buffer_ptr = file_data_buffer + audio_bytes_amount;

    if (f_read(&file,
               file_data_buffer_ptr,
               (FILE_BUFFER_SIZE - audio_bytes_amount),
               (void *) &bytes_read) != F_OK)
    {
        xprintf("ERROR: Failed to read from file\n");
        return -1;
    }

    file_data_buffer_ptr = file_data_buffer;
    audio_bytes_amount += bytes_read;
    intermediate_data_buffer_offs -= DMA_BUFFER_SIZE / 4;
    intermediate_data_buffer_ptr = intermediate_data_buffer + intermediate_data_buffer_offs;
    dma_audio_buffer_offs = BUFFER_OFFSET_NONE;

    return 0;
}


void play_directory(){
    int err = 0;

    err = start_reading_file();

    if (err){
        return;
    }

    write_title=1;

    while(1){
        if(current_song_state==PLAYING){

            if(last_button_pressed==PREV_B){

                xprintf("playing,prev\n");
                err= prev_audio();
                current_song_state=PLAYING;

            }else if(last_button_pressed==NEXT_B){
                xprintf("playing,next\n");
                err= next_audio();
                current_song_state=PLAYING;

            }else if(last_button_pressed==PAUSE_B){
                xprintf("playing,pause\n");
                err = pause_audio();

            }else if(last_button_pressed==STOP_B){
                xprintf("playing,stop\n");
                err = stop_audio();

            }else{//still playing for play_b,none_b
                if(dma_audio_buffer_offs == BUFFER_OFFSET_HALF){
                    err = process_callback(0);
                }

                if(dma_audio_buffer_offs == BUFFER_OFFSET_FULL){
                    err = process_callback(DMA_BUFFER_SIZE / 2);
                }
                if(audio_bytes_amount == 0){
      
                    if (!err){
                        xprintf("End of file \n");
                        CURRENT_FILE = next_file();
                        err=start_audio();
                        write_title=1;
                    }
                }   
               
            }

        }else if(current_song_state==PAUSED){
            if(last_button_pressed==PREV_B){
                xprintf("paused,prev\n");
                err= prev_audio();
                current_song_state=PAUSED;

            }else if(last_button_pressed==NEXT_B){
                xprintf("paused,next\n");
                err= next_audio();
                current_song_state=PAUSED;

            }else if(last_button_pressed==PLAY_B){
                xprintf("paused,play\n");
                err= resume_audio();

            }else if(last_button_pressed==STOP_B){

                //TODO working properly for combination:
                //playing...pause...stop...playing
                xprintf("paused,stop\n");
                err = stop_audio();

            }else{//do nothing for pause,none,
            }
        }else if(current_song_state==STOPPED){
            if(last_button_pressed==PREV_B){
                xprintf("stopped,prev\n");
                CURRENT_FILE=prev_file();
                current_song_state=STOPPED;

            }else if(last_button_pressed==NEXT_B){
                xprintf("stoppped,next\n");
                CURRENT_FILE=next_file();
                current_song_state=STOPPED;

            }else if(last_button_pressed==PLAY_B){
                xprintf("stopped,play\n");
                err= start_audio();
            }else{//do nothing for pause,stop,none,
            }
        }

        last_button_pressed=NONE_B;

        if (err)//break main loop
            break;
        vTaskDelay(2);
    }//end while
}

int start_reading_file(){
    /* open File to play */
    if (f_open(&file, FILES[CURRENT_FILE], FA_READ) != FR_OK)
    {
        xprintf("Failed to open file %s!\n",FILES[CURRENT_FILE]);
        return -1;
    }

    /* set file_data_buffer pointer to file_data_buffer */
    file_data_buffer_ptr = file_data_buffer;

    /* read File data */
    if (f_read(&file, file_data_buffer_ptr, FILE_BUFFER_SIZE, (void *) &audio_bytes_amount)
        != F_OK)
    {
        xprintf("Failed to read from file %s!\n",FILES[CURRENT_FILE]);
        return -1;
    }

    /* set intermediate_data_buffer_pointer to intermediate_data_buffer and set offset to beginning*/
    intermediate_data_buffer_ptr = intermediate_data_buffer;
    intermediate_data_buffer_offs = 0;

    dma_audio_buffer_offs = BUFFER_OFFSET_NONE;

    if (BSP_AUDIO_OUT_Play((uint16_t * ) & dma_audio_buffer[0], DMA_BUFFER_SIZE)
        != AUDIO_OK)
    {
        xprintf("Failed to start the audio!\n");
        return -1;
    }

    return 0;
}

int stop_audio(){
    if(BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW) != AUDIO_OK){
        xprintf("Failed to stop audio!\n");
        return 1;
    }

    memset(dma_audio_buffer, 0, DMA_BUFFER_SIZE);
    memset(intermediate_data_buffer, 0, DMA_BUFFER_SIZE);
    memset(file_data_buffer, 0, FILE_BUFFER_SIZE);

    if (f_close(&file) != F_OK){
        xprintf("Failed to close file- %s !\n",FILES[CURRENT_FILE]);
        return 2;
    }

    current_song_state=STOPPED;

    return 0;
}

int start_audio(){
    if(start_reading_file()){
        xprintf("Failed to start audio!\n");
        return 1;
    }

    current_song_state = PLAYING;
    return 0;
}

int pause_audio(){
    if (BSP_AUDIO_OUT_Pause() != AUDIO_OK)
    {
        xprintf("Failed to pause audio!\n");
        return 1;
    }
    current_song_state=PAUSED;
    return 0;
}

int resume_audio(){
    if (BSP_AUDIO_OUT_Resume() != AUDIO_OK)
    {
        xprintf("Failed to resume audio!\n");
        return 1;
    }

    current_song_state=PLAYING;
    return 0;
}

int prev_audio(){
    if(stop_audio()){
        xprintf("Failed to skip to previous audio[S]!\n");
        return 1;
    }

    CURRENT_FILE = prev_file();

    if(start_reading_file()){
        xprintf("Failed to skip to previous audio[R]!\n");
        return 1;
    }

    write_title=1;
    return 0;
}

int next_audio(){
    if(stop_audio()){
        xprintf("Failed to skip to next audio![S]\n");
        return 1;
    }

    CURRENT_FILE = next_file();

    if(start_reading_file()){
        xprintf("Failed to skip to next audio![R]\n");
        return 1;
    }

    write_title=1;
    return 0;
}

int next_file(){
    return (CURRENT_FILE + 1) % FILE_COUNTER;
}

int prev_file(){
    if(CURRENT_FILE == 0){
        return FILE_COUNTER - 1;
    }
    else{
        return (CURRENT_FILE - 1) % FILE_COUNTER;
    }
}