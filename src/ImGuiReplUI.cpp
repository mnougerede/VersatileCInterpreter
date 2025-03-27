// ImGuiReplUI.cpp
#include "ImGuiReplUI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

ImGuiReplUI::ImGuiReplUI() {
    replOutput = "";
    replInput[0] = '\0';
}

ImGuiReplUI::~ImGuiReplUI() {
    // Cleanup will be handled in run() if needed.
}

bool ImGuiReplUI::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    return true;
}

void ImGuiReplUI::cleanup() {
    glfwTerminate();
}

void ImGuiReplUI::run() {
    if (!init()) {
        return;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "REPL Terminal", nullptr, nullptr);
    if (!window) {
        const char* errorDesc = "";
        glfwGetError(&errorDesc);
        std::cerr << "Failed to create GLFW window: " << errorDesc << "\n";
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    // Main loop
    static bool firstFrame = true;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Force the REPL window to get focus on the first frame.
        if (firstFrame)
            ImGui::SetNextWindowFocus();

        // Build REPL Terminal window
        ImGui::Begin("REPL Terminal");
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -50), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(replOutput.c_str());
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        bool inputSubmitted = ImGui::InputText("Input", replInput, IM_ARRAYSIZE(replInput), ImGuiInputTextFlags_EnterReturnsTrue);

        // On the very first frame, immediately set keyboard focus on the input widget.

        if (firstFrame) {
            ImGui::SetKeyboardFocusHere(-1); // '0' means focus the widget we just created. adjusted to -1 as that works
            firstFrame = false;
        }

        if (inputSubmitted) {
            std::string inputStr(replInput);
            if (inputStr == "exit" || inputStr == "quit") {
                glfwSetWindowShouldClose(window, true);
            } else {
                std::string result = repl.evaluateCommand(inputStr);
                replOutput += "> " + inputStr + "\n" + result + "\n";
            }
            replInput[0] = '\0';
            // Optionally, refocus the input for subsequent frames:
            ImGui::SetKeyboardFocusHere(-1);
        }
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    cleanup();
}
