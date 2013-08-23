#ifndef __DEBUG_H_INCLUDED__
#define __DEBUG_H_INCLUDED__

#define DEBUG_ENABLED ///< Define this to enable debugging. comment to turn off.
#include <windows.h>
#ifdef DEBUG_ENABLED //this allows compilation to skip when debugging is disabled.
#include <windows.h>
#include <stdio.h>

#include "dxerr.h"

#define DEBUG_OUTBUF_SIZE 1024 ///< Size of debug output buffer.
#define DEBUG_FNAME_SIZE 256 ///< Size of debug file name.

class Debugger
{
public:
	Debugger();
	~Debugger();

	void Init(); ///< initialize debugger.
	void Printf(const char* format,...); ///< Debug printf.
	void SetSource(char *file, int line); ///< Set file and line number.
	void Release(); ///< shutdown debugger.

private:
	bool m_bOutputToDebugger; ///< TRUE for debug output to debugger.
	bool m_bPrependFileInfo; ///< TRUE to output filename and line in debug messages.
	bool m_bWriteOutHeader; ///< TRUE if we want to output header file.

	char m_szOutBuffer[DEBUG_OUTBUF_SIZE]; ///< Output Buffer.
	char m_szFileName[DEBUG_FNAME_SIZE]; ///< File that debug message comes from.

	int m_nLineNumber; ///< Line that debug message comes from.
};


void RealDebugPrintF(const char *fmt, ...); ///< DEBUGPRINTF macro calls this function. 

//must be defined as an external (as it is located in the cpp file)
//this allows the DEBUGPRINTF when it is allowed to actually do something to handle the construction / destruction of the debugger object
extern Debugger g_cDebugger;

//below macro allows debugprintf to take a variable number of arguments.
	#define DEBUGPRINTF (g_cDebugger.SetSource(__FILE__,__LINE__), RealDebugPrintF)
	#define DEBUGPRINTFD3D(x) DEBUGPRINTF("D3D Hresult %s: %s\n", DXGetErrorString(x), DXGetErrorDescription(x))
#else
	#define DEBUGPRINTF //does nothing.
	#define DEBUGPRINTFD3D(x) //does nothing.

#endif

#endif