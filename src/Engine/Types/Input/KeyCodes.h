#pragma once

#include "../CommonTypes.h"

// Only realistically usable keys
enum class WKey : uint8
{
	// Numbers
	N0,
	N1,
	N2,
	N3,
	N4,
	N5,
	N6,
	N7,
	N8,
	N9,

	ENTER,
	ESCAPE,
	SPACE,
	TAB,
	BACKSPACE,
	CAPSLOCK,
	L_SHIFT,
	L_CONTROL,
	L_ALT,
	R_SHIFT,
	R_CONTROL,
	R_ALT,

	LEFT,
	RIGHT,
	UP,
	DOWN,

	// Debug Keys (function keys)
	// If anyone ever thinks they should use them in gameplay
	// then they should reconsider their choices. These are reserved
	// for debugging and tools, and should be used as such.
	// The types of games that this engine makes does not require F keys to be used.
	DEBUG1,
	DEBUG2,
	DEBUG3,
	DEBUG4,
	DEBUG5,
	DEBUG6,
	DEBUG7,
	DEBUG8,
	DEBUG9,
	DEBUG10,
	DEBUG11,
	DEBUG12,

	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,

	WKey_Count
};

enum class WMouseBtn : uint8
{
	LEFT,
	MIDDLE,
	RIGHT,
	SIDE_CLOSE,
	SIDE_FAR,
	WHEEL_UP,
	WHEEL_DOWN,
	WHEEL_UP_FAST,
	WHEEL_DOWN_FAST,
	WMouseBtn_Count
};

enum class WPadBtn : uint8
{
	// Xbox , Valve , PlayStation , Nintendo (for Copy&Paste)
	South,		///< Xbox A, Valve A, PlayStation Cross, Nintendo B
	East,		///< Xbox B, Valve B, PlayStation Circle, Nintendo A
	West,		///< Xbox X, Valve X, PlayStation Square, Nintendo Y
	North,		///< Xbox Y, Valve Y, PlayStation Triangle, Nintendo X
	Menu1,		///< Xbox Menu, Valve Menu, PlayStation Options, Nintendo +
	Menu2,		///< Xbox View, Valve View, PlayStation Touchpad, Nintendo -
	Menu3,		///< Xbox Guide, Valve Steam, PlayStation PS, Nintendo Home (This is not addressable, Haptic uses this to invoke a game pause.)
	LS,			///< Xbox LS, Valve L3, PlayStation L3, Nintendo LSB
	RS,			///< Xbox RS, Valve R3, PlayStation R3, Nintendo RSB
	LB,			///< Xbox LB, Valve L1, PlayStation L1, Nintendo L Button
	RB,			///< Xbox RB, Valve R1, PlayStation R1, Nintendo R Button
	LT,			///< Xbox LT, Valve L2, PlayStation L2, Nintendo ZL
	RT,			///< Xbox RT, Valve R2, PlayStation R2, Nintendo ZR
	Up,			///< Xbox D-Pad Up, Valve D-Pad Up, PlayStation D-Pad Up, Nintendo D-Pad Up
	Down,		///< Xbox D-Pad Down, Valve D-Pad Down, PlayStation D-Pad Down, Nintendo D-Pad Down
	Left,		///< Xbox D-Pad Left, Valve D-Pad Left, PlayStation D-Pad Left, Nintendo D-Pad Left
	Right,		///< Xbox D-Pad Right,Valve D-Pad Right, PlayStation D-Pad Right, Nintendo D-Pad Right
	Touchpad,	///< Valve Trackpad (any), PlayStation is mapped to Menu2
	L4,			///< Xbox P1, Valve L4, Playstation L4
	R4,			///< Xbox P3, Valve R4, Playstation R4
	L5,			///< Xbox P2, Valve L5
	R5,			///< Xbox P4, Valve R5

	WPadBtn_Count
};
