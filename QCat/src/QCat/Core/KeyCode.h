#pragma once
// From Window Virtual Keycode

#define QCAT_KEY_BACKSPACE  0x08
#define QCAT_KEY_TAB		0x09
#define QCAT_KEY_CLEAR		0x0C
#define QCAT_KEY_RETRUN		0x0D
#define QCAT_KEY_SHIFT		0x10
#define QCAT_KEY_LSHIFT		0xA0
#define QCAT_KEY_RSHIFT		0xA1

#define QCAT_KEY_CONTROL	0x11
#define QCAT_KEY_LCONTROL   0xA2
#define QCAT_KEY_RCONTROL	0xA3

#define QCAT_KEY_MENU		0x12
#define QCAT_KEY_LMENU		0xA4
#define QCAT_KEY_RMENU		0xA5

#define QCAT_KEY_PAUSE		0x13
#define QCAT_KEY_CAPITAL	0x14
#define QCAT_KEY_ESCAPE		0x1B
#define QCAT_KEY_SPACE		0x20
#define QCAT_KEY_PRIOR		0x21
#define QCAT_KEY_NEXT		0x22
#define QCAT_KEY_END		0x23
#define QCAT_KEY_HOME		0x24
#define QCAT_KEY_LEFT		0x25
#define QCAT_KEY_UP			0x26
#define QCAT_KEY_RIGHT		0x27
#define QCAT_KEY_DOWN		0x28
#define QCAT_KEY_SELECT		0x29
#define QCAT_KEY_PRINT		0x2A
#define QCAT_KEY_EXECUTE	0x2B
#define QCAT_KEY_SNAPSHOT	0x2C
#define QCAT_KEY_INSERT		0x2D
#define QCAT_KEY_DELETE		0x2E
#define QCAT_KEY_HELP		0x2F
#define QCAT_KEY_PRINT		0x2A

#define QCAT_KEY_0			0x30
#define QCAT_KEY_1			0x31
#define QCAT_KEY_2			0x32
#define QCAT_KEY_3			0x33
#define QCAT_KEY_4			0x34
#define QCAT_KEY_5			0x35
#define QCAT_KEY_6			0x36
#define QCAT_KEY_7			0x37
#define QCAT_KEY_8			0x38
#define QCAT_KEY_9			0x39
#define QCAT_KEY_A			0x41
#define QCAT_KEY_B			0x42
#define QCAT_KEY_C			0x43
#define QCAT_KEY_D			0x44
#define QCAT_KEY_E			0x45
#define QCAT_KEY_F			0x46
#define QCAT_KEY_G			0x47
#define QCAT_KEY_H			0x48
#define QCAT_KEY_I			0x49
#define QCAT_KEY_J			0x4A
#define QCAT_KEY_K			0x4B
#define QCAT_KEY_L			0x4C
#define QCAT_KEY_M			0x4D
#define QCAT_KEY_N			0x4E
#define QCAT_KEY_O			0x4F
#define QCAT_KEY_P			0x50
#define QCAT_KEY_Q			0x51
#define QCAT_KEY_R			0x52
#define QCAT_KEY_S			0x53
#define QCAT_KEY_T			0x54
#define QCAT_KEY_U			0x55
#define QCAT_KEY_V			0x56
#define QCAT_KEY_W			0x57
#define QCAT_KEY_X			0x58
#define QCAT_KEY_Y			0x59
#define QCAT_KEY_Z			0x5A

#define QCAT_KEY_LWIN		0x5B
#define QCAT_KEY_RWIN		0x5C
#define QCAT_KEY_APPS		0x5D
#define QCAT_KEY_SLEEP		0x5F

#define QCAT_KEY_NUMPAD0	0x60
#define QCAT_KEY_NUMPAD1	0x61
#define QCAT_KEY_NUMPAD2	0x62
#define QCAT_KEY_NUMPAD3	0x63
#define QCAT_KEY_NUMPAD4	0x64
#define QCAT_KEY_NUMPAD5	0x65
#define QCAT_KEY_NUMPAD6	0x66
#define QCAT_KEY_NUMPAD7	0x67
#define QCAT_KEY_NUMPAD8	0x68
#define QCAT_KEY_NUMPAD9	0x69

#define QCAT_KEY_MULTIPLY	0x6A
#define QCAT_KEY_ADD		0x6B
#define QCAT_KEY_SEPARATOR	0x6C
#define QCAT_KEY_SUBTRACT	0x6D
#define QCAT_KEY_DECIMAL	0x6E
#define QCAT_KEY_DIVIDE		0x6F

#define QCAT_KEY_FUNCTION1	0x70
#define QCAT_KEY_FUNCTION2	0x71
#define QCAT_KEY_FUNCTION3	0x72
#define QCAT_KEY_FUNCTION4	0x73
#define QCAT_KEY_FUNCTION5	0x74
#define QCAT_KEY_FUNCTION6	0x75
#define QCAT_KEY_FUNCTION7	0x76
#define QCAT_KEY_FUNCTION8	0x77
#define QCAT_KEY_FUNCTION9	0x78
#define QCAT_KEY_FUNCTION10	0x79
#define QCAT_KEY_FUNCTION11	0x7A
#define QCAT_KEY_FUNCTION12 0x7B

