#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_task_info.h"
#include "esp_random.h"

#include <VolcEngineRTCLite.h>
// #include "opus_frames.h"
#include "freertos/semphr.h"
#include "esp_err.h"
// #include "esp_littlefs.h"
// #include "config.h"
#include "sdkconfig.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "audio_sys.h"
#include "board.h"
#include "esp_peripherals.h"
#include "periph_sdcard.h"
#include "fatfs_stream.h"
#include "i2s_stream.h"
#include "AudioPipeline.h"
#include "byte_rtc_request.h"

#define STATS_TASK_PRIO     5

static const char* TAG = "VolcRTCDemo";
static bool joined = false;

#define USE_HTTP_REQ (1)

// #define DEFAULT_APPID "671221aa298a540183df32d9"
// #define DEFAULT_ROOMID "CozeRTC_H264_G711A_7451895241524920383"
// #define DEFAULT_UID "uid_7451895241524903999"
// #define DEFAULT_TOKEN "001671221aa298a540183df32d9bQAv86sE6H1qZ2jPa2cmAENvemVSVENfSDI2NF9HNzExQV83NDUxODk1MjQxNTI0OTIwMzgzFwB1aWRfNzQ1MTg5NTI0MTUyNDkwMzk5OQUAAAAAAAAAAQAAAAAAAgAAAAAAAwAAAAAABAAAAAAAIAAIkY6lbaY0BL6tYa+AKIcMV6y8r2r1K+mpikEOzRERTQ=="

// byte rtc lite callbacks
static void byte_rtc_on_join_room_success(byte_rtc_engine_t engine, const char* channel, int elapsed_ms, bool ms) {
    ESP_LOGI(TAG, "join channel success %s elapsed %d ms now %d ms\n", channel, elapsed_ms, elapsed_ms);
    joined = true;
};

static void byte_rtc_on_rejoin_room_success(byte_rtc_engine_t engine, const char* channel, int elapsed_ms){
    // g_byte_rtc_data.channel_joined = TRUE;
    ESP_LOGI(TAG, "rejoin channel success %s\n", channel);
};

static void byte_rtc_on_user_joined(byte_rtc_engine_t engine, const char* channel, const char* user_name, int elapsed_ms){
    ESP_LOGI(TAG, "remote user joined  %s:%s\n", channel, user_name);
};

static void byte_rtc_on_user_offline(byte_rtc_engine_t engine, const char* channel, const char* user_name, int reason){
    ESP_LOGI(TAG, "remote user offline  %s:%s\n", channel, user_name);
};

static void byte_rtc_on_user_mute_audio(byte_rtc_engine_t engine, const char* channel, const char* user_name, int muted){
    ESP_LOGI(TAG, "remote user mute audio  %s:%s %d\n", channel, user_name, muted);
};

static void byte_rtc_on_user_mute_video(byte_rtc_engine_t engine, const char* channel, const char* user_name, int muted){
    ESP_LOGI(TAG, "remote user mute video  %s:%s %d\n", channel, user_name, muted);
};

static void byte_rtc_on_connection_lost(byte_rtc_engine_t engine, const char* channel){
    ESP_LOGI(TAG, "connection Lost  %s\n", channel);
};

static void byte_rtc_on_room_error(byte_rtc_engine_t engine, const char* channel, int code, const char* msg){
    ESP_LOGI(TAG, "error occur %s %d %s\n", channel, code, msg?msg:"");
};

// remote audio
static void byte_rtc_on_audio_data(byte_rtc_engine_t engine, const char* channel, const char*  uid , uint16_t sent_ts,
                      audio_codec_type_e codec, const void* data_ptr, size_t data_len){
    // ESP_LOGI(TAG, "byte_rtc_on_audio_data... len %d\n", data_len);
    player_pipeline_handle_t player_pipeline = (player_pipeline_handle_t) byte_rtc_get_user_data(engine);
    player_pipeline_write(player_pipeline, data_ptr, data_len);

}

// remote video
static void byte_rtc_on_video_data(byte_rtc_engine_t engine, const char*  channel, const char* uid, uint16_t sent_ts,
                      video_data_type_e codec, int is_key_frame,
                      const void * data_ptr, size_t data_len){
    ESP_LOGI(TAG, "byte_rtc_on_video_data... len %d\n", data_len);
}

// remote message
void on_message_received(byte_rtc_engine_t engine, const char*  room, const char* uid, const uint8_t* message, int size, bool binary) {
    ESP_LOGI(TAG, "on_message_received uid: %s, message size: %d", uid, size);
}

static void on_key_frame_gen_req(byte_rtc_engine_t engine, const char*  channel, const char*  uid) {}
// byte rtc lite callbacks end.

