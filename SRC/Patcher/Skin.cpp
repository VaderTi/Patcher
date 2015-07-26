#include "StdAfx.h"
#include "Patcher.h"
#include "Skin.h"

CSkin::CSkin(void)
{
}


CSkin::~CSkin(void)
{
}

void CSkin::SaveSkin(CString &SkinFolder, CString ResName /* = _T */)
{
	CRes Res;
	if (!Res.SaveResource(_T("SKIN"), _T("skin.res")))
	{
		MessageBox(NULL, _T("Settings not embedded!"), _T("Patcher"), MB_ICONERROR);
		return;
	}
	DeleteFiles(SkinFolder + _T("*.*"));
	Res.ExtractResource(_T("skin.res"), &SkinFolder);
	DeleteFile(_T("skin.res"));
}

void CSkin::LoadSkin(const CString SkinFolder /* = _T */)
{
	CIni Ini;

	auto StyleFile = SkinFolder;
	StyleFile += _T("skin.ini");

	Ini.LoadFile(StyleFile);
	Ini.SetSection(_T("Patcher::Style"));
	SkinFile = SkinFolder + Ini.GetString(_T("Skin"), _T("Skin.png"));

	Ini.SetSection(_T("Browser::Notice"));
	auto Browser = &Browsers[CONTROL::NOTICE];
	Browser->Pos.x = Ini.GetInt(_T("Left"), 15);
	Browser->Pos.y = Ini.GetInt(_T("Top"), 160);
	Browser->Size.cx = Ini.GetInt(_T("Width"), 390);
	Browser->Size.cy = Ini.GetInt(_T("Height"), 229);

	Ini.SetSection(_T("Text::Status"));
	auto Static = &Labels[CONTROL::STATUS];
	Static->Pos.x = Ini.GetInt(_T("Left"), 17);
	Static->Pos.y = Ini.GetInt(_T("Top"), 408);
	Static->Size.cx = Ini.GetInt(_T("Width"), 485);
	Static->Size.cy = Ini.GetInt(_T("Height"), 15);
	Static->FontColor = HexToRGB(Ini.GetString(_T("Color"), _T("FFFF80")));
	Static->FontSize = Ini.GetInt(_T("Size"), 8);
	Static->Bold = Ini.GetInt(_T("Bold"), 1);

	Ini.SetSection(_T("Text::Info"));
	Static = &Labels[CONTROL::INFO];
	Static->Pos.x = Ini.GetInt(_T("Left"), 17);
	Static->Pos.y = Ini.GetInt(_T("Top"), 422);
	Static->Size.cx = Ini.GetInt(_T("Width"), 485);
	Static->Size.cy = Ini.GetInt(_T("Height"), 15);
	Static->FontColor = HexToRGB(Ini.GetString(_T("Color"), _T("FFFF80")));
	Static->FontSize = Ini.GetInt(_T("Size"), 8);
	Static->Bold = Ini.GetInt(_T("Bold"), 1);

	Ini.SetSection(_T("ProgressBar::Current"));
	auto Progress = &PBars[CONTROL::CPROGRESS];
	Progress->Pos.x = Ini.GetInt(_T("Left"), 15);
	Progress->Pos.y = Ini.GetInt(_T("Top"), 445);
	Progress->Size.cx = Ini.GetInt(_T("Width"), 490);
	Progress->Size.cy = Ini.GetInt(_T("Height"), 20);

	Ini.SetSection(_T("ProgressBar::Total"));
	Progress = &PBars[CONTROL::TPROGRESS];
	Progress->Pos.x = Ini.GetInt(_T("Left"), 15);
	Progress->Pos.y = Ini.GetInt(_T("Top"), 470);
	Progress->Size.cx = Ini.GetInt(_T("Width"), 490);
	Progress->Size.cy = Ini.GetInt(_T("Height"), 20);

	Ini.SetSection(_T("Button::Start"));
	auto Button = &Buttons[BUTTONS::RUN];
	Button->Button = nullptr;
	Button->Pos.x = Ini.GetInt(_T("Left"), 16);
	Button->Pos.y = Ini.GetInt(_T("Top"), 106);
	Button->Active = SkinFolder + Ini.GetString(_T("Active"), _T("start_active.png"));
	Button->Hovered = SkinFolder + Ini.GetString(_T("Hovered"), _T("start_hover.png"));
	Button->Pressed = SkinFolder + Ini.GetString(_T("Pressed"), _T("start_press.png"));
	Button->Action = ACTION::CL_RUN;
	Button->FirstParam = Patcher.m_Settings.m_rSettings.EXENAME;
	Button->SecondParam = Patcher.m_Settings.m_rSettings.EXEPARAM;

	Ini.SetSection(_T("Button::Exit"));
	Button = &Buttons[BUTTONS::EXIT];
	Button->Button = nullptr;
	Button->Pos.x = Ini.GetInt(_T("Left"), 424);
	Button->Pos.y = Ini.GetInt(_T("Top"), 349);
	Button->Active = SkinFolder + Ini.GetString(_T("Active"), _T("exit_active.png"));
	Button->Hovered = SkinFolder + Ini.GetString(_T("Hovered"), _T("exit_hover.png"));
	Button->Pressed = SkinFolder + Ini.GetString(_T("Pressed"), _T("exit_press.png"));
	Button->Action = ACTION::KP_CLOSE;

	Ini.SetSection(_T("Button::KRO Patches"));
	Button = &Buttons[BUTTONS::KRO];
	Button->Button = nullptr;
	Button->Pos.x = Ini.GetInt(_T("Left"), 425);
	Button->Pos.y = Ini.GetInt(_T("Top"), 160);
	Button->Active = SkinFolder + Ini.GetString(_T("Active"), _T("kro_u.png"));
	Button->Hovered = SkinFolder + Ini.GetString(_T("Hovered"), _T("kro_f.png"));
	Button->Pressed = SkinFolder + Ini.GetString(_T("Pressed"), _T("kro_d.png"));
	Button->Action = ACTION::KRO_UPDATE;

	//Ini.SetSection(_T("Button::KRORE Patches"));
	//Button = &Buttons[KRORE];
	//Button->Button = NULL;
	//Button->Pos.x = Ini.GetInt(_T("Left"), 425);
	//Button->Pos.y = Ini.GetInt(_T("Top"), 195);
	//Button->Active = SkinFolder + Ini.GetString(_T("Active"), _T("krore_u.png"));
	//Button->Hovered = SkinFolder + Ini.GetString(_T("Hovered"), _T("krore_f.png"));
	//Button->Pressed = SkinFolder + Ini.GetString(_T("Pressed"), _T("krore_d.png"));
	//Button->Action = KRORE_UPDATE;

	Ini.SetSection(_T("Patcher::Customs"));
	CustomButtons = Ini.GetInt(_T("CBC"), 0);
	if (CustomButtons > 10) CustomButtons = 10;
	for (auto i = 0; i < CustomButtons; i++)
	{
		CString BtnSection;
		BtnSection.Format(_T("Button::Custom %d"), (i+1));

		ACTION Action;
		auto BtnID = static_cast<BUTTONS>(BUTTONS::LAST + i);

		Ini.SetSection(BtnSection);
		Button = &Buttons[BtnID];
		Button->Button = nullptr;
		Action = static_cast<ACTION>(Ini.GetInt(_T("Action"), 0));
		if (Action > ACTION::URL_OPEN) Action = ACTION::NOACTION;
		Button->Action = Action;
		Button->Pos.x = Ini.GetInt(_T("Left"), 0);
		Button->Pos.y = Ini.GetInt(_T("Top"), 0);
		Button->Active = SkinFolder + Ini.GetString(_T("Active"), _T(""));
		Button->Hovered = SkinFolder + Ini.GetString(_T("Hovered"), _T(""));
		Button->Pressed = SkinFolder + Ini.GetString(_T("Pressed"), _T(""));
		Button->FirstParam = Ini.GetString(_T("FirstParam"), _T(""));
		Button->SecondParam = Ini.GetString(_T("SecondParam"), _T(""));
	}
}