#define QCAT_KEY_NUMLOCK	0x90
#define QCAT_KEY_SCROLL		0x91

#define QCAT_KEY_LSHIFT		0xA0
#define QCAT_KEY_RSHIFT		0xA1
#define QCAT_KEY_LCONTROL	0xA2
#define QCAT_KEY_RCONTROL	0xA3
#define QCAT_KEY_LMENU		0xA4
#define QCAT_KEY_RMENU		0xA5

#define QCAT_MOUSE_BUTTON_1         0
#define QCAT_MOUSE_BUTTON_2         1
#define QCAT_MOUSE_BUTTON_3         2
#define QCAT_MOUSE_BUTTON_4         3
#define QCAT_MOUSE_BUTTON_5         4
#define QCAT_MOUSE_BUTTON_6         5
#define QCAT_MOUSE_BUTTON_7         6
#define QCAT_MOUSE_BUTTON_8         7

#define QCAT_MOUSE_BUTTON_LAST      QCAT_MOUSE_BUTTON_8
#define QCAT_MOUSE_BUTTON_LEFT      QCAT_MOUSE_BUTTON_1
#define QCAT_MOUSE_BUTTON_RIGHT     QCAT_MOUSE_BUTTON_2
#define QCAT_MOUSE_BUTTON_MIDDLE    QCAT_MOUSE_BUTTON_3

namespace QCat
{
	using KeyCode = uint16_t;

	namespace Key
	{
		enum : KeyCode
		{
			// From glfw3.h
			Space = 32,
			Apostrophe = 39, /* ' */
			Comma = 44, /* , */
			Minus = 45, /* - */
			Period = 46, /* . */
			Slash = 47, /* / */

			D0 = 48, /* 0 */
			D1 = 49, /* 1 */
			D2 = 50, /* 2 */
			D3 = 51, /* 3 */
			D4 = 52, /* 4 */
			D5 = 53, /* 5 */
			D6 = 54, /* 6 */
			D7 = 55, /* 7 */
			D8 = 56, /* 8 */
			D9 = 57, /* 9 */

			Semicolon = 59, /* ; */
			Equal = 61, /* = */

			A = 65,
			B = 66,
			C = 67,
			D = 68,
			E = 69,
			F = 70,
			G = 71,
			H = 72,
			I = 73,
			J = 74,
			K = 75,
			L = 76,
			M = 77,
			N = 78,
			O = 79,
			P = 80,
			Q = 81,
			R = 82,
			S = 83,
			T = 84,
			U = 85,
			V = 86,
			W = 87,
			X = 88,
			Y = 89,
			Z = 90,

			LeftBracket = 91,  /* [ */
			Backslash = 92,  /* \ */
			RightBracket = 93,  /* ] */
			GraveAccent = 96,  /* ` */

			World1 = 161, /* non-US #1 */
			World2 = 162, /* non-US #2 */

			/* Function keys */
			Escape = 27,
			Enter = 257,
			Tab = 258,
			Backspace = 259,
			Insert = 260,
			Delete = 261,
			Right = 262,
			Left = 263,
			Down = 264,
			Up = 265,
			PageUp = 266,
			PageDown = 267,
			Home = 268,
			End = 269,
			CapsLock = 280,
			ScrollLock = 281,
			NumLock = 282,
			PrintScreen = 283,
			Pause = 284,
			F1 = 290,
			F2 = 291,
			F3 = 292,
			F4 = 293,
			F5 = 294,
			F6 = 295,
			F7 = 296,
			F8 = 297,
			F9 = 298,
			F10 = 299,
			F11 = 300,
			F12 = 301,
			F13 = 302,
			F14 = 303,
			F15 = 304,
			F16 = 305,
			F17 = 306,
			F18 = 307,
			F19 = 308,
			F20 = 309,
			F21 = 310,
			F22 = 311,
			F23 = 312,
			F24 = 313,
			F25 = 314,

			/* Keypad */
			KP0 = 320,
			KP1 = 321,
			KP2 = 322,
			KP3 = 323,
			KP4 = 324,
			KP5 = 325,
			KP6 = 326,
			KP7 = 327,
			KP8 = 328,
			KP9 = 329,
			KPDecimal = 330,
			KPDivide = 331,
			KPMultiply = 332,
			KPSubtract = 333,
			KPAdd = 334,
			KPEnter = 335,
			KPEqual = 336,

			LeftShift = QCAT_KEY_LSHIFT,
			LeftControl = QCAT_KEY_LCONTROL,
			LeftAlt = QCAT_KEY_LMENU,
			LeftSuper = 343,
			RightShift = QCAT_KEY_RSHIFT,
			RightControl = QCAT_KEY_RCONTROL,
			RightAlt = QCAT_KEY_RMENU,
			RightSuper = 347,
			Menu = 348
		};
	}
}







