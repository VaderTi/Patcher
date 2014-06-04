#pragma once

typedef struct _LSETTINGS
{
	CString	URL;
	CString	SETTINGS;
	CString	UPDATE;
}_LSETTINGS;

enum class SERVERTYPE
{
	START = 0,
	SERVER_MAIN,
	SERVER_KRO,
	END
};

struct _SERVERINFO
{
	INT		STARTTYPE;
	CString	PATCHLISTURL;
	CString	PATHLISTNAME;
	CString	PATCHSERVERURL;
	CString	GRFNAME;
	CString	PATCHINF;
};

typedef unordered_map<SERVERTYPE, struct _SERVERINFO> __SERVERINFO;

typedef struct _RSETTINGS
{
	// [Settings::Main]
	CString		SERVER_NAME;
	CString		SERVER_UID;
	CString		PATCHER_DATA;

	// [Settings::Browsers]
	CString		NOTICE_URL;

	// [Settings::PatchServer]
	__SERVERINFO	SERVERINFO;

	// [Settings::Executable]
	INT			AUTOSTART;
	CString		EXENAME;
	CString		EXEPARAM;
}_RSETTINGS;

class CSettings
{
public:
	CSettings(void);
	~CSettings(void);
	void LoadLocal(const CString FileName = CString());
	void LoadRemote(const CString FileName = CString());

	// Local settings
	_LSETTINGS m_lSettings;
	// Remote settings
	_RSETTINGS m_rSettings;
};

