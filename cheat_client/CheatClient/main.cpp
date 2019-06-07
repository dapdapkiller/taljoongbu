#pragma once
#include <nanogui/nanogui.h>
#include <iostream>
#include <list>
#include "gui.h"
#include "Utility.h"

using namespace nanogui;
using namespace std;

HANDLE hClientPipe = 0;
HANDLE hSCMgr = 0;

int main(int /* argc */, char ** /* argv */) {
	nanogui::init();
	{
		// create a fixed size screen with one window
		Screen *screen = new Screen({ width, height }, "PPL Bypass", false);
		Window *window = new Window(screen, "");
		window->setPosition({ 0, 0 });
		window->setFixedSize({ width, height });

		// Background Image
		GLTexture texture("");
		auto data = texture.load(imageDirectory + "background.png");
		using imagesDataType = vector<pair<GLTexture, GLTexture::handleType>>;
		imagesDataType mImagesData;
		mImagesData.emplace_back(std::move(texture), std::move(data));

		auto imageView = new ImageView(window, mImagesData[0].first.texture());
		imageView->setPosition({ 0, 0 });
		imageView->setFixedSize({ width, height });
		imageView->setFixedScale(true);

		// attach a vertical scroll panel
		auto vscroll = new VScrollPanel(window);
		vscroll->setFixedSize({ width, height });

		// vscroll should only have *ONE* child. this is what `wrapper` is for
		auto wrapper = new Widget(vscroll);
		wrapper->setFixedSize({ width, height });

		////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////

		// TextBox
		TextBox *textBox = new TextBox(wrapper, "MapleStory.exe");
		textBox->setEditable(true);
		textBox->setFontSize(30);
		textBox->setFixedSize({ width_50per, height_10per });
		textBox->setFormat("[a-z A-Z 0-9]+.exe");
		textBox->setPosition({ width_20per, 25 });
		textBox->requestFocus();

		// Submit button
		list<Button*> buttonList;
		Button *button;
		ADD_BUTTON("submit", 28, width_10per, height_10per, width_50per + width_20per + 10, 25, false);
		button->setCallback([&buttonList, textBox] { processName = textBox->value().c_str(); button_call1(buttonList, (int)SUBMIT, (int)CONNECTION); }); //  수정 필요

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		NVGcontext *temp = screen->nvgContext();
		ImagePanel *imgPanel;
		vector<pair<int, string>> icons;
		TextBox *logview;

		// Driver Load button
		ADD_BUTTON("          Driver Load", 33, width_30per, height_20per, width_2_5per, height_20per, true);
		button->setTooltip("Loading Vulnerable Drivers");
		button->setCallback([&buttonList, &logview] { logview->setValue(""); button_call1(buttonList, (int)DRIVER_LOAD, (int)DLL_INJECTION); });

		ADD_IMAGE(imageDirectory + "0/", width_2_5per, height_20per + (int)((height_20per - 81) / 2), true);
		imgPanel->setCallback([&buttonList, &logview](int i) { logview->setValue("");  if (button_check(buttonList, (int)DRIVER_LOAD, (int)DLL_INJECTION)) button_call1(buttonList, (int)DRIVER_LOAD, (int)DLL_INJECTION); });


		// DLL Injection buttion
		ADD_BUTTON("           DLL Injection", 32, width_30per, height_20per, width_30per + width_2_5per * 2, height_20per, false);
		button->setTooltip("User DLL injection into csrs.exe process");
		button->setCallback([&buttonList] { button_call1(buttonList, (int)DLL_INJECTION, (int)SUBMIT); });

		ADD_IMAGE(imageDirectory + "1/", width_2_5per * 2 + width_30per, height_20per + (int)((height_20per - 81) / 2), false);
		imgPanel->setCallback([&buttonList](int i) { if (button_check(buttonList, (int)DLL_INJECTION, (int)SUBMIT)) button_call1(buttonList, (int)DLL_INJECTION, (int)SUBMIT); });


		// Connection button
		ADD_BUTTON("          Connection", 34, width_30per, height_20per, width_30per * 2 + width_2_5per * 3, height_20per, false);
		button->setTooltip("This button has a fairly long tooltip. It is so long, in ");
		button->setCallback([&buttonList] { button_call1(buttonList, (int)CONNECTION, (int)TOGGLEO1); });

		ADD_IMAGE(imageDirectory + "2/", width_2_5per * 3 + width_30per * 2, height_20per + (int)((height_20per - 81) / 2), false);
		imgPanel->setCallback([&buttonList](int i) { if (button_check(buttonList, (int)CONNECTION, (int)TOGGLEO1)) button_call1(buttonList, (int)CONNECTION, (int)TOGGLEO1); });

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Toggle button
		ADD_BUTTON("Script1(Infinite)", 30, width_30per, height_15per, width_2_5per, height_47per, false);
		button->setFlags(Button::ToggleButton);
		button->setBackgroundColor(Color(0, 0, 255, 25));
		button->setChangeCallback([&buttonList](bool state) { button_call1(buttonList, (int)TOGGLEO1, (int)state); });

		ADD_BUTTON("Script2(MobVac)", 30, width_30per, height_15per, width_2_5per, height_47per + height_15per + height_1_25per, false);
		button->setFlags(Button::ToggleButton);
		button->setBackgroundColor(Color(0, 0, 255, 25));
		button->setChangeCallback([](bool state) { cout << "Script2(MobVac) state: " << state << endl; });
		button->setChangeCallback([&buttonList](bool state) { button_call1(buttonList, (int)TOGGLEO2, (int)state); });

		ADD_BUTTON("Script3(Ability)", 30, width_30per, height_15per, width_2_5per, height_47per + height_15per * 2 + height_1_25per * 2, false);
		button->setFlags(Button::ToggleButton);
		button->setBackgroundColor(Color(0, 0, 255, 25));
		button->setChangeCallback([](bool state) { cout << "Script3(Ability) state: " << state << endl; });
		button->setChangeCallback([&buttonList](bool state) { button_call1(buttonList, (int)TOGGLEO3, (int)state); });

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Log View
		logview = new TextBox(wrapper, "");
		logview->setFontSize(20);
		logview->setValue("Log View");
		logview->setFixedSize({ width - (width_30per + width_2_5per * 3), height_47per });
		logview->setPosition({ width_30per + width_2_5per * 2, height_47per });
		
		vscroll_log = new VScrollPanel(window);
		vscroll_log->setFixedSize({ width - (width_30per + width_2_5per * 3), height_47per });
		vscroll_log->setPosition({ width_30per + width_2_5per * 2, height_47per });

		wrapper_log = new Widget(vscroll_log);
		wrapper_log->setFixedSize({ width - (width_30per + width_2_5per * 3), height_47per });
		wrapper_log->setPosition({ width_30per + width_2_5per * 2, height_47per });
		wrapper_log->setLayout(new GridLayout(Orientation::Horizontal, 1));// 1 columns

		// 로그뷰 자동 업데이트를 구현하기 위해 라벨 사전에 생성
		for (int i=0; i<LOG_LABEL_MAX; i++) {
			ADD_LABEL("　");
			labelList.push_back(labelLog);
			labelListEmpty++;
		}

		// Version & CopyRights
		Label *label = new Label(wrapper, "Version: 1.0");
		label->setPosition({ 6, height - 22 });

		label = new Label(wrapper, "2019. Team of Dapdap. All rights reserved.");
		label->setPosition({ width - 238, height - 22 });

		screen->performLayout();
		screen->setVisible(true);

//		ShowWindow(::GetConsoleWindow(), SW_HIDE);


		////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////

		// 코드 리펙토링 시, 다른 함수로 뺄 것
		// popup
		Screen *screen2 = new Screen({ width_popup, height_popup }, "A Production Crew", false);
		Window *window2 = new Window(screen2, "");
		window2->setPosition({ 0, 0 });
		window2->setFixedSize({ width_popup, height_popup });

		//makers Image
		GLTexture texture2("");
		auto data2 = texture2.load(imageDirectory + "makers.png");
		using imagesDataType = vector<pair<GLTexture, GLTexture::handleType>>;
		imagesDataType mImagesData2;
		mImagesData2.emplace_back(std::move(texture2), std::move(data2));

		auto imageView2 = new ImageView(window2, mImagesData2[0].first.texture());
		imageView2->setPosition({ 0, 0 });
		imageView2->setFixedSize({ width_popup, height_popup });
		imageView2->setFixedScale(true);
		
		auto wrapper2 = new Widget(window2);
		wrapper2->setFixedSize({ width_popup, height_popup });

		screen2->performLayout();
		screen2->setVisible(true);
		
		nanogui::mainloop();
	}

	nanogui::shutdown();

	CloseHandle(hClientPipe);
	CloseHandle(hSCMgr);
	return 0;
}

