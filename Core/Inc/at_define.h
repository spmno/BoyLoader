#ifndef _AT_DEFINE_H_
#define _AT_DEFINE_H_

#include "stm32g4xx_hal.h"

#define AT_DATA_FLAG	1U
#define AT_IDLE_FLAG	2U
#define AT_RESET_FLAG	4U
#define AT_WAIT_FLAG	(AT_DATA_FLAG|AT_IDLE_FLAG|AT_RESET_FLAG)

#define AT_BUFFER_SIZE 1200

typedef enum
{
	STATE_QUERY_LIST,
	STATE_OPEN_FILE,
	STATE_READ_FILE,
	STATE_CLOSE_FILE,
	STATE_LENGTH
} nbiot_state_e;

typedef enum
{
  ACTION_SUCCESS= 0x01,  
  ACTION_FAILED,
  ACTION_REPEAT,
  ACTION_WAIT_AGAIN,
  ACTION_COMPLETED,
} action_result;

typedef struct 
{
    nbiot_state_e cur_state;  
    int  try_cnt;              
    int  wait_time;           
	int delay_time;
    int (*action1)(void);      
    action_result (*action2)(const char *command_buffer); 
} at_fsm_state_t;

typedef struct
{
    int cur_state;  
    int trycnt;     
    uint32_t init;  
    const at_fsm_state_t *fsm_state; 
} nbiot_fsm_state_index_t;

void at_download_file_task(void *argument);

#endif
