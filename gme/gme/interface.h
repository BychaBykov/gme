#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
//#include "imgui/imgui_stdlib.cpp"
#include "imgui/imgui_internal.h"
//#include "imgui/imgui_stdlib.h"

#pragma once
//global var
void set_funclist();
bool set_funclist_active = true;
bool setfunc = false;
static char FuncFromInput[255];
static int number = 70;

void render() {
    set_funclist();
};

void set_funclist() {
    ImGui::SetNextWindowSize(ImVec2(400, 300));
    ImGui::Begin("Function List", &set_funclist_active, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::SetWindowPos("Function List", ImVec2(0, 0));
    struct Funcs
    {
        static int MyCallback(ImGuiInputTextCallbackData* data)
        {
            if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
            {
                data->InsertChars(data->CursorPos, "..");
            }
            else if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit)
            {
                // Toggle casing of first character
                char c = data->Buf[0];
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) data->Buf[0] ^= 32;
                data->BufDirty = true;

                // Increment a counter
                int* p_int = (int*)data->UserData;
                *p_int = *p_int + 1;
            }
            return 0;
        }
    };
    ImGui::InputText("##", FuncFromInput, IM_ARRAYSIZE(FuncFromInput), ImGuiInputTextFlags_CallbackCompletion, Funcs::MyCallback);
    //Func2arg<float> fun(FuncFromInput);
    ImGui::SliderInt("Number of polygons", &number,30,500);
    if (ImGui::Button("Set Func")) setfunc = true;
    ImGui::End();
}
