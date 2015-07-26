#include "StdAfx.h"
#include "Patcher.h"
#include "PatcherForm.h"

IMPLEMENT_DYNAMIC(CPatcherForm, CDialog)

CPatcherForm::CPatcherForm(CWnd* pParent /*=NULL*/)
	: CDialog(CPatcherForm::IDD, pParent)
{
}

CPatcherForm::~CPatcherForm()
{
}

void CPatcherForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPatcherForm, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CONTROL_RANGE(BN_CLICKED, BUTTONS::NULLID, BUTTONS::ENDID, &CPatcherForm::OnButtonDown)
END_MESSAGE_MAP()

BOOL CPatcherForm::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pSkin = nullptr;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINICON);
	SetIcon(m_hIcon, TRUE); SetIcon(m_hIcon, FALSE);

	m_pSkin = new CImage();
	m_pSkin->Load(Patcher.m_Skin.SkinFile);
	if (m_pSkin->IsNull())
	{
		DbgMsg(_T("Skin not found at path %s "), Patcher.m_Skin.SkinFile);
		EndDialog(IDCANCEL);
		return FALSE;
	}

	DrawForm();

	CRect Rect;

	SIZE Size;
	Size.cx = Size.cy = 0;

	DbgMsg(_T("Create interface elements:\t\t"));
	auto Browser = &Patcher.m_Skin.Browsers[CONTROL::NOTICE];
	Rect = CRect(Browser->Pos, Browser->Size);
	Browser->Browser = dynamic_cast<CBrowser*>(RUNTIME_CLASS(CBrowser)->CreateObject());
	Browser->Browser->Create(nullptr, _T(""), 0, Rect, this, CONTROL::NOTICE);
	Browser->Browser->Navigate(Patcher.m_Settings.m_rSettings.NOTICE_URL);
	Browser->Browser->ShowWindow(SW_SHOW);
	Browser->Browser->UpdateWindow();

	auto Static = &Patcher.m_Skin.Labels[CONTROL::STATUS];
	Rect = CRect(Static->Pos, Static->Size);
	Static->Label = new CLabel();
	Static->Label->Create(nullptr, 0, Rect, this, CONTROL::STATUS);
	Static->Label->SetTextColor(Static->FontColor);
	Static->Label->SetFontSize(Static->FontSize);
	Static->Label->SetFontBold(Static->Bold);
	Static->Label->SetText(_T(""));
	Static->Label->ShowWindow(SW_SHOW);
	Static->Label->UpdateWindow();

	Static = &Patcher.m_Skin.Labels[CONTROL::INFO];
	Rect = CRect(Static->Pos, Static->Size);
	Static->Label = new CLabel();
	Static->Label->Create(nullptr, 0, Rect, this, CONTROL::INFO);
	Static->Label->SetTextColor(Static->FontColor);
	Static->Label->SetFontSize(Static->FontSize);
	Static->Label->SetFontBold(Static->Bold);
	Static->Label->SetText(_T(""));
	Static->Label->ShowWindow(SW_SHOW);
	Static->Label->UpdateWindow();

	/// Создание прогрессбаров
	auto PBar = &Patcher.m_Skin.PBars[CONTROL::CPROGRESS];
	Rect = CRect(PBar->Pos, PBar->Size);
	PBar->ProgressBar = new CProgressCtrl();
	PBar->ProgressBar->Create(0, Rect, this, CONTROL::CPROGRESS);
	PBar->ProgressBar->SetRange32(0, 1);
	PBar->ProgressBar->SetPos(1);
	PBar->ProgressBar->ShowWindow(SW_SHOW);
	PBar->ProgressBar->UpdateWindow();

	PBar = &Patcher.m_Skin.PBars[CONTROL::TPROGRESS];
	Rect = CRect(PBar->Pos, PBar->Size);
	PBar->ProgressBar = new CProgressCtrl();
	PBar->ProgressBar->Create(0, Rect, this, CONTROL::TPROGRESS);
	PBar->ProgressBar->SetRange32(0, 1);
	PBar->ProgressBar->SetPos(1);
	PBar->ProgressBar->ShowWindow(SW_SHOW);
	PBar->ProgressBar->UpdateWindow();

	auto Button = &Patcher.m_Skin.Buttons[BUTTONS::RUN];
	Rect = CRect(Button->Pos, Size);
	Button->Button = new CImgButton();
	Button->Button->Create(_T(""), BS_FLAT, Rect, this, BUTTONS::RUN);
	Button->Button->LoadImages(Button->Active, Button->Hovered, Button->Pressed);
	Button->Button->ShowWindow(SW_SHOW);
	Button->Button->UpdateWindow();

	Button = &Patcher.m_Skin.Buttons[BUTTONS::EXIT];
	Rect = CRect(Button->Pos, Size);
	Button->Button = new CImgButton();
	Button->Button->Create(_T(""), BS_FLAT, Rect, this, BUTTONS::EXIT);
	Button->Button->LoadImages(Button->Active, Button->Hovered, Button->Pressed);
	Button->Button->ShowWindow(SW_SHOW);
	Button->Button->UpdateWindow();

	Button = &Patcher.m_Skin.Buttons[BUTTONS::KRO];
	Rect = CRect(Button->Pos, Size);
	Button->Button = new CImgButton();
	Button->Button->Create(_T(""), BS_FLAT, Rect, this, BUTTONS::KRO);
	Button->Button->LoadImages(Button->Active, Button->Hovered, Button->Pressed);
	Button->Button->ShowWindow(SW_SHOW);
	Button->Button->UpdateWindow();
	DbgMsg(_T("Completed.\n"))

	StartThread();

	return TRUE;
}

