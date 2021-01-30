#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"

#define UPDATE_FLAG_ADDRESS	0x0801F800
#define UPDATE_FLAG_PAGE	31
#define UPDATE_FLAG 		0x44
#define APP_ADDRESS			0x08008000 

_Bool should_update(void) 
{
	uint64_t read_flag = *(__IO uint64_t*)UPDATE_FLAG_ADDRESS;
	if (read_flag == UPDATE_FLAG) {
		printf("should update\n");
		return true;
	}
	
	printf("not update\n");
	return false;
}

_Bool check_update_file(void)
{
	return false;
}

static char file_handle[4];
_Bool get_file_handle_from_buffer(const char* buffer)
{
	printf("get_file_handle\n");
	char *open_str = "+QFOPEN:";
	const char* open_position = strstr(buffer, open_str);
	if (NULL == open_position) {
		printf("not found { \n");
		return false;
	}
	const char* open_end_posiont = open_position+strlen(open_str)+1;
	const char* end_position = strchr(open_end_posiont+1, '\r');
	strncpy(file_handle, open_end_posiont, end_position-open_end_posiont);
	for (int i = 0; i < sizeof(file_handle); ++i) {
		if (file_handle[i] == ' ') {
			for (int j = i; j < (sizeof(file_handle)-1); ++j) {
				file_handle[j] = file_handle[j+1];
			}
		}
	}
	printf("file:%s\n", file_handle);
	return true;
}

ErrorStatus isCorrectCommand(const char* buffer, const char* command)
{
	if (strstr(buffer, command) != NULL) {
		return SUCCESS;
	} else {
		return ERROR;
	}
}
#define TEMP_STRING_LENGTH	32
ErrorStatus isCorrectCommandFirst(const char* buffer, const char* command)
{
	char temp[TEMP_STRING_LENGTH];
	strncpy(temp, buffer, TEMP_STRING_LENGTH);
	if (strstr(temp, command) != NULL) {
		return SUCCESS;
	} else {
		return ERROR;
	}
}

#define LAST_LENGTH	10
ErrorStatus isCorrectCommandFromLast(const char* buffer, const char* command)
{
	int current_length = get_current_buffer_length();
	if (current_length > LAST_LENGTH) {
		const char* search_position = &buffer[current_length-LAST_LENGTH];
		if (strstr(search_position, command) != NULL) {
			return SUCCESS;
		} 
	} else {
		if (strstr(buffer, command) != NULL) {
			return SUCCESS;
		} 
	}
	return ERROR;
}

typedef void(*jump_func)(void);
void jump_to_app(void)
{
	printf("jump app\n");
	__disable_irq();

	uint32_t jump_address = *(__IO uint32_t*)(APP_ADDRESS+4);
	jump_func jump_to_app_func = (jump_func)jump_address;
	
	SysTick->CTRL = 0 ;

	if( CONTROL_SPSEL_Msk & __get_CONTROL( ) ) {
		/* MSP is not active */
		__set_CONTROL( __get_CONTROL( ) & ~CONTROL_SPSEL_Msk ) ;
	}
	__set_MSP(*(__IO uint32_t*)APP_ADDRESS);
	SCB->VTOR = APP_ADDRESS;
	jump_to_app_func();
	printf("jump app end\n");
}

char* get_file_handle_string() 
{
	return file_handle;
}
