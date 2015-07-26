#include "StdAfx.h"
#include "Res.h"
#include "Settings.h"


CSettings::CSettings(void)
{
}


CSettings::~CSettings(void)
{
}


void CSettings::LoadLocal(const CString FileName)
{
	auto File = FileName;
	if (File.IsEmpty())
	{
		CRes Res;
		File = "./local.ini";
		if (Res.SaveResource(_T("SETTINGS"), _T("./local.ini"), nullptr) == nullptr) return;
	}

	CIni Ini;
	Ini.LoadFile(File);
	DeleteFile(File);

	Ini.SetSection(_T("Settings::Remote"));
	
	m_lSettings.URL = Ini.GetString(_T("Url"), _T("http://localhost/patcher/"));
	m_lSettings.SETTINGS = Ini.GetString(_T("Settings"), _T("rsettings.ini"));
	m_lSettings.UPDATE = Ini.GetString(_T("Update"), _T("update.ini"));
}


void CSettings::LoadRemote(const CString FileName)
{
	auto File = FileName;
	if (File.IsEmpty())
	{
		CInternet Inet;
		Inet.Connect(m_lSettings.URL);
		Inet.Download(m_lSettings.SETTINGS, FALSE);
		Inet.Disconnect();
		File = _T("./") + m_lSettings.SETTINGS;
	}

	CIni Ini;
	Ini.LoadFile(File); //DeleteFile(File);

	Ini.SetSection(_T("Settings::Main"));
	m_rSettings.SERVER_NAME = Ini.GetString(_T("ServerName"), _T("Patcher V3.0"));
	m_rSettings.SERVER_UID = Ini.GetString(_T("ServerUID"), _T("PATCHER"));
	//PatcherData = Ini.GetString(_T("PatcherData"), _T("info.inf"));

	Ini.SetSection(_T("Settings::Browsers"));
	m_rSettings.NOTICE_URL = Ini.GetString(_T("NoticeURL"), _T("http://localhost/patcher/index.php"));

	Ini.SetSection(_T("Settings::PatchServer::Main"));
	auto Server = &m_rSettings.SERVERINFO[SERVERTYPE::SERVER_MAIN];
	Server->STARTTYPE = 0;
	Server->PATCHLISTURL = Ini.GetString(_T("PatchListURL"), _T("http://localhost/patcher/"));
	Server->PATHLISTNAME = Ini.GetString(_T("PatchListName"), _T("patch.lst"));
	Server->PATCHSERVERURL = Ini.GetString(_T("FileServerURL"), _T("ftp://localhost/patcher/"));
	Server->GRFNAME = Ini.GetString(_T("Grf"), _T("server.grf"));
	Server->PATCHINF = Ini.GetString(_T("PatchStore"), _T("info.inf"));

	Ini.SetSection(_T("Settings::PatchServer::KRO"));
	if (m_rSettings.SERVERINFO[SERVERTYPE::SERVER_KRO].STARTTYPE = Ini.GetInt(_T("StartType"), 0))
	{
		Server = &m_rSettings.SERVERINFO[SERVERTYPE::SERVER_KRO];
		Server->PATCHSERVERURL = Ini.GetString(_T("PatchListURL"), _T("http://webpatch.ragnarok.co.kr/patch/"));
		Server->PATHLISTNAME = Ini.GetString(_T("PatchListName"), _T("patch.txt"));
		Server->PATCHSERVERURL = Ini.GetString(_T("FileServerURL"), _T("ftp://ragnarok.nowcdn.co.kr:20021/Patch/"));
		Server->GRFNAME = Ini.GetString(_T("Grf"), _T("data.grf"));
		Server->PATCHINF = Ini.GetString(_T("PatchStore"), _T("patch.inf"));
	}

	Ini.SetSection(_T("Settings::Executable"));
//	NewLogin = Ini.GetInt(_T("NewLogin"), 0);
	m_rSettings.AUTOSTART = Ini.GetInt(_T("AutoStart"), 0);
	m_rSettings.EXENAME = Ini.GetString(_T("ExeName"), _T("patcher.exe"));
	m_rSettings.EXEPARAM = Ini.GetString(_T("ExeParam"), _T(""));

	//Ini.SetSection(_T("Settings::PatchServer::KRORE"));
	//if (PatchServer[KRORE_SERVER].StartType = Ini.GetInt(_T("StartType"), 0))
	//{
	//	Server = &PatchServer[KRORE_SERVER];
	//	Server->PatchListURL = Ini.GetString(_T("PatchListURL"), _T("http://webpatch.ragnarok.co.kr/patch/"));
	//	Server->PatchListName = Ini.GetString(_T("PatchListName"), _T("patchRE.txt"));
	//	Server->FileServerURL = Ini.GetString(_T("FileServerURL"), _T("ftp://ragnarok.nowcdn.co.kr:20021/Patch/"));
	//	Server->PatchGrfName = Ini.GetString(_T("GrfName"), _T("rdata.grf"));
	//	Server->PatchInfo = Ini.GetString(_T("PatchInfo"), _T("patchRE.inf"));
	//}
}