bool button_call1(list<Button*> btnList, int before, int after)
{
	try {
		REQUEST_HEADER req;
		bool state = after; // For the meaning of the Toggle button
		list<Button*>::iterator it = btnList.begin();
		advance(it, before);

		hSCMgr = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		switch (before) {
		// TODO. SUBMIT BUTTON FUNCTION
		case SUBMIT:
			println("[INFO] ProcessName : " + processName);
			// 동작 코드 없음
			break;

		// TODO. DRIVER_LOAD BUTTON FUNCTION
		case DRIVER_LOAD:
			println("[INFO] Click the Driver_Load Image button");
			if (!DriverLoader(hSCMgr, (LPSTR)"MitiBypass", (LPSTR)"MitiKiller.sys"))
				ERROR_MACRO("DriverLoader");
			if (!DriverLoader(hSCMgr, (LPSTR)"VulnDriver", (LPSTR)"Vuln.sys"))
				ERROR_MACRO("DriverLoader");
			// Call create dispatch
			CreateFileA("\\\\.\\MitiLinker", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			break;

		// TODO. DLL_INJECTION BUTTON FUNCTION
		case DLL_INJECTION:
			DllLoader((LPSTR)"CheatServer.dll", (LPSTR)"csrss.exe");
			println("[INFO] Click the DLL_Injection Image button");
			break;

		// TODO. CONNECTION BUTTON FUNCTION
		case CONNECTION:
			if (!(hClientPipe = CreateFile(RECV_PIPE_NAME, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL)))
			{
				// 에러 발생시 catch 문으로 보낼 것
				ERROR_MACRO("CreateFile");
			}
			req.type = CHEAT_LIST::SetTargetHandle;
			req.address = 0;
			memset(req.buffer, 0, 256);
			memcpy(req.buffer, processName.c_str(), processName.size());

			if (!WriteFile(hClientPipe, &req, sizeof(REQUEST_HEADER), NULL, NULL))
			{
				// 에러 발생시 catch 문으로 보낼 것
				ERROR_MACRO("WriteFile");
			}

			(*it)->setEnabled(false);
			for (it = btnList.begin(), advance(it, after); it != btnList.end(); ++it)
				(*it)->setEnabled(true);
			println("[INFO] Click the Connection Image button");
			return true;

		// TODO. TOGGLEO1 BUTTON FUNCTION
		case TOGGLEO1:
			if (state)	// ON
			{
				if (MakeSendPacket(&req, CHEAT_LIST::ScriptInfiniteOn, 0, NULL))
				{
					if (!WriteFile(hClientPipe, &req, sizeof(REQUEST_HEADER), NULL, NULL))
						ERROR_MACRO("WriteFile");
				}
				println("[INFO] Script1(Infinite) state: True");
			}
			else		// OFF
			{
				if (MakeSendPacket(&req, CHEAT_LIST::ScriptInfiniteOff, 0, NULL))
				{
					if (!WriteFile(hClientPipe, &req, sizeof(REQUEST_HEADER), NULL, NULL))
						ERROR_MACRO("WriteFile");
				}
				println("[INFO] Script1(Infinite) state: False");
			}
			return true;

		// TODO. TOGGLEO1 BUTTON FUNCTION
		case TOGGLEO2:
			if (state)	// ON
			{
				if (MakeSendPacket(&req, CHEAT_LIST::ScriptMobVacOn, 0, NULL))
				{
					if (!WriteFile(hClientPipe, &req, sizeof(REQUEST_HEADER), NULL, NULL))
						ERROR_MACRO("WriteFile");
				}
				println("[INFO] Script2(MobVac) state: True");
			}
			else		// OFF
			{
				if (MakeSendPacket(&req, CHEAT_LIST::ScriptMobVacOff, 0, NULL))
				{
					if (!WriteFile(hClientPipe, &req, sizeof(REQUEST_HEADER), NULL, NULL))
						ERROR_MACRO("WriteFile");
				}
				println("[INFO] Script2(MobVac) state: False");
			}
			return true;

		// TODO. TOGGLEO1 BUTTON FUNCTION
		case TOGGLEO3:
			if (state)	// ON
			{
				if (MakeSendPacket(&req, CHEAT_LIST::ScriptAbilityOn, 0, NULL))
				{
					if (!WriteFile(hClientPipe, &req, sizeof(REQUEST_HEADER), NULL, NULL))
						ERROR_MACRO("WriteFile");
				}
				println("[INFO] Script3(Ability) state: True");
			}
			else		// OFF
			{
				if (MakeSendPacket(&req, CHEAT_LIST::ScriptAbilityOff, 0, NULL))
				{
					if (!WriteFile(hClientPipe, &req, sizeof(REQUEST_HEADER), NULL, NULL))
						ERROR_MACRO("WriteFile");
				}
				println("[INFO] Script3(Ability) state: False");
			}
			return true;

		default:
			ERROR_MESSAGE = "[ERR ] There is no information about the image button";
			throw ERROR_MESSAGE;
		}

		(*it)->setEnabled(false);
		it = btnList.begin();
		advance(it, after);
		(*it)->setEnabled(true);
	}
	catch (...) {
		println(ERROR_MESSAGE);
		return false;
	}

	return true;
}

bool button_check(list<Button*> btnList, int before, int after)
{
	list<Button*>::iterator it = btnList.begin();
	advance(it, before);
	if ((*it)->enabled() == false) {
		return false;
	}
	return true;
}

void println(std::string msg) {
#if LOGING_CONSOLE
	std::cout << msg << std::endl;
#else
	if (labelListEmpty) { (labelList[LOG_LABEL_MAX - labelListEmpty--])->setCaption(msg); }
	else { ADD_LABEL(msg); }
#endif
}

void println(std::string msg, NTSTATUS hexcode) {
	stringstream ss;
	ss << hex << hexcode;
	msg += ss.str();

#if LOGING_CONSOLE
	std::cout << msg << std::endl;
#else
	if (labelListEmpty) { (labelList[LOG_LABEL_MAX - labelListEmpty--])->setCaption(msg); }
	else { ADD_LABEL(msg); }
#endif
}

void println(std::string msg /* Message */, DWORD errcode) {
	CString str;
	str.Format("%d", errcode);
	msg += str;

#if LOGING_CONSOLE
	std::cout << msg << std::endl;
#else
	if(labelListEmpty)	{ (labelList[LOG_LABEL_MAX - labelListEmpty--])->setCaption(msg); }
	else				{ ADD_LABEL(msg); }
#endif
}
