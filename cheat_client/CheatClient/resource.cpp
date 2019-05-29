#include <iostream>
#include <windows.h>
#include <string>
#include <regex>
#include "pipe.h"
#include "resource.h"
#include "Utility.h"

#pragma warning (disable:4996)

constexpr auto PIPE_NAME = "\\\\.\\pipe\\CheatPipe";
constexpr auto MAX_BUFFER = 256;

BOOL CALLBACK DialogProc(HWND arg1, UINT arg2, WPARAM wParam, LPARAM lParam);
LPTHREAD_START_ROUTINE ThreadProc(LPVOID lParam);

HANDLE hClientPipe = 0;
static bool dialog_state = true;
static char temp_process_buffer[256] = { 0, };
std::string command(MAX_BUFFER, '\0');
std::string command_list(MAX_BUFFER, '\0');

int main_before(int argc, char** argv)
{
	HANDLE hTargetProcess = INVALID_HANDLE_VALUE;
	HANDLE MyThread;

	MyThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, NULL, NULL);
	if (!set_privilege((LPSTR)("SeDebugPrivilege")))
	{
		ERROR_MACRO("set_privilege");
		ExitProcess(0);
	}
	
	while (1)
	{
		if (!dialog_state)
		{
			ExitProcess(0);
		}
		SwitchToThread();
	}

	hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)atoi(argv[1]));
	std::cout << "Target Handle : " << hTargetProcess << std::endl;
	while (1)
	{

	}
	
	return 0;
}

LPTHREAD_START_ROUTINE ThreadProc(LPVOID lParam) {
	HWND myDlg = 0;
	MSG Msg;

	myDlg = CreateDialogParamA(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), GetDesktopWindow(), (DLGPROC)DialogProc, NULL);
	//ShowWindow(FindConsoleHandle(), SW_HIDE); // hide conolse -> DebugMode
	ShowWindow(myDlg, SW_SHOW); // show dialog
	UpdateWindow(myDlg);

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		if (!IsDialogMessage(myDlg, &Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		// [ ÆÄ½Ì Çü½Ä ]
		// ex) !process 0x1234 5678
		std::regex get_command("!([a-z]+) 0x([0-9]+) {0,}([0-9]*)");
//		std::regex get_command("!([^ ]*) 0x([^ ]*) {0,}([^ ]*)");
//		std::regex get_command("!([a-z]+)\\s0x([0-9]+)\\s*([0-9]+){0,}");
		std::smatch base_match;

		if (std::regex_search(command, base_match, get_command))
		{
			std::ssub_match match_list[3];
			for (int i = 0; i < 3; i++)
			{
				match_list[i] = base_match[i];
			}
			std::string cmd = match_list[0].str();
			std::string arg1 = base_match[1].str();
			std::string arg2 = base_match[2].str();
			command_list.find(cmd);
		}
	}
	return 0;
}

BOOL CALLBACK DialogProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static HANDLE hSCMgr;
	switch (iMessage) {
		case WM_INITDIALOG:
			hSCMgr = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			break;
		case WM_COMMAND:
			switch (wParam) {
			// TODO: DriverLoader
			case IDC_DRIVER_IMAGE:
				if (!DriverLoader(hSCMgr, (LPSTR)"MitiBypass", (LPSTR)"MitiKiller.sys"))
					ERROR_MACRO("DriverLoader");
				if (!DriverLoader(hSCMgr, (LPSTR)"VulnDriver", (LPSTR)"Vuln.sys"))
					ERROR_MACRO("DriverLoader");
				// Call create dispatch
				CreateFileA("\\\\.\\MitiLinker",
							GENERIC_READ | GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);
				break;
			case IDC_DLL_IMAGE:
				// TODO: Dll Injector
				DllLoader((LPSTR)"CheatServer.dll", (LPSTR)"csrss.exe");
				break;
			case IDC_BUTTON1:
					GetDlgItemTextA(hWnd, IDC_EDIT1, const_cast<char *>(command.c_str()), 256);
					break;
			case IDC_PRCFIND:
					GetDlgItemTextA(hWnd, IDC_PRCNAME, temp_process_buffer, 256);
					break;
			case IDC_ECHOCRT:
				if (!(hClientPipe = CreateFile(
					PIPE_NAME,
					GENERIC_READ | GENERIC_WRITE,
					0,
					NULL,
					OPEN_EXISTING,
					0,
					NULL
				)))
				{
					ERROR_MACRO("CreateFile");
				}
			break;
			case IDC_ECHOSEND:					
				REQUEST_HEADER req;
				req.type = CHEAT_LIST::GetTargetHandle;
				req.address = 0;
				memset(req.buffer, 0, 256);
				memcpy(req.buffer, temp_process_buffer, strlen(temp_process_buffer));
				if (!WriteFile(hClientPipe, &req, sizeof(REQUEST_HEADER), NULL, NULL)) ERROR_MACRO("WriteFile");
				break;
			case IDC_ECHOREAD:
				REPLY_HEADER rep;
				char temp_buffer[32];
				if (!ReadFile(hClientPipe, &rep, sizeof(REPLY_HEADER), NULL, NULL)) ERROR_MACRO("ReadFile");
				sprintf(temp_buffer, "ProcessHandle : %x\n", rep.return_value);
				MessageBox(NULL, temp_buffer, "Nice", MB_OK);
				break;
			} 
			break;
		case WM_CLOSE:
			CloseHandle(hSCMgr);
			CloseHandle(hClientPipe);
			EndDialog(hWnd, 0);
			dialog_state = false;
			break;
	}
	return false;
}

