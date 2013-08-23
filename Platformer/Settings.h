#ifndef __SETTINGS_H_INCLUDED__
#define __SETTINGS_H_INCLUDED__

#include "TinyXML\tinyxml.h"
#include "Common.h"

class SettingsManager
{
public:
	SettingsManager();
	~SettingsManager();
	bool Init(char *filename);
	void Release();
	bool LoadStartupSettings();
	bool LoadKeymap();

	int m_nWidth; //width in pixels of resolution
	int m_nHeight; // height in pixels of resolution
	int m_nRefreshRate; // ref rate of monitor in hz
	bool m_bFullscreen;
	bool m_bVsync;
	char* m_szFilename;
	const char* m_szP1KeyMap;
	bool m_bUseKeyboard;

private:
	TiXmlDocument m_xmlDoc;
	TiXmlElement *m_xmlSettings;
};

#endif