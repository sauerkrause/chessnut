#ifndef __COMMON_H_INCLUDED__
#define __COMMON_H_INCLUDED__
//#define _AMD64_
#include <windows.h>
//Comment the following line to disable sound compilation
//#define __SOUND_ON__

#include <string>
/*#include "EulerAngles.h"
#include "MathUtil.h"
#include "Vector3.h"
*/
#include "Vector.h"
#include "Debug.h"
#include "Settings.h"


extern const char *abortSourceFile;  ///< Contains the name of the source file that called ABORT()
extern int abortSourceLine;  ///< Contains the source line number of a call to ABORT()

bool areWeBeingDebugged();  ///< Detects whether the Windows debugger is running.

// Declare a function to terminate the program with a printf-like
// formatted error message
void reallyAbort(const char *fmt, ...);  ///< Aborts the program.  Called by ABORT()


template <class derived>
class UniversalInterface
{
	bool Init()
	{
		return static_cast<derived*>(this)->Init();
	}
	void Release()
	{
		static_cast<derived*>(this)->Release();
	}
	bool Process()
	{
		static_cast<derived*>(this)->Process();
	}
};


#define ABORT (abortSourceFile = __FILE__, abortSourceLine = __LINE__, reallyAbort)

#endif