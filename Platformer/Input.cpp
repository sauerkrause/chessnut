#include "Input.h"

Input::Input(DWORD index)
{
	m_nIndex = (int)index;
	m_xisState = XINPUT_STATE();
	m_xisPrevState = XINPUT_STATE();
}
Input::~Input()
{
}

bool Input::Init()
{
	return true;
}

bool Input::Process()
{
	m_xisPrevState = m_xisState;
	m_xisState = XINPUT_STATE();
	unsigned long keyValues = 0;
	bool actuallyProcesses = (XInputGetState((DWORD)m_nIndex, &m_xisState) == ERROR_SUCCESS) || m_bUseKeyboard;
	if(!actuallyProcesses)
		return false;
	if(m_nIndex == 0)
	{
		for(int i = 0; i < NUM_BUTTONS; i++)
		{
			if(GetAsyncKeyState(m_szKeyMap[i]))
			{
				keyValues |= GetButtonHex((Button)i);
			}
		}
	}
	if(m_bUseKeyboard)
	{
		m_xisState.Gamepad.wButtons |= keyValues;
	}
	return true;
}

long Input::GetButtonHex(Button button)
{
	switch(button)
	{
	case UP:
		return XINPUT_GAMEPAD_DPAD_UP;
	case DOWN:	
		return XINPUT_GAMEPAD_DPAD_DOWN;
	case LEFT:
		return XINPUT_GAMEPAD_DPAD_LEFT;
	case RIGHT:
		return XINPUT_GAMEPAD_DPAD_RIGHT;
	case A:
		return XINPUT_GAMEPAD_A;
    case B:
		return XINPUT_GAMEPAD_B;
	case X:	
		return XINPUT_GAMEPAD_X;
	case Y:
		return XINPUT_GAMEPAD_Y;
	case L:
		return XINPUT_GAMEPAD_LEFT_SHOULDER;
	case R:
		return XINPUT_GAMEPAD_RIGHT_SHOULDER;
	case START:
		return XINPUT_GAMEPAD_START;
	case BACK:
		return XINPUT_GAMEPAD_BACK;
	default:
		return 0;
	}
}

bool Input::ButtonDown(Button button)
{
	return ((m_xisState.Gamepad.wButtons & GetButtonHex(button)) != 0);
}
bool Input::ButtonReleased(Button button)
{
	bool retVal = 0 != (m_xisPrevState.Gamepad.wButtons & GetButtonHex(button) & ~(m_xisState.Gamepad.wButtons & GetButtonHex(button)));
	return retVal;
}
bool Input::ButtonPressed(Button button)
{
	bool retVal = 0 != (m_xisState.Gamepad.wButtons & GetButtonHex(button) & ~(m_xisPrevState.Gamepad.wButtons & GetButtonHex(button)));
	return retVal;
}

void Input::Release()
{
	return;
}

void Input::PassStruct(XINPUT_STATE *passVal)
{
	*passVal = m_xisState;
	return;
}