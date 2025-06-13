#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glad.h"

#ifdef _WIN32
    #include <windows.h>
    static HMODULE libGL;
#else
    #include <dlfcn.h>
    static void* libGL;
#endif

#if defined(__GNUC__) || defined(__clang__)
__attribute__((unused))
#endif
static void* get_proc(const char *name) {
    void* result = NULL;
    #ifdef _WIN32
        result = (void*)GetProcAddress(libGL, name);
    #else
        result = dlsym(libGL, name);
    #endif
    return result;
}

int gladLoadGL(void) {
    #ifdef _WIN32
        libGL = LoadLibraryA("opengl32.dll");
    #else
        libGL = dlopen("libGL.so", RTLD_LAZY);
    #endif
    if (libGL == NULL) {
        return 0;
    }
    // Hier folgt der generierte Code für die OpenGL-Funktionszeiger
    // (wird durch den Generator auf glad.dav1d.de erzeugt)
    return 1;
}

int gladLoadGLLoader(GLADloadproc load) {
    if (load == NULL) {
        return 0;
    }
    // Hier folgt der generierte Code für die OpenGL-Funktionszeiger
    // (wird durch den Generator auf glad.dav1d.de erzeugt)
    return 1;
}
