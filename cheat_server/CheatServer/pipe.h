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

// REQ, REP struct must define opposite order
// e.g) REQ -> REP, REP -> REQ
typedef struct _REPLY_HEADER {
	uint8_t type;
	uint32_t address;
	char buffer[256];
}REPLY_HEADER, *PREPLY_HEADER;

typedef struct _REQUEST_HEADER {
	uint8_t type;
	uint32_t address;
	uint32_t return_value;
}REQUEST_HEADER, *PREQUEST_HEADER;
#pragma pack(pop)