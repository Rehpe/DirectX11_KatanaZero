#pragma once

#include "framework.h"

//Engine Library 불러오기
#include <Engine\global.h>
#include <Engine\CEngine.h>

#ifdef _DEBUG  
#pragma comment(lib, "Engine//Engine_d.lib")
#else
#pragma comment(lib, "Engine//Engine.lib")
#endif

// Script Library
#ifdef _DEBUG
#pragma comment(lib, "Script//Script_d")
#else
#pragma comment(lib, "Script//Script")
#endif
