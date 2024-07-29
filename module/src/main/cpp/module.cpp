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
#include "include/dobby.h"
#include "imconfig.h"
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

using namespace std;
int width, height;

hookInput(void, Input, void *thiz, void *event_a, void *event_b) {
    origInput(thiz, event_a, event_b);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
}

using namespace ImGui;
void guiSetup() {
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);
    ImGui_ImplOpenGL3_Init("version 300 es");
    StyleColorsLight();
    ImFontConfig fontCfg;
    fontCfg.SizePixels = 22.0f;
    io.Fonts->AddFontDefault(&fontCfg);
    GetStyle().ScaleAllSizes(3.0f);

}

bool setGui = false;
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


void moduleMain(const char *appDataDir, void *data, size_t length) {
    void *libInput = DobbySymbolResolver("/system/lib/libinput.so", "_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE");
    if (NULL != libInput) {
        DobbyHook((void *)libInput, (void *)myInput, (void **)&origInput);
    }
    
    void *libEgl = xdl_open("libEGL.so", 0);
    void *eglSbSym = xdl_sym(libEgl, "eglSwapBuffers", nullptr);
    xdl_close(libEgl);
    DobbyHook((void *)eglSbSym, (void *)hookEglSB, (void **)*origEglSB);
}
