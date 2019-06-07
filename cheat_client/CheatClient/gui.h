#pragma once
#include <nanogui/nanogui.h>
#include <iostream>
#include <list>
#include "gui_class.h"
#include "gui_macro.h"
#include "print.h"

bool button_call1(std::list<nanogui::Button*> btnList, int before, int after);
bool button_check(std::list<nanogui::Button*> btnList, int before, int after);


/* image directory path */
// $(ProjectName)/img/
static std::string imageDirectory("img/");


/* GUI 사이즈 정의 */
constexpr int width = 800;
constexpr int width_popup = 662;
constexpr int width_50per = int(width * 0.5);
constexpr int width_30per = int(width * 0.3);
constexpr int width_20per = int(width * 0.2);
constexpr int width_10per = int(width * 0.1);
constexpr int width_2_5per = int(width * 0.025);
constexpr int height = 450;
constexpr int height_popup = 363;
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
	TOGGLEO1		= 4,
	TOGGLEO2		= 5,
	TOGGLEO3		= 6
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
