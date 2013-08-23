#include "Settings.h"
#include "Debug.h"
#include <string>

SettingsManager::SettingsManager()
{
	m_nWidth = 1024;
	m_nHeight = 768;
	m_bFullscreen = false;
	m_bVsync = true;
	m_szFilename = NULL;
	m_bUseKeyboard = false;
}

SettingsManager::~SettingsManager()
{
	Release();
}

bool SettingsManager::Init(char *filename)
{
	m_szFilename = filename;
	m_xmlDoc = TiXmlDocument(filename);
	return true;
}

bool SettingsManager::LoadStartupSettings()
{
	if(!m_xmlDoc.LoadFile())
	{
		DEBUGPRINTF("Could not load settings file %s.\n", m_szFilename);
		return false;
	}
	m_xmlSettings = m_xmlDoc.FirstChildElement("settings");
	if(!m_xmlSettings)
	{
		DEBUGPRINTF("Could not find <settings> tag in %s.\n", m_szFilename);
		return false;
	}

	TiXmlElement* startupSettings = m_xmlSettings->FirstChildElement("renderer"); //renderer tag
	if(startupSettings)
	{
		int tmp;
		startupSettings->Attribute("width", &m_nWidth);
		startupSettings->Attribute("height", &m_nHeight);
		startupSettings->Attribute("fullscreen", &tmp);
		m_bFullscreen = (tmp!=0);
		startupSettings->Attribute("vsync", &tmp);
		m_bVsync = (tmp!=0);
		startupSettings->Attribute("refreshrate", &m_nRefreshRate);
	}
	else
	{
		DEBUGPRINTF("No <render> tag under <settings> in %s.\n", m_szFilename);
		return false;
	}
	startupSettings = m_xmlSettings->FirstChildElement("input"); //input tag
	if(startupSettings)
	{

#if 0
		std::string strTmp = std::string();

		TiXmlElement * pTempSetting = 0;
		if(pTempSetting = startupSettings->FirstChildElement("up"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		if(pTempSetting = startupSettings->FirstChildElement("down"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		if(pTempSetting = startupSettings->FirstChildElement("left"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		if(pTempSetting = startupSettings->FirstChildElement("right"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		if(pTempSetting = startupSettings->FirstChildElement("A"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		if(pTempSetting = startupSettings->FirstChildElement("B"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		if(pTempSetting = startupSettings->FirstChildElement("X"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		if(pTempSetting = startupSettings->FirstChildElement("Y"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		if(pTempSetting = startupSettings->FirstChildElement("L"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		if(pTempSetting = startupSettings->FirstChildElement("R"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		if(pTempSetting = startupSettings->FirstChildElement("start"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		if(pTempSetting = startupSettings->FirstChildElement("back"))
		{
			strTmp.append(pTempSetting->GetText());
			pTempSetting->~TiXmlElement();
			pTempSetting = NULL;
		}
		DEBUGPRINTF("Current Keymap is: %s\n", strTmp);
		m_nKeyMapSize = strTmp.length();
#endif
		if(startupSettings->Attribute("keymap"))
		{
			m_szP1KeyMap = startupSettings->Attribute("keymap");
		}

		int tmp;
		startupSettings->Attribute("keyboard", &tmp);
		m_bUseKeyboard = (tmp != 0);
	}
	else
	{
		DEBUGPRINTF("No <input> tag under <settings> in %s.\n", m_szFilename);
		return false;
	}
	return true;
}

void SettingsManager::Release()
{
	m_szFilename = NULL;
	m_xmlSettings->Clear();
	m_xmlSettings = NULL;
	m_xmlDoc.Clear();
	m_xmlDoc.~TiXmlDocument();
}

