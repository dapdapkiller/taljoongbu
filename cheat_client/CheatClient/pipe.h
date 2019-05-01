#include <stdint.h>
#pragma once

#pragma pack(push,1)
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
	char buffer[256];
}REQUEST_HEADER, *PREQUEST_HEADER;

typedef struct _REPLY_HEADER {
	uint8_t type;
	uint32_t address;
	uint32_t return_value;
}REPLY_HEADER, *PREPLY_HEADER;
#pragma pack(pop)