static void byte_rtc_task(void *pvParameters) {
    // start audio capture & play
#if  1//USE_HTTP_REQ
    byte_rtc_req_config_t req_config = BYTE_RTC_DEFAULT_CONFIG();
    byte_rtc_req_result_t *req_result = byte_rtc_request(&req_config);
    // byte_rtc_req_result_t *req_result = malloc(16);
    // req_result->room_id = "CozeRTC_H264_G711A_7451938261737635855";
    // req_result->app_id = "671221aa298a540183df32d9";
    // req_result->token = "001671221aa298a540183df32d9bQBT6i8Et7JqZzcEbGcmAENvemVSVENfSDI2NF9HNzExQV83NDUxOTM4MjYxNzM3NjM1ODU1FwB1aWRfNzQ1MTkzODI2MTczNzYxOTQ3MQUAAAAAAAAAAQAAAAAAAgAAAAAAAwAAAAAABAAAAAAAIAAbBDztKRWVvDa7bGdVynO2k7ms/KNVtXMtxL0XAK6xcQ==";
    // req_result->uid = "uid_7451938261737619471";

#if 1
#else
    #define  room_id "CozeRTC_H264_G711A_7451938261737603087"
    #define app_id "671221aa298a540183df32d9"
    #define token "001671221aa298a540183df32d9bQD7B/QEvbBqZz0CbGcmAENvemVSVENfSDI2NF9HNzExQV83NDUxOTM4MjYxNzM3NjAzMDg3FwB1aWRfNzQ1MTkzODI2MTczNzU4NjcwMwUAAAAAAAAAAQAAAAAAAgAAAAAAAwAAAAAABAAAAAAAIAAZOap7NeenmgKsv0C/OWsYBfw9v8j+houBBIitzCSv/A=="
    #define uid "uid_7451938261737586703"
#endif
    // byte_rtc_req_result_t *req_result = byte_rtc_request(&req_config);
    assert(req_result);
    // printf("room_id :%s\n", room_id);
    // printf("app_id :%s\n", app_id);
    // printf("token :%s\n", token);
    // printf("uid :%s\n", uid);
#endif

    recorder_pipeline_handle_t pipeline = recorder_pipeline_open();
    player_pipeline_handle_t player_pipeline = player_pipeline_open();
    recorder_pipeline_run(pipeline);
    player_pipeline_run(player_pipeline);

    byte_rtc_event_handler_t handler = {
        .on_join_room_success       =   byte_rtc_on_join_room_success,
        .on_room_error              =   byte_rtc_on_room_error,
        .on_user_joined             =   byte_rtc_on_user_joined,
        .on_user_offline            =   byte_rtc_on_user_offline,
        .on_user_mute_audio         =   byte_rtc_on_user_mute_audio,
        .on_user_mute_video         =   byte_rtc_on_user_mute_video,
        .on_audio_data              =   byte_rtc_on_audio_data,
        .on_video_data              =   byte_rtc_on_video_data,
        .on_key_frame_gen_req       =   on_key_frame_gen_req,
        .on_message_received        =   on_message_received,
    };

#if USE_HTTP_REQ
    byte_rtc_engine_t engine = byte_rtc_create(req_result->app_id, &handler);
#else
    byte_rtc_engine_t engine = byte_rtc_create(DEFAULT_APPID, &handler);
#endif
    byte_rtc_set_log_level(engine, BYTE_RTC_LOG_LEVEL_ERROR);
#ifdef RTC_TEST_ENV
    byte_rtc_set_params(engine, "{\"env\":2}"); // test env
#endif
    // byte_rtc_set_params(engine, "{\"rtc\":{\"root_path\":\"/littlefs\"}}");
    // byte_rtc_config_log(engine, NULL, 1024 * 200, 8);
    byte_rtc_set_params(engine, "{\"debug\":{\"log_to_console\":1}}"); 

    byte_rtc_init(engine);
    byte_rtc_set_audio_codec(engine, AUDIO_CODEC_TYPE_G711A);
    byte_rtc_set_video_codec(engine, VIDEO_CODEC_TYPE_H264);

    byte_rtc_set_user_data(engine, player_pipeline);

    byte_rtc_room_options_t options;
    options.auto_subscribe_audio = 1; // 接收远端音频
    options.auto_subscribe_video = 0; // 不接收远端视频
#if USE_HTTP_REQ
    byte_rtc_join_room(engine, req_result->room_id, req_result->uid, req_result->token, &options);
#else
    byte_rtc_join_room(engine, DEFAULT_ROOMID, DEFAULT_UID, DEFAULT_TOKEN, &options);
#endif

    const int DEFAULT_READ_SIZE = recorder_pipeline_get_default_read_size(pipeline);
    uint8_t *audio_buffer = heap_caps_malloc(DEFAULT_READ_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!audio_buffer) {
        ESP_LOGE(TAG, "Failed to alloc audio buffer!");
        return;
    }

    while (true) {
        int ret =  recorder_pipeline_read(pipeline, (char*) audio_buffer, DEFAULT_READ_SIZE);
        if (ret == DEFAULT_READ_SIZE && joined) {
            // push_audio data
            audio_frame_info_t audio_frame_info = {.data_type = AUDIO_DATA_TYPE_PCMA};
        #if USE_HTTP_REQ
            int l = byte_rtc_send_audio_data(engine, req_result->room_id, audio_buffer, DEFAULT_READ_SIZE, &audio_frame_info);
            // printf("send audio buffer len %d\n", l);
        #else
            byte_rtc_send_audio_data(engine, DEFAULT_ROOMID, audio_buffer, DEFAULT_READ_SIZE, &audio_frame_info);
        #endif 
        }
    }

    byte_rtc_fini(engine);
    usleep(1000 * 1000);
    byte_rtc_destory(engine);

    recorder_pipeline_close(pipeline);
    player_pipeline_close(player_pipeline);
    ESP_LOGI(TAG, "............. finished\n");
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());

    ESP_LOGI(TAG, "[ 1 ] Mount sdcard");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

    // Initialize SD Card peripheral
    audio_board_sdcard_init(set, SD_MODE_1_LINE);

    audio_board_handle_t board_handle = audio_board_init();   
    // audio_hal = board_handle->audio_hal;
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);
    ESP_LOGI(TAG, "[2.1] audio_board & audio_board_key init");

    audio_board_key_init(set);  //在audio_hal.h中修改默认音量AUDIO_HAL_VOL_DEFAULT  90    // ESP_LOGI(TAG, "[3.1] Create fatfs stream to write data to sdcard");
    
    // fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    // fatfs_cfg.type = AUDIO_STREAM_WRITER;
    // fatfs_stream_writer = fatfs_stream_init(&fatfs_cfg);
    ESP_LOGI(TAG, "Starting again!\n");

    //Allow other core to finish initialization
    vTaskDelay(pdMS_TO_TICKS(100));

    //Create and start stats task
    xTaskCreate(&byte_rtc_task, "byte_rtc_task", 8192, NULL, STATS_TASK_PRIO, NULL);
}

