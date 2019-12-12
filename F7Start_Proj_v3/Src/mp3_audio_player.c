#include "mp3_audio.h"

int init_mp3()
{
    redraw_title = 0;

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