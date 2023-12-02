#include "App.h"
#include <wchar.h>
#include <cstdlib>
#include <cstring>
#include "imgui.h"
#include "imgui_internal.h"

#include <files.h>
#include <iostream>
#include <vector>
#include "image_editor.h"  // avif -> jpeg

namespace {
static bool show_tip = false;
static char tip_message[1024 * 16];

static bool check_replace = true;
static bool check_img_editor = false;
static char working_path[1024 * 16];
}  // namespace

void Initialize() {
  base::files::GetWorkingDirectory(working_path, sizeof(working_path));
}

void RenderUi() {
  const ImGuiViewport* view_port = ImGui::GetMainViewport();
  ImVec2 window_size = view_port->WorkSize;

  // 功能菜单
  ImGui::Begin("XTools Menu", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
  ImGui::SetWindowPos(ImVec2(0, 0));
  ImGui::SetWindowSize(ImVec2(120, window_size.y));
  Ui_Menu();
  ImGui::End();

  // 功能区
  ImGui::Begin("XTools Function", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
  ImGui::SetWindowPos(ImVec2(120, 0));
  ImGui::SetWindowSize(ImVec2(window_size.x - 120, window_size.y));

  Ui_TextReplace();
  Ui_AVIF2JPEG();

  ImGui::End();
}

void Ui_Menu() {
  if (ImGui::Checkbox("文本替换", &check_replace)) {
    check_img_editor = !check_replace;
  }

  if (ImGui::Checkbox("图片转换", &check_img_editor)) {
    check_replace = !check_img_editor;
  }
}

void Ui_TextReplace() {
  if (!check_replace) {
    return;
  }
  static const char* replace_items[] = {",", ".", "，", "。"};
  static const wchar_t w_replace_items[] = {L',', L'.', L'，', L'。'};

  static int replace_checked_index = 0;
  static char input[1024 * 16] = "";
  static char output[1024 * 16] = "";
  static wchar_t* w_source = (wchar_t*)malloc(1024 * 16 * sizeof(wchar_t));

  ImGui::InputTextEx("##input source", "输入文本", input, IM_ARRAYSIZE(input),
                     ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 36),
                     ImGuiInputTextFlags_Multiline);

  ImGui::Text(u8"符号");
  ImGui::SameLine();
  ImGui::PushItemWidth(80);
  ImGui::Combo("##replace_symbol_combo", &replace_checked_index, replace_items,
               IM_ARRAYSIZE(replace_items));

  ImGui::SameLine();
  if (ImGui::Button("替换")) {
    int len = (int)std::mbstowcs(w_source, input, sizeof(input));
    for (int i = 0; i < len; i++) {
      if (w_source[i] == '\n') {
        w_source[i] = w_replace_items[replace_checked_index];
      }
    }
    std::wcstombs(output, w_source, sizeof(output));
  }

  ImGui::PopItemWidth();

  struct TextFilters {
    static int FilterDisableInput(ImGuiInputTextCallbackData* data) {
      return 1;
    }
  };
  ImGui::InputTextEx(
      "##output source", "点击替换", output, IM_ARRAYSIZE(output),
      ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 3.5),
      ImGuiInputTextFlags_Multiline | ImGuiInputTextFlags_CallbackCharFilter,
      TextFilters::FilterDisableInput);
}

void Ui_AVIF2JPEG() {
  if (!check_img_editor) {
    return;
  }

  bool exists = base::files::Exists(working_path);
  bool is_directory = base::files::IsDirectory(working_path);

  ImGui::LabelText("##directory_label", "%s", u8"路径/目录");
  ImGui::InputTextEx("##directory", "输入路径/目录", working_path,
                     IM_ARRAYSIZE(working_path),
                     ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 2),
                     ImGuiInputTextFlags_Multiline);

  if (!exists) {
    ImGui::LabelText("##invalid_path", "无效路径：%s", working_path);
    return;
  }

  if (!is_directory) {
    if (ImGui::Button("文件转换", ImVec2(160, 30))) {
      show_tip = true;
      avif2jpeg(working_path);

      const char* msg = u8"转完了，去原目录查看";
      memcpy(tip_message, msg, strlen(msg));
    }
  } else {
    if (ImGui::Button("目录转换", ImVec2(160, 30))) {
      std::vector<std::string> paths;
      base::files::GetAllFilePaths(working_path, paths);

      show_tip = true;
      bool has_avif = false;
      const char* suffix = ".avif";
      int suffix_len = strlen(suffix);
      for (std::string path : paths) {
        if (path.compare(path.length() - suffix_len, suffix_len, suffix) == 0) {
          avif2jpeg(path.c_str());
          has_avif = true;
        }
      }

      if (has_avif) {
        const char* msg = u8"转完了，去原目录查看";
        memcpy(tip_message, msg, strlen(msg));
      } else {
        const char* msg = u8"目录下没有avif图片";
        memcpy(tip_message, msg, strlen(msg));
      }
    }
  }

  if (show_tip) {
    ImGui::Begin("提示", &show_tip,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove);
    const ImGuiViewport* view_port = ImGui::GetMainViewport();
    ImVec2 window_size = view_port->WorkSize;
    ImGui::SetWindowPos(
        ImVec2((window_size.x - 280) / 2, (window_size.y - 160) / 2));
    ImGui::SetWindowSize(ImVec2(280, 80));
    ImGui::LabelText("##tip_message", "%s", tip_message);
    ImGui::End();
  }
}
