#ifndef __COMMON_H_INCLUDED__
#define __COMMON_H_INCLUDED__

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Renderer.h"

#ifdef WIN32
	#include <windows.h>
#endif

const char	*abortSourceFile = "(unknown)";
int		abortSourceLine;

// globals for handling abort error message reporting
char g_errMsg[1024]; //error message if we had to abort
bool g_errorExit=false; // true if we had to abort

bool	areWeBeingDebugged() {
	#ifdef WIN32

		// This routine is in kernel32.  Acquire access to the
		// DLL

		HINSTANCE h = LoadLibrary(TEXT("kernel32.dll"));
		if (h == NULL) {
			return false;
		}

		// Fetch the code

		typedef BOOL (*PFunc)(VOID);

		PFunc func = (PFunc)GetProcAddress(h, "IsDebuggerPresent");
		if (func == NULL) {
			return false;
		}

		// Actually call the routine

		bool result = (func() != FALSE);

		// Don't hang on to any resources we don't need

		FreeLibrary(h);

		// Return the result

		return result;
	#else

		// For now, let's assume we are being debugged
		// in a debug build, and otherwise we're not being
		// debugged

		#ifdef _DEBUG
			return true;
		#else
			return false;
		#endif

	#endif
}

void	reallyAbort(const char *fmt, ...) {

	// Format the error message into our buffer

	va_list ap;
	va_start(ap, fmt);
	//SECURITY-UPDATE:2/3/07
	//vsprintf(g_errMsg, fmt, ap);
	vsprintf_s(g_errMsg,sizeof(g_errMsg), fmt, ap);
	va_end(ap);

	// Tack on the source file and line number

	//SECURITY-UPDATE:2/3/07
	//sprintf(strchr(g_errMsg, '\0'), "\n%s line %d", abortSourceFile, abortSourceLine);
	sprintf_s(strchr(g_errMsg, '\0'),sizeof(g_errMsg)-strlen(g_errMsg), "\n%s line %d", abortSourceFile, abortSourceLine);

	// Shutdown renderer so we can see the error message

	gRenderer->Release();

	// Windows?  Dump message box

	#ifdef WIN32

		// Check if we're under the debugger

		if (areWeBeingDebugged()) {

			// Dump error message to the debug console

			OutputDebugString(TEXT("FATAL ERROR: "));
			OutputDebugString(g_errMsg);
			OutputDebugString(TEXT("\n"));

			// Break the program, so we can check out what was going on.

			/*_asm {

				// HELLO! If you hit this breakpoint, then look at the debug
				// console, and go up the call stack to see what went wrong!

				int 3;
			}*/
			assert(0);

			// You can try to keep running, if you want...

		} else {

			// Just dump a message box and terminate the app
			g_errorExit=true; // flag that there's been an error
			//DestroyWindow(gWindowsWrapper.getHandle()); // post quit message so we can show dialog box last
			DestroyWindow(gRenderer->GetWindowHandle());
			ExitProcess(1); // so we don't proceed with whatever caused this abort
		}
	#else

		// Just dump it to printf and use exit.  On most OSs,
		// this is basically useless for debugging, so you'd
		// want to do better, especially under the debugger

		printf("FATAL ERROR: %s\n", errMsg);
		exit(1);

	#endif

#endif
}