void CPatcherForm::DrawForm(void)
{
	auto nStart = GetTickCount();

	DbgMsg(_T("Drawing main form:\t\t\t"));

	int x = 0,
		y = 0,
		nSkinWidth = m_pSkin->GetWidth(),
		nSkinHeight = m_pSkin->GetHeight();

	CRgn Rgn; Rgn.DeleteObject();
	Rgn.CreateRectRgn(0, 0, 0, 0);

	for (; y < nSkinHeight; x = 0, y++)
	{
		for ( ; x < nSkinWidth; )
		{
			// Перебираем прозрачные пиксели
			for (; (x < nSkinWidth) && (m_pSkin->GetPixel(x, y) == CLR_TRANCPERENT); x++);

			// Запоминаем первый не прозрачный пиксель
			auto iLeftX = x;

			// Перебираем не прозрачные пиксели
			for (; (x < nSkinWidth) && (m_pSkin->GetPixel(x, y) != CLR_TRANCPERENT); x++);

			// По полученным данным создаем регион
			CRgn TmpRgn; TmpRgn.CreateRectRgn(iLeftX, y, x, y+1);

			// Применяем созданный регион к основному
			Rgn.CombineRgn(&Rgn, &TmpRgn, RGN_OR);
		}
	}

	x = GetSystemMetrics(SM_CXSCREEN)/2 - (nSkinWidth/2);
	y = GetSystemMetrics(SM_CYSCREEN)/2 - (nSkinHeight/2);
	SetWindowPos(nullptr, x, y, nSkinWidth, nSkinHeight, NULL);
	SetWindowRgn(Rgn, TRUE);

	Rgn.DeleteObject();
	auto nDiff = GetTickCount() - nStart;
	auto nElapsed = static_cast<double>(nDiff / 1000.0);
	DbgMsg(_T("Completed in %.2f seconds.\n"), nElapsed);
}


BOOL CPatcherForm::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CPatcherForm::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	CDialog::OnLButtonDown(nFlags, point);
}

void CPatcherForm::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		auto cxIcon = GetSystemMetrics(SM_CXICON);
		auto cyIcon = GetSystemMetrics(SM_CYICON);

		CRect rect;
		GetClientRect(&rect);
		auto x = (rect.Width() - cxIcon + 1) / 2;
		auto y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		auto MemDC = CDC::FromHandle(m_pSkin->GetDC());
		//BLENDFUNCTION blend;
		//blend.AlphaFormat = AC_SRC_ALPHA;
		//blend.BlendFlags = 0;
		//blend.BlendOp = AC_SRC_OVER;
		//blend.SourceConstantAlpha = 255;
		//dc.AlphaBlend(0, 0, m_pSkin->GetWidth(), m_pSkin->GetHeight(), MemDC, 0, 0, m_pSkin->GetWidth(), m_pSkin->GetHeight(), blend);
		dc.BitBlt(0, 0, m_pSkin->GetWidth(), m_pSkin->GetHeight(),
			MemDC, 0, 0, SRCCOPY);
		m_pSkin->ReleaseDC();
	}
}

HCURSOR CPatcherForm::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CPatcherForm::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CPatcherForm::OnButtonDown(UINT nID)
{
	auto BtnID = static_cast<BUTTONS>(nID);
	auto Buttons = Patcher.m_Skin.Buttons;
	switch (Buttons[BtnID].Action)
	{
	case NOACTION:
		break;

	case CL_RUN:
		//if (Patcher.m_Settings)
		//{
		//	//CLoginForm Login;
		//	//Login.SetOwner(this);
		//	//DeleteTrayIcon();
		//	//ShowWindow(SW_HIDE);
		//	//Login.m_BtnID = BtnID;
		//	//INT_PTR nResponse = Login.DoModal();
		//	//if (nResponse == IDOK)
		//	//{
		//	//}
		//	//else if (nResponse == IDCANCEL)
		//	//{
		//	//	AddTrayIcon();
		//	//	ShowWindow(SW_SHOW);
		//	//}
		//}
		//else
		//{
			RunClient(Buttons[BtnID].FirstParam, Buttons[BtnID].SecondParam);
		//}
		break;

	case APP_RUN:
	case URL_OPEN:
		RunCmd(Buttons[BtnID].FirstParam, Buttons[BtnID].SecondParam);
		break;

	case AS_SHOW:
		break;

	case KRO_UPDATE:
		StartThread(2);
		break;

	case KRORE_UPDATE:
		//StartThread(4);
		break;

	case KP_MINIMIZE:
		ShowWindow(SW_MINIMIZE);
		break;

	case KP_CLOSE:
		if (Patcher.m_bWorking)
		{
			auto iRet = MessageBox(Patcher.m_Language.PatchInProgress,
				_T("Patcher Warning!"), MB_OKCANCEL | MB_ICONWARNING);
			if (iRet == IDOK) Patcher.m_bExiting = TRUE;
			break;
		}
		EndDialog(IDOK);
		break;

	default:
		break;
	}
}
