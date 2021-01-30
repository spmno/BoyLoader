#include "stm32g4xx_hal.h"
#include "stdbool.h"

ErrorStatus isCorrectCommand(const char* buffer, const char* command);

_Bool should_update(void);

void jump_to_app(void);

ErrorStatus isCorrectCommandFirst(const char* buffer, const char* command);

ErrorStatus isCorrectCommandFromLast(const char* buffer, const char* command);

_Bool get_file_handle_from_buffer(const char* buffer);

char* get_file_handle_string(void);
