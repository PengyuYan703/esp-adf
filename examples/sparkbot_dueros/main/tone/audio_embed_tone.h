
/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2022 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef AUDIO_EMBED_TONE_URI_H
#define AUDIO_EMBED_TONE_URI_H

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    const uint8_t * address;
    int size;
} embed_tone_t;

extern const uint8_t echo_wav[] asm("_binary_echo_wav_start");
extern const uint8_t muyu_wav[] asm("_binary_muyu_wav_start");

/**
 * @brief embed tone url index for `embed_tone_url` array
 */
enum tone_url_e {
    ECHO_WAV = 0,
    MUYU_WAV = 1,
    EMBED_TONE_URL_MAX = 2,
};


extern embed_tone_t g_embed_tone[2];


/**
 * @brief embed tone url
 */
extern const char * embed_tone_url[2];

#endif // AUDIO_EMBED_TONE_URI_H