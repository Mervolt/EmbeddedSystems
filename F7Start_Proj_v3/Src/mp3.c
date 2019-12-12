#include "mp3.h"



int mp3_init()
{
    //redraw_title = 0;

    volume = 30;

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

int read_directory(char *path)
{
    FILE_COUNTER = 0;
    CURRENT_FILE = 0;

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

int process_callback(int dma_offset)
{
    int bytes_read, offset;

    while (processing_buff_offs < DMA_BUFFER_SIZE / 4)
    {
        offset = MP3FindSyncWord((unsigned char *) file_buff_ptr, bytes_left);
        if (offset == -1)
        {
            bytes_left = 0;
            return 0;
        }
        bytes_left -= offset;
        file_buff_ptr += offset;
        if (MP3Decode(hMP3Decoder,
                      (unsigned char **) &file_buff_ptr,
                      (int *) &bytes_left,
                      processing_buff_ptr,
                      0))
        {
            xprintf("ERROR: Failed to decode the next frame\n");
            return -1;
        }
        MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
        processing_buff_offs += mp3FrameInfo.outputSamps;
        processing_buff_ptr = processing_buff + processing_buff_offs;
    }

    memcpy(dma_buff + dma_offset, processing_buff, DMA_BUFFER_SIZE / 2);
    memcpy(file_buff, file_buff_ptr, bytes_left);
    memcpy(processing_buff,
           &processing_buff[DMA_BUFFER_SIZE / 4],
           (processing_buff_offs - DMA_BUFFER_SIZE / 4) * 2);

    file_buff_ptr = file_buff + bytes_left;

    if (f_read(&file,
               file_buff_ptr,
               (FILE_BUFFER_SIZE - bytes_left),
               (void *) &bytes_read) != F_OK)
    {
        xprintf("ERROR: Failed to read from file\n");
        return -1;
    }

    file_buff_ptr = file_buff;
    bytes_left += bytes_read;
    processing_buff_offs -= DMA_BUFFER_SIZE / 4;
    processing_buff_ptr = processing_buff + processing_buff_offs;
    dma_buff_offs = BUFFER_OFFSET_NONE;

    return 0;
}


void play_directory()
{
    int err = 0;

    err = start_reading_file();

    if (err)
    {
        return;
    }


    vTaskDelay(2);

    while(1){
        if (dma_buff_offs == BUFFER_OFFSET_HALF){
        err = process_callback(0);
    }

    if (dma_buff_offs == BUFFER_OFFSET_FULL){
        err = process_callback(DMA_BUFFER_SIZE / 2);
    }

    if (bytes_left == 0){
       // err = mp3_stop();
        if (!err){
            xprintf("End of file \n");
           // CURRENT_FILE = next_file();
           // err = mp3_play();
            //redraw_title = 1;
        }
    }
          
    if (err)
        break;
    }
    vTaskDelay(2);
    
    
}

int start_reading_file()
{
    if (f_open(&file, FILES[CURRENT_FILE], FA_READ) != FR_OK)
    {
        xprintf("ERROR: Failed to open file %s\n", FILES[CURRENT_FILE]);
        return -1;
    }

    file_buff_ptr = file_buff;

    if (f_read(&file, file_buff_ptr, FILE_BUFFER_SIZE, (void *) &bytes_left)
        != F_OK)
    {
        xprintf("ERROR: Failed to read from file %s\n", FILES[CURRENT_FILE]);
        return -1;
    }

    processing_buff_ptr = processing_buff;
    processing_buff_offs = 0;

    dma_buff_offs = BUFFER_OFFSET_NONE;

    if (BSP_AUDIO_OUT_Play((uint16_t * ) & dma_buff[0], DMA_BUFFER_SIZE)
        != AUDIO_OK)
    {
        xprintf("ERROR: Failed to start the audio stream\n");
        return -1;
    }

    return 0;
}