/////////////////////////////////////////////////////////////////////////////////
// print task cpu info.
// #define NUM_OF_SPIN_TASKS   6
// #define ARRAY_SIZE_OFFSET   5   //Increase this if print_real_time_stats returns ESP_ERR_INVALID_SIZE
// #define STATS_TICKS         pdMS_TO_TICKS(1000)
// static char task_names[NUM_OF_SPIN_TASKS][configMAX_TASK_NAME_LEN];
// static SemaphoreHandle_t sync_spin_task;
// static SemaphoreHandle_t sync_stats_task;

// static esp_err_t print_real_time_stats(TickType_t xTicksToWait)
// {
//     TaskStatus_t *start_array = NULL, *end_array = NULL;
//     UBaseType_t start_array_size, end_array_size;
//     configRUN_TIME_COUNTER_TYPE start_run_time, end_run_time;
//     esp_err_t ret;

//     //Allocate array to store current task states
//     start_array_size = uxTaskGetNumberOfTasks() + ARRAY_SIZE_OFFSET;
//     start_array = malloc(sizeof(TaskStatus_t) * start_array_size);
//     if (start_array == NULL) {
//         ret = ESP_ERR_NO_MEM;
//         goto exit;
//     }
//     //Get current task states
//     start_array_size = uxTaskGetSystemState(start_array, start_array_size, &start_run_time);
//     if (start_array_size == 0) {
//         ret = ESP_ERR_INVALID_SIZE;
//         goto exit;
//     }

//     vTaskDelay(xTicksToWait);

//     //Allocate array to store tasks states post delay
//     end_array_size = uxTaskGetNumberOfTasks() + ARRAY_SIZE_OFFSET;
//     end_array = malloc(sizeof(TaskStatus_t) * end_array_size);
//     if (end_array == NULL) {
//         ret = ESP_ERR_NO_MEM;
//         goto exit;
//     }
//     //Get post delay task states
//     end_array_size = uxTaskGetSystemState(end_array, end_array_size, &end_run_time);
//     if (end_array_size == 0) {
//         ret = ESP_ERR_INVALID_SIZE;
//         goto exit;
//     }

//     //Calculate total_elapsed_time in units of run time stats clock period.
//     uint32_t total_elapsed_time = (end_run_time - start_run_time);
//     if (total_elapsed_time == 0) {
//         ret = ESP_ERR_INVALID_STATE;
//         goto exit;
//     }

