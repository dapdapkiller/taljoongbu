#pragma once
#include <stdint.h>

#define RECV_PIPE_NAME "\\\\.\\pipe\\CheatRecv"
#define SEND_PIPE_NAME "\\\\.\\pipe\\CheatSend"

#pragma pack(push,1)
typedef enum _CHEAT_LIST {
	SetTargetHandle = 0,
	GetServerState,
	ScriptInfiniteOn,
	ScriptMobVacOn,
	ScriptAbilityOn,
	ScriptInfiniteOff,
	ScriptMobVacOff,
	ScriptAbilityOff
}CHEAT_LIST;

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