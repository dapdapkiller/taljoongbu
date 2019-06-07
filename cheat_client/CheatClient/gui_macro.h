#pragma once
#include <nanogui/nanogui.h>

static std::string ERROR_MESSAGE(256, '\0');	// try/catch문 에러 메시지 내용
static std::string processName = "";			// 선택한 프로세스 이름
static nanogui::VScrollPanel *vscroll_log;		// 로그뷰어 스크롤
static nanogui::Widget *wrapper_log;			// 로그뷰어 칸
static nanogui::Label* labelLog;				// 로깅 라벨
static std::vector<nanogui::Label*> labelList;	// 로깅 라벨 vector
static int labelListEmpty = 0;					// 비어있는 라벨 리스트 갯 수


constexpr int LOG_LABEL_MAX = 11; // 로그뷰 화면 최대 라벨 수

/* 자주 사용하는 GUI 객체에 대해 초기 설정을 한 상태로 생성하기 위한 매크로 */
#define ADD_BUTTON(name, font_size, width_size, height_size, width_position, height_position, tf)	\
    button = new nanogui::Button(wrapper, name);													\
	button->setFixedSize({ width_size, height_size });												\
	button->setFontSize(font_size);																	\
	button->setPosition({ width_position, height_position });										\
	buttonList.push_back(button);																	\
	button->setEnabled(tf)

#define ADD_IMAGE(imgDir, width, height, tf)		\
	icons = loadImageDirectory(temp, imgDir);		\
	imgPanel = new nanogui::ImagePanel(wrapper);	\
	imgPanel->setImages(icons);						\
	imgPanel->setFixedSize({ 74, 74 });				\
	imgPanel->setPosition({ width, height })

#define ADD_LABEL(log)													\
	labelLog = new nanogui::Label(wrapper_log, "　");					\
	labelLog->setFontSize(20);											\
	labelLog->setFixedWidth(width - (width_30per + width_2_5per * 4));	\
	labelLog->setCaption(log)
