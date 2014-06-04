#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#define VC_EXTRALEAN
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _AFX_ALL_WARNINGS

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <SDKDDKVer.h>
#include "Resource.h"

#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include <afxdisp.h>
#include <afxhtml.h>
#include <atlimage.h>

#include <fstream>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <utility>
using namespace std;

#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib, "GdiPlus.lib")


#ifdef _DEBUG
	#define DbgMsg(x, ...) wprintf_s(x, __VA_ARGS__);
#else
	#define DbgMsg(x, ...)
#endif

#include "Helpers.h"
#include "Res.h"
#include "Ini.h"
#include "Settings.h"
#include "Language.h"
#include "Browser.h"
#include "Label.h"
#include "ImgButton.h"
#include "Skin.h"
#include "Internet.h"
#include "PatcherForm.h"
#include "Patcher.h"
#include "Thread.h"
#include "PatchThread.h"

