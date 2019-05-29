#pragma once
#include <nanogui/nanogui.h>
#include <iostream>
#include <list>
#include "gui.h"

using namespace nanogui;
using namespace std;

string ERROR_MESSAGE(256, '\0');



int main(int /* argc */, char ** /* argv */) {
	nanogui::init();
	{
		// create a fixed size screen with one window
		Screen *screen = new Screen({ width, height }, "Client", false);
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

		// submit button
//		int flag = 0;
//		int order_flag[] = { 1,2,0,3,4,5,6 };
		list<Button*> buttonList;
//		static list<Button*>::iterator iter;
//		auto it = buttonList.begin();
		string procName = "";
		Button *button;
		ADD_BUTTON("submit", 28, width_10per, height_10per, width_50per + width_20per + 10, 25, false)
		button->setCallback([&buttonList, textBox, &procName] { procName = textBox->value().c_str(); button_call1(buttonList, (int)SUBMIT, (int)CONNECTION); cout << "[INFO] ProcessName: " << procName.c_str() << endl; }); //  수정 필요


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		NVGcontext *temp = screen->nvgContext();
		ImagePanel *imgPanel;
		vector<pair<int, string>> icons;

		// Driver Load button
		ADD_BUTTON("          Driver Load", 33, width_30per, height_20per, width_2_5per, height_20per, true)
		button->setTooltip("Loading Vulnerable Drivers");
//		button->setCallback([&order_flag, &flag, &buttonList] { auto it = buttonList.begin(); advance(it, (int)DRIVER_LOAD); (*it)->setEnabled(false); cout << "Driver Load!" << endl; it = buttonList.begin(); advance(it, (int)DLL_INJECTION); (*it)->setEnabled(true); });
		button->setCallback([&buttonList] { button_call1(buttonList, (int)DRIVER_LOAD, (int)DLL_INJECTION); });
//		button->setCallback([submit, &flag] {if (flag == 0) submit->setEnabled(true); cout << "Driver Load!" << endl; });
		
		ADD_IMAGE(imageDirectory + "0/", width_2_5per, height_20per + (int)((height_20per - 81) / 2), true)
		imgPanel->setCallback([&buttonList](int i) { img_button_call(buttonList, (int)DRIVER_LOAD, (int)DLL_INJECTION); });
/*		imgPanel->mouseEnterEvent({ 1,2 }, true);
		imgPanel->setEnabled(false);
		imgPanel->setEnabled(true);
*/


		// DLL Injection buttion
		ADD_BUTTON("           DLL Injection", 32, width_30per, height_20per, width_30per + width_2_5per * 2, height_20per, false)
		button->setTooltip("User DLL injection into csrs.exe process");
//		button->setCallback([] { cout << "DLL Injection!" << endl; });
		button->setCallback([&buttonList] { button_call1(buttonList, (int)DLL_INJECTION, (int)SUBMIT); });

		ADD_IMAGE(imageDirectory + "1/", width_2_5per * 2 + width_30per, height_20per + (int)((height_20per - 81) / 2), false)
//		imgPanel->setCallback([](int i) { cout << "DLL Injection icon" << endl ; });
		imgPanel->setCallback([&buttonList](int i) { img_button_call(buttonList, (int)DLL_INJECTION, (int)SUBMIT); });


		// Connection button
		ADD_BUTTON("          Connection", 34, width_30per, height_20per, width_30per * 2 + width_2_5per * 3, height_20per, false)
		button->setTooltip("This button has a fairly long tooltip. It is so long, in ");
//		button->setCallback([] { cout << "Connection!" << endl; });
		button->setCallback([&buttonList] { button_call1(buttonList, (int)CONNECTION, (int)SCENARIO1); });

		ADD_IMAGE(imageDirectory + "2/", width_2_5per * 3 + width_30per * 2, height_20per + (int)((height_20per - 81) / 2), false)
//		imgPanel->setCallback([](int i) { cout << "Connection icon" << endl; });
imgPanel->setCallback([&buttonList](int i) { img_button_call(buttonList, (int)CONNECTION, (int)SCENARIO1); });

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// toggle button
		ADD_BUTTON("Scenario1", 30, width_30per, height_15per, width_2_5per, height_47per, false)
		button->setFlags(Button::ToggleButton);
		button->setBackgroundColor(Color(0, 0, 255, 25));
		button->setChangeCallback([](bool state) { cout << "Scenario1 state: " << state << endl; });

		ADD_BUTTON("Scenario2", 30, width_30per, height_15per, width_2_5per, height_47per + height_15per + height_1_25per, false)
		button->setFlags(Button::ToggleButton);
		button->setBackgroundColor(Color(0, 0, 255, 25));
		button->setChangeCallback([](bool state) { cout << "Scenario2 state: " << state << endl; });

		ADD_BUTTON("Scenario3", 30, width_30per, height_15per, width_2_5per, height_47per + height_15per *2 + height_1_25per * 2, false)
		button->setFlags(Button::ToggleButton);
		button->setBackgroundColor(Color(0, 0, 255, 25));
		button->setChangeCallback([](bool state) { cout << "Scenario3 state: " << state << endl; });

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Label *label = new Label(wrapper, "Version: 1.0");
		label->setPosition({ 6, height - 17 });
		label->setPosition({ 6, height - 22 });

		Label *labels = new Label(wrapper, "Dapdap Team.");
		labels->setPosition({ width - 88, height - 17 });
		labels->setPosition({ width - 88, height - 22 });
//		Vector2i *value;
//		*value = labels->position;
//		labels->mouseButtonEvent(*value, 0, true, 0);

		// Log View // 구상 실패~ > ~
		TextBox *logview = new TextBox(wrapper);
		logview->setEditable(true);
		logview->setFontSize(20);
		logview->setValue("Log View");
		logview->setFixedSize({ width - (width_30per + width_2_5per * 3), height_47per });
		logview->setPosition({ width_30per + width_2_5per * 2, height_47per });
		logview->setTooltip("tq");


		auto vscroll_log = new VScrollPanel(window);
		vscroll_log->setFixedSize({ width - (width_30per + width_2_5per * 3), height_47per });
		vscroll_log->setPosition({ width_30per + width_2_5per * 2, height_47per });

		auto wrapper_log = new Widget(vscroll_log);
		wrapper_log->setFixedSize({ width - (width_30per + width_2_5per * 3), height_47per });
		wrapper_log->setPosition({ width_30per + width_2_5per * 2, height_47per });

		Label *label_log = new Label(wrapper_log,"");
		label_log->setPosition({ 10, 8 });
		label_log->setFontSize(20);
		label_log->setCaption("dddd");
//		label_log->setVisible(false);


//		int i=0;
//		for (iter = buttonList.begin(); iter != buttonList.end(); ++iter) {			i++;		}
//		cout << "개수 : " << i << endl;

		screen->performLayout();
		screen->setVisible(true);
//		ShowWindow(::GetConsoleWindow(), SW_HIDE);
//		cout << vscroll->getRefCount() << endl;
		nanogui::mainloop();
	}

	nanogui::shutdown();
	return 0;
}

