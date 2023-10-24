#pragma once

#ifdef DYNAMICLIB_EXPORTS
#define DLL extern "C" __declspec(dllexport)
#else
#define DLL extern "C" __declspec(dllimport)
#endif


DLL int Mul(int a, int b);