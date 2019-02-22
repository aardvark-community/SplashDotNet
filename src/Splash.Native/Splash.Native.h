#pragma once

#ifdef __APPLE__
#include <opengl/gl3.h>
#define DllExport(t) extern "C" t
#elif __GNUC__
#include <GL/gl.h>
#include <GL/glx.h>
#define DllExport(t) extern "C" t
#else
#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#define DllExport(t) extern "C"  __declspec( dllexport ) t __cdecl
#endif


DllExport(DWORD) ShowSplash(int w, int h, int bits, const void* data);
DllExport(void) CloseSplash(DWORD hwnd);