bool button_call1(list<Button*> btnList, int before, int after)
{
	try {
		list<Button*>::iterator it = btnList.begin();
		advance(it, before);

		switch (before) {
		case SUBMIT:
//			if (submit_button() == false) throw;
			break;
		case DRIVER_LOAD:
			cout << "[INFO] Click the Driver_Load Image button." << endl;
			break;
		case DLL_INJECTION:
			cout << "[INFO] Click the DLL_Injection Image button." << endl;
			break;
		case CONNECTION:
			cout << "[INFO] Click the Connection Image button." << endl;
			(*it)->setEnabled(false);
			it = btnList.begin();
			advance(it, after);
			(*it)->setEnabled(true);
			advance(it, 1);
			(*it)->setEnabled(true);
			advance(it, 1);
			(*it)->setEnabled(true);
			return true;
		default:
			ERROR_MESSAGE = "[ERR ] There is no information about the image button.";
			throw ERROR_MESSAGE;
		}

		(*it)->setEnabled(false);
		it = btnList.begin();
		advance(it, after);
		(*it)->setEnabled(true);
	}
	catch (...) {
		cout << ERROR_MESSAGE << endl;
		return false;
	}

	return true;
}


bool img_button_call(list<Button*> btnList, int before, int after)
{
	try {
		list<Button*>::iterator it = btnList.begin();
		advance(it, before);
		if ((*it)->enabled() == false) {
			//if (before == DRIVER_LOAD)			ERROR_MESSAGE = "[ERR ] Image button Driver Load cannot be pressed.";
			//else if (before == DLL_INJECTION)	ERROR_MESSAGE = "[ERR ] Image button DLL Injection cannot be pressed.";
			//else if (before == CONNECTION)		ERROR_MESSAGE = "[ERR ] Image button Connection cannot be pressed.";
			//else								ERROR_MESSAGE = "[ERR ] There is no information about the image button.";
			//throw ERROR_MESSAGE;
			return false;
		}

		switch (before) {
		case DRIVER_LOAD:
			cout << "[INFO] Click the Driver_Load Image button." << endl;
			break;
		case DLL_INJECTION:
			cout << "[INFO] Click the DLL_Injection Image button." << endl;
			break;
		case CONNECTION:
			cout << "[INFO] Click the Connection Image button." << endl;
			(*it)->setEnabled(false);
			it = btnList.begin();
			advance(it, after);
			(*it)->setEnabled(true);
			advance(it, 1);
			(*it)->setEnabled(true);
			advance(it, 1);
			(*it)->setEnabled(true);
			return true;
		default:
			ERROR_MESSAGE = "[ERR ] There is no information about the image button.";
			throw ERROR_MESSAGE;
		}

		(*it)->setEnabled(false);
		it = btnList.begin();
		advance(it, after);
		(*it)->setEnabled(true);
	}
	catch (...) {
		cout << ERROR_MESSAGE << endl;
		return false;
	}

	return true;
}

bool submit_button()
{
	try {

	}
	catch (...) {
		return false;
	}
	return true;
}