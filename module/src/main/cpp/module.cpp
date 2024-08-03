#include "module.h"
#include <string>
#include <fstream>
#include "data.h"
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/system_properties.h>
#include <dlfcn.h>
#include <jni.h>
#include <sys/mman.h>
#include <linux/unistd.h>
#include <array>
#include <string>
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <cstdint>
#include "xdl.h"
// #include "include/dobby.h"
#include "imconfig.h"
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "hook.h"

using namespace std;
using namespace ImGui;

int  width, height;
bool setGui = false;

hookInput(void, Input, void *thiz, void *event_a, void *event_b) {
    origInput(thiz, event_a, event_b);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
}

void guiSetup() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    StyleColorsLight();
    ImFontConfig fontCfg;
    fontCfg.SizePixels = 22.0f;
    io.Fonts->AddFontDefault(&fontCfg);
    GetStyle().ScaleAllSizes(3.0f);

}

EGLBoolean (*origEglSB)(EGLDisplay display, EGLSurface surface);
EGLBoolean hookEglSB(EGLDisplay display, EGLSurface surface) {
    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);
    if (!setGui) {
        guiSetup();
        setGui = true;
    }
    ImGuiIO &io = GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    NewFrame();
    ShowDemoWindow();
    EndFrame();
    Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
    return origEglSB(display, surface);
}

/*
typedef dobby_dummy_func_t func_t;
int hook(void *target, func_t replace, func_t *backup) {
	return DobbyHook(target, replace, *backup);
}
*/
	
void moduleMain(const char *appDataDir, void *data, size_t length) {
	sleep(5);
    void *symInput = DobbySymbolResolver("/system/lib/libinput.so", "_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE");
    if (NULL != symInput) {
        // DobbyHook((void *)symInput, (void *)myInput, (void **)&origInput);
		DobbyHook((void *)symInput, (dobby_dummy_func_t)myInput, (dobby_dummy_func_t*)origInput);
    }
    
    void *libEgl = xdl_open("libEGL.so", 0);
    void *symEgl = xdl_sym(libEgl, "eglSwapBuffers", nullptr);
	if (NULL != symEgl) {
		customHook::hook((void *)symEgl, (func_t)hookEglSB, (func_t*)&origEglSB);
	}
    xdl_close(libEgl);
}
