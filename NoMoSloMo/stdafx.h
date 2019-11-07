#pragma once

#define MAX_STRING 0xFFFF

#include <windows.h>

#include <cstdio>
#include <cstdint>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <memory>

#include "resource.h"

#include "Utility.h"
#include "Logger.h"
#include "config.h"
#include "patch.h"
#include "pattern.h"
#include "opcode.h"

extern HMODULE ourModule;

void initialize();

void unload();
