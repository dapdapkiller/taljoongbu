#include <stdint.h>
#pragma once

typedef enum _CHEAT_LIST {
	GetTargetHandle = 0,
	Read = 1,
	Write,
	ScriptOn,
	ScriptOff
}CHEAT_LIST; // use by type

typedef struct _REQUEST_HEADER {
	uint8_t type;
	uint32_t address;
}REQUEST_HEADER, *PREQUEST_HEADER;

typedef struct _REPLY_HEADER {
	uint8_t type;
	uint32_t address;
	uint32_t return_buffer;
}REPLY_HEADER, *PREPLY_HEADER;