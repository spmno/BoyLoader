#include "at_statemachine.h"
#include "usart.h"
#include "at_read_file_state.h"

static at_fsm_state_t *current_at_state_list;
static nbiot_fsm_state_index_t nbiot_fsm_state_index;
static int current_state_index = 0;

static const at_fsm_state_t file_at_state_list[] = {
	{STATE_QUERY_LIST,       		3,  1000,	300, at_list_file_action1,         at_list_file_action2	},
	{STATE_OPEN_FILE,     			3,  3000,	300, at_open_file_action1,       	at_open_file_action2	},
	{STATE_READ_FILE,				3,  3000,	300, at_read_file_action1,  		at_read_file_action2	},
	{STATE_CLOSE_FILE,				3,  3000,	300, at_close_file_action1,  		at_close_file_action2	},
};

nbiot_fsm_state_index_t* get_current_state_index()
{
	return &nbiot_fsm_state_index;
}


void jump_to_next_at_statemachine() 
{
	current_state_index++;
	nbiot_fsm_state_index.cur_state = current_at_state_list[current_state_index].cur_state;
	nbiot_fsm_state_index.init = 1;
	nbiot_fsm_state_index.trycnt = 0;
	nbiot_fsm_state_index.fsm_state = &current_at_state_list[current_state_index];
}

void init_statemachine(void)
{
	current_at_state_list = (at_fsm_state_t *)file_at_state_list;
	nbiot_fsm_state_index.cur_state = STATE_QUERY_LIST;
	nbiot_fsm_state_index.init = 1;
	nbiot_fsm_state_index.trycnt = 0;
	nbiot_fsm_state_index.fsm_state = current_at_state_list;
	current_state_index = 0;
}
