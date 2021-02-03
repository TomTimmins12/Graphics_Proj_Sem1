#include "Input.h"

Input::Input(HWND hwnd)
{
	_pDIKeyboard = nullptr;
	_pDIMouse = nullptr;

	_hWnd = hwnd;
    m_counter = 0;
}

Input::~Input()
{
    
}


HRESULT Input::InitInput(HINSTANCE hInstance)          ////////////////////////////////////////////////////////////////////////////////////////////////
{
	HRESULT hr;

	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_directInput,
		NULL);

	hr = m_directInput->CreateDevice(GUID_SysKeyboard,
		&_pDIKeyboard,
		NULL);

	hr = m_directInput->CreateDevice(GUID_SysMouse,
		&_pDIMouse,
		NULL);

	hr = _pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = _pDIKeyboard->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = _pDIMouse->SetDataFormat(&c_dfDIMouse);
	hr = _pDIMouse->SetCooperativeLevel(_hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	if (FAILED(hr))
		return hr;

	return S_OK;
}


int Input::DetectKeyboardInput(FLOAT time)
{
	_pDIKeyboard->Acquire();

	_pDIKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);

	if (m_keyboardState[DIK_ESCAPE] & 0x80)
		return 0;

	float speed = 15.0f * time;

	if (m_keyboardState[DIK_A] & 0x80)
	{
		return 1;
	}
	if (m_keyboardState[DIK_D] & 0x80)
	{
		return 2;
	}
	if (m_keyboardState[DIK_W] & 0x80)
	{
		return 3;
	}
	if (m_keyboardState[DIK_S] & 0x80)
	{
		return 4;
	}
	if (m_keyboardState[DIK_P] & 0x80)
	{
		return 5;
	}
	if (m_keyboardState[DIK_O] & 0x80)
	{
		return 6;
	}
	if (m_keyboardState[DIK_Q] & 0x80)
	{
		return 7;
	}
	if (m_keyboardState[DIK_E] & 0x80)
	{
		return 8;
	}


	else {
		return -1;
	}
}

void Input::DetectMouseInput(FLOAT time)
{
	_pDIMouse->Acquire();

	_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseCurrState);
}