#pragma once

enum ACTION
{
	NOACTION = 0,
	CL_RUN,
	APP_RUN,
	URL_OPEN,
	KRO_UPDATE,
	KRORE_UPDATE,
	AS_SHOW,
	KP_MINIMIZE,
	KP_CLOSE
};

enum BUTTONS
{
	NULLID = 0,
	RUN = 1,
	EXIT,
	KRO,
	KRORE,
	LAST,
	ENDID = 13
};

enum CONTROL
{
	BEGIN = 0,
	NOTICE,
	STATUS,
	INFO,
	CPROGRESS,
	TPROGRESS
};

struct BROWSER
{
	CBrowser *Browser;
	POINT Pos;
	SIZE Size;
};
typedef unordered_map<INT, BROWSER> _Browsers;

struct LABEL
{
	CLabel		*Label;
	POINT		Pos;
	SIZE		Size;
	COLORREF	FontColor;
	INT			FontSize;
	BOOL		Bold;
};
typedef unordered_map<INT, LABEL> _Labels;

struct PROGRESSBAR
{
	CProgressCtrl	*ProgressBar;
	POINT			Pos;
	SIZE			Size;
};
typedef unordered_map<int, PROGRESSBAR> _PBars;

struct BUTTON
{
	CImgButton *Button;
	POINT		Pos;
	CString		Active,
				Hovered,
				Pressed;
	ACTION		Action;
	CString		FirstParam,
				SecondParam;
};
typedef unordered_map<INT, BUTTON> _Buttons;

class CSkin
{
public:
	CSkin(void);
	~CSkin(void);

	void SaveSkin(CString &SkinFolder, CString ResName = _T("SKIN"));
	void LoadSkin(CString SkinFolder = _T("./Patcher.skin/"));

public:
	CString SkinFile;
	_Browsers Browsers;
	_Labels Labels;
	_PBars PBars;
	_Buttons Buttons;
	INT CustomButtons;
};

