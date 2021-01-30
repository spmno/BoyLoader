#include "at_read_file_state.h"
#include "utils.h"
#include "stdio.h"
#include "usart.h"

#define FILE_NAME 	"UFS:NewBoy2.hex"
#define READ_LENGTH	500

int at_list_file_action1()
{
	printf("send command list\n");
	send_at_command("AT+QFDWL=\"UFS:NewBoy2.hex\"\r\n");
	return 0;
}

action_result at_list_file_action2(const char *command_buffer)
{
	if (isCorrectCommand(command_buffer, "OK") == SUCCESS) {
		return ACTION_SUCCESS;
	} else {
		return ACTION_FAILED;
	}
}

int at_open_file_action1()
{
	char open_file_command[32];
	printf("send command open\n");
	sprintf(open_file_command, "AT+QFOPEN=%s, 2", FILE_NAME);
	send_at_command(open_file_command);
	return 0;
}

action_result at_open_file_action2(const char *command_buffer)
{
	if ((isCorrectCommand(command_buffer, "OK") == SUCCESS) || (isCorrectCommand(command_buffer, "426")) == SUCCESS) {
		get_file_handle_from_buffer(command_buffer);
		return ACTION_SUCCESS;
	} else {
		return ACTION_FAILED;
	}
}

int at_read_file_action1()
{
	char read_file_command[32];
	printf("command read\n");
	sprintf(read_file_command, "AT+QFREAD=%s,500", get_file_handle_string());
	send_at_command(read_file_command);
	return 0;
}

static _Bool last_frame_flag = false;
action_result at_read_file_action2(const char *command_buffer)
{
	printf("receive data\n");
	if (isCorrectCommandFirst(command_buffer, "CONNECT") == SUCCESS) {
		if (isCorrectCommandFirst(command_buffer, "500") == SUCCESS) {

		} else {
			last_frame_flag = true;
		}
		return ACTION_WAIT_AGAIN;
	} else if (isCorrectCommandFromLast(command_buffer, "OK") == SUCCESS) {
		if (!last_frame_flag) {
			return ACTION_REPEAT;
		} else {
			last_frame_flag = false;
			return ACTION_SUCCESS;
		}
	} else {
		return ACTION_FAILED;
	}
}

int at_close_file_action1(void)
{
	char close_file_command[32];
	printf("command close\n");
	sprintf(close_file_command, "AT+QFCLOSE=%s", get_file_handle_string());
	send_at_command(close_file_command);
	return 0;
}

action_result at_close_file_action2(const char *command_buffer)
{
	
	if (isCorrectCommand(command_buffer, "OK") == SUCCESS) {
		return ACTION_COMPLETED;
	} else {
		return ACTION_FAILED;
	}
}

