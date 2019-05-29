#pragma once
#include <nanogui/nanogui.h>
#include <iostream>
#include <list>
#include "gui_class.h"

bool button_call1(std::list<nanogui::Button*> btnList, int before, int after);
bool img_button_call(std::list<nanogui::Button*> btnList, int before, int after);
bool submit_button();

/* image directory path */
// $(ProjectName)/img/
std::string imageDirectory("img/");


/* 자주 사용하는 GUI 객체 초기 설정한 상태로 생성하기 위한 매크로 */
#define ADD_BUTTON(name, font_size, width_size, height_size, width_position, height_position, tf)	\
    button = new Button(wrapper, name);																\
	button->setFixedSize({ width_size, height_size });												\
	button->setFontSize(font_size);																	\
	button->setPosition({ width_position, height_position });										\
	buttonList.push_back(button);																	\
	button->setEnabled(tf);

#define ADD_IMAGE(imgDir, width, height, tf)		\
	icons = loadImageDirectory(temp, imgDir);		\
	imgPanel = new ImagePanel(wrapper);				\
	imgPanel->setImages(icons);						\
	imgPanel->setFixedSize({ 74, 74 });				\
	imgPanel->setPosition({ width, height });


/* GUI 사이즈 정의 */
constexpr int width = 800;
constexpr int width_50per = int(width * 0.5);
constexpr int width_30per = int(width * 0.3);
constexpr int width_20per = int(width * 0.2);
constexpr int width_10per = int(width * 0.1);
constexpr int width_2_5per = int(width * 0.025);
constexpr int height = 450;
constexpr int height_47per = int(height * 0.47);
constexpr int height_20per = int(height * 0.2);
constexpr int height_15per = int(height * 0.15);
constexpr int height_10per = int(height * 0.1);
constexpr int height_1_25per = int(height * 0.0125);


/* Main button flag  */
enum ORDER_FLAG {
	SUBMIT			= 0,
	DRIVER_LOAD		= 1,
	DLL_INJECTION	= 2,
	CONNECTION		= 3,
	SCENARIO1		= 4,
	SCENARIO2		= 5,
	SCENARIO3		= 6
};


//typedef enum ORDER_FLAG {
//	SUBMIT			= (0x01 << 0),		// 1
//	DRIVER_LOAD		= (0x01 << 1),		// 2
//	DLL_INJECTION	= (0x01 << 2),		// 4
//	CONNECTION		= (0x01 << 3),		// 8
//	SCENARIO1		= (0x01 << 4),		// 16
//	SCENARIO2		= (0x01 << 5),		// 32
//	SCENARIO3		= (0x01 << 6)		// 64
//};
