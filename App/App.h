#ifndef APP_APP_H
#define APP_APP_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void Initialize();

void RenderUi();

void Ui_Menu();

void Ui_TextReplace();

void Ui_AVIF2JPEG();

#endif
