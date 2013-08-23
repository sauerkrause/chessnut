#ifndef __INPUT_H_INCLUDED__
#define __INPUT_H_INCLUDED__

#include "Common.h"
#include <XInput.h>

enum Button 
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	A,
	B,
	X,
	Y,
	L,
	R,
	START,
	BACK,
	NUM_BUTTONS
};

class Input : UniversalInterface<Input>
{
public:
	Input(DWORD index);
	~Input();
	bool Init();
	bool Init(const char *keymap, bool useKeyboard)
	{
		bool retval = Init();
		m_szKeyMap = keymap;
		m_bUseKeyboard = useKeyboard;
		return retval;
	}
	bool Process();
	bool ButtonDown(Button button);
	bool ButtonReleased(Button button);
	bool ButtonPressed(Button button);
	void Release();
	void PassStruct(XINPUT_STATE *passVal);
	bool HasChanged()
	{
		return (m_xisState.Gamepad.wButtons != m_xisPrevState.Gamepad.wButtons);
	}

private:
	int m_nIndex;
	XINPUT_STATE m_xisState;
	XINPUT_STATE m_xisPrevState;
	const char *m_szKeyMap;
	bool m_bUseKeyboard;
	long m_lButton;
	long GetButtonHex(Button button);
};



#endif