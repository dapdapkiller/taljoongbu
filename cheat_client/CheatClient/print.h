#pragma once
#include <iostream> // Use std::cout, std::endl
#include <string>	// Use std::String
#include <atlstr.h>	// Use ATL::CString
#include <bcrypt.h>	// Use NTSTATUS

// LOGING_CONSOLE = 1 인 경우 콘솔창에 로그 출력
// LOGING_CONSOLE = 0 인 경우 GUI 화면에 로그 출력
#define LOGING_CONSOLE 0

// Insert a string message.
void println(std::string msg);
// If you have an error code, add the second argument.
void println(std::string msg, DWORD errcode);
// If you have an hexa code, add the second argument.
void println(std::string msg, NTSTATUS hexcode);