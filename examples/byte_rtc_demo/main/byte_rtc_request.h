#pragma once


typedef enum {
    BYTE_RTC_REQ_AUDIO_TYPE_UNKNOE,
    BYTE_RTC_REQ_AUDIO_TYPE_G711A,
    BYTE_RTC_REQ_AUDIO_TYPE_OPUS,
} byte_rtc_req_audio_type_e;

typedef enum {
    BYTE_RTC_REQ_VIDEO_TYPE_UNKNOE,
    BYTE_RTC_REQ_VIDEO_TYPE_MJPEG,
    BYTE_RTC_REQ_VIDEO_TYPE_H264,
} byte_rtc_req_video_type_e;

#define BYTE_RTC_DEFAULT_CONFIG() {                                                                  \
    .uri           = "https://api.coze.cn/v1/audio/rooms",                                           \
    .authorization = "Bearer pat_x3Q7vptWB4iMaHBBIjIEnrEbrOZDJJU98KeSsmnNB0912OdkEKa5x2JN71lEA6GZ",  \
    .bot_id        = "7439190997705965604",                                                          \
    .voice_id      = "7426720361733160969",                                                          \
    .audio_type    = BYTE_RTC_REQ_AUDIO_TYPE_G711A,                                                  \
    .video_type    = BYTE_RTC_REQ_VIDEO_TYPE_H264,                                                   \
}

typedef struct byte_rtc_req_result_s {
    char *room_id;
    char *app_id;
    char *token;
    char *uid;
} byte_rtc_req_result_t;

typedef struct {
    const char               *uri;
    const char               *authorization;
    const char               *bot_id;
    const char               *voice_id;
    byte_rtc_req_audio_type_e audio_type;
    byte_rtc_req_video_type_e video_type;
} byte_rtc_req_config_t;

byte_rtc_req_result_t *byte_rtc_request(byte_rtc_req_config_t *config);

void byte_rtc_request_free(byte_rtc_req_result_t *result);