//     ESP_LOGI(TAG, "| Task | Run Time | Percentage\n");
//     //Match each task in start_array to those in the end_array
//     for (int i = 0; i < start_array_size; i++) {
//         int k = -1;
//         for (int j = 0; j < end_array_size; j++) {
//             if (start_array[i].xHandle == end_array[j].xHandle) {
//                 k = j;
//                 //Mark that task have been matched by overwriting their handles
//                 start_array[i].xHandle = NULL;
//                 end_array[j].xHandle = NULL;
//                 break;
//             }
//         }
//         //Check if matching task found
//         if (k >= 0) {
//             uint32_t task_elapsed_time = end_array[k].ulRunTimeCounter - start_array[i].ulRunTimeCounter;
//             uint32_t percentage_time = (task_elapsed_time * 100UL) / (total_elapsed_time * portNUM_PROCESSORS);

//             if (strcmp(start_array[i].pcTaskName, "pthread") == 0 || strcmp(start_array[i].pcTaskName, "byte_rtc_task") == 0 || percentage_time >= 8) 
//             {
//                 ESP_LOGE(TAG, "| %s | %"PRIu32" | %"PRIu32"%%", start_array[i].pcTaskName, task_elapsed_time, percentage_time);
//             }
//         }
//     }

//     //Print unmatched tasks
//     for (int i = 0; i < start_array_size; i++) {
//         if (start_array[i].xHandle != NULL) {
//             ESP_LOGI(TAG, "| %s | Deleted\n", start_array[i].pcTaskName);
//         }
//     }
//     for (int i = 0; i < end_array_size; i++) {
//         if (end_array[i].xHandle != NULL) {
//             ESP_LOGI(TAG, "| %s | Created\n", end_array[i].pcTaskName);
//         }
//     }
//     ret = ESP_OK;

// exit:    //Common return path
//     free(start_array);
//     free(end_array);
//     return ret;
// }

// static void stats_task(void *arg) {
//     //Print real time stats periodically
//     while (1) {
//         ESP_LOGI(TAG, "\nGetting real time stats over %"PRIu32" ticks\n", STATS_TICKS);
//         if (print_real_time_stats(STATS_TICKS) == ESP_OK) {
//             ESP_LOGI(TAG, "Real time stats obtained\n");
//         } else {
//             ESP_LOGI(TAG, "Error getting real time stats\n");
//         }
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }

// void start_stats_task() {
//     xTaskCreatePinnedToCore(stats_task, "stats", 4096, NULL, 3, NULL, tskNO_AFFINITY);
// }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// print heap info
// #define MAX_TASK_NUM 20                         // Max number of per tasks info that it can store
// #define MAX_BLOCK_NUM 20                        // Max number of per block info that it can store

// static size_t s_prepopulated_num = 0;
// static heap_task_totals_t s_totals_arr[MAX_TASK_NUM];
// static heap_task_block_t s_block_arr[MAX_BLOCK_NUM];

// static void esp_dump_per_task_heap_info(void)
// {
    // heap_task_info_params_t heap_info = {0};
    // heap_info.caps[0] = MALLOC_CAP_8BIT;        // Gets heap with CAP_8BIT capabilities
    // heap_info.mask[0] = MALLOC_CAP_8BIT;
    // heap_info.caps[1] = MALLOC_CAP_32BIT;       // Gets heap info with CAP_32BIT capabilities
    // heap_info.mask[1] = MALLOC_CAP_32BIT;
    // heap_info.tasks = NULL;                     // Passing NULL captures heap info for all tasks
    // heap_info.num_tasks = 0;
    // heap_info.totals = s_totals_arr;            // Gets task wise allocation details
    // heap_info.num_totals = &s_prepopulated_num;
    // heap_info.max_totals = MAX_TASK_NUM;        // Maximum length of "s_totals_arr"
    // heap_info.blocks = s_block_arr;             // Gets block wise allocation details. For each block, gets owner task, address and size
    // heap_info.max_blocks = MAX_BLOCK_NUM;       // Maximum length of "s_block_arr"

    // heap_caps_get_per_task_info(&heap_info);

    // for (int i = 0 ; i < *heap_info.num_totals; i++) {
    //     ESP_LOGI(TAG, "Task: %s -> CAP_8BIT: %d CAP_32BIT: %d\n",
    //             heap_info.totals[i].task ? pcTaskGetName(heap_info.totals[i].task) : "Pre-Scheduler allocs" ,
    //             heap_info.totals[i].size[0],    // Heap size with CAP_8BIT capabilities
    //             heap_info.totals[i].size[1]);   // Heap size with CAP32_BIT capabilities
    // }

    // ESP_LOGI(TAG, "\n");
// }