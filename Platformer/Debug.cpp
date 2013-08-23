#include <string.h>
#include "Debug.h"

#ifdef DEBUG_ENABLED // this will allow for compilation to skip when debug is not enabled.

Debugger g_cDebugger;

Debugger::Debugger()
{
	m_bOutputToDebugger = true; ///< output to debugger.
	m_bPrependFileInfo = true; ///< output filename and line in debug messages.
	m_bWriteOutHeader = true; ///< output header onto debug output.
	Init();
}

Debugger::~Debugger()
{
	Release();
}

void Debugger::Init()
{
	if(m_bWriteOutHeader)
	{
		bool temp = m_bPrependFileInfo;
		m_bPrependFileInfo = false;
		Printf("**** Sick Engine started\n");
		m_bPrependFileInfo = temp;
	}
}
void Debugger::Release()
{
	if(m_bWriteOutHeader)
	{
		bool temp = m_bPrependFileInfo;
		m_bPrependFileInfo = false;
		Printf("**** Sick Engine ending\n");
		m_bPrependFileInfo = temp; //why do I even bother with this sort of thing?
	}
}

void Debugger::SetSource(char *file, int line)
{
	char* p = strrchr(file, '\\'); //scans string for last occurrence of \ (folder delimiter)
	if(p==NULL)
		strcpy_s(m_szFileName, file); //set the file name because it is in root directory.
	else
		strcpy_s(m_szFileName,(char *)(p+1)); //sets the file name to only characters after last \ character.
	m_nLineNumber = line; //sets the line number to the input parameter.
}

/// Printf function
/// \param format printf style format string

void Debugger::Printf(const char* format,...)
{
	if(m_bPrependFileInfo) // if we want to prepend these,
		sprintf_s(m_szOutBuffer, "%s(%d): ", m_szFileName, m_nLineNumber);
	else
		m_szOutBuffer[0] = '\0'; //set first character to null pointer (empty string)

	//append debug message from parameter list using charv , chara formatting.
	va_list arglist;
	va_start(arglist, format);
	//sprintf_s function for stdio style formatting.
	_vsnprintf_s((char*)(m_szOutBuffer+strlen(m_szOutBuffer)),sizeof(m_szOutBuffer),
		sizeof(m_szOutBuffer)-strlen(m_szOutBuffer),format,arglist);
	va_end(arglist);

	// output to vs debugger.
	if(m_bOutputToDebugger)
		OutputDebugString(m_szOutBuffer);

	//all this just to output some text... :-/

}

//extern CDebugManager g_cDebugger; ///< so we can access the class from below function

//yay for non-member functions.
//only call if you have a debugger initialized.
void RealDebugPrintF(const char *fmt, ...)
{
	static char buffer[1024];
	va_list ap;
	va_start(ap,fmt);
	_vsnprintf_s(buffer, sizeof(buffer), fmt, ap);
	g_cDebugger.Printf("%s", buffer);
	va_end(ap);



}

#endif