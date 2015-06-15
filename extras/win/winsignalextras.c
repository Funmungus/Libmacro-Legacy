/* extras/win/winsignalextras.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "extras/win/def.h"
# include "extras/extras.h"

static void add_nonshifts ( ) ;
static void add_shifts ( ) ;
static void get_strings_redirect ( mcr_SafeString * ssPt,
		mcr_Array * envMemPt ) ;
static void putenv_redirect ( mcr_Array * envStr, ... ) ;
static void arr_free_redirect ( mcr_Array * arrPt, ... ) ;

static void add_keymods ( ) ;
static void add_keynames ( ) ;
static void add_echokeys ( ) ;
static void add_echonames ( ) ;

int mcr_Command_execvpe ( mcr_Command * cmdPt )
{
	dassert ( cmdPt ) ;
	int success = 0 ;
	size_t i ;
	mcr_Array file = mcr_SafeString_get ( & cmdPt->file ) ;
	mcr_Array * argsArr = malloc ( sizeof ( mcr_Array ) *
			( cmdPt->argv.used ) ) ;
	char ** args = malloc ( sizeof ( char * ) *
			( cmdPt->argv.used + 1 ) ) ;
	// Get environment and args beforehand, to decrypt before forking.
	mcr_Array envMem ;
	mcr_Array_init ( & envMem, sizeof ( mcr_Array ) ) ;
	MCR_ARR_FOR_EACH ( & cmdPt->env, get_strings_redirect, & envMem ) ;

	if ( ( ! cmdPt->argv.used || argsArr ) && args
			&& ! MCR_STR_ISEMPTY ( & file ) )
	{
		for ( i = 0 ; i < cmdPt->argv.used ; i ++ )
		{
			argsArr [ i ] = mcr_SafeString_get (
					MCR_ARR_AT ( & cmdPt->argv, i ) ) ;
			args [ i ] = argsArr [ i ].array ;
		}
		// Need at least one args for null termination.
		args [ cmdPt->argv.used ] = NULL ;

		pid_t child = fork ( ) ;
		if ( child )
		{
			if ( waitpid ( child, & success, 0 ) == -1 )
			{
				dmsg ;
			}
			else
				success = 1 ;
		}
		else
		{
			/* TODO: Assuming environment changes only effect child. */
			MCR_ARR_FOR_EACH ( & envMem, putenv_redirect, 0 ) ;
			if ( execvp ( file.array, args ) == -1 )
			{
				dmsg ;
			}
			abort ( ) ;
			/* abort abort */
		}
	}

	mcr_Array_free ( & file ) ;
	if ( argsArr )
	{
		for ( i = 0 ; i < cmdPt->argv.used ; i ++ )
		{
			mcr_Array_free ( argsArr + i ) ;
		}
		free ( argsArr ) ;
	}
	free ( args ) ;
	MCR_ARR_FOR_EACH ( & envMem, arr_free_redirect, 0 ) ;
	mcr_Array_free ( & envMem ) ;
	return success ;
}

void mcr_StringKey_load_contract ( )
{
	add_shifts ( ) ;
	add_nonshifts ( ) ;
}

void mcr_Key_load_contract ( )
{
	add_keymods ( ) ;
	add_keynames ( ) ;
}

void mcr_Echo_load_contract ( )
{
	add_echokeys ( ) ;
	add_echonames ( ) ;
}

static const int _keyVals [ ] =
{
	0, 	//Null character
	0, 	//01	SOH		^A		Start of Header
	0, 	//02	STX		^B		Start of Text
	0, 	//03	ETX		^C		End of Text
	0, 	//04	EOT		^D		End of Transmission
	0, 	//05	ENQ		^E		Enquiry
	0, 	//06	ACK		^F		Acknowledgment
	0, 	//07	BEL		^G	\a	Bell
	VK_BACK, 	//08	BS		^H	\b	Backspace [ d ] [ e ]
	VK_TAB, 		//09	HT		^I	\t	Horizontal Tab [ f ]
	VK_RETURN, 	//0A	LF		^J	\n	Line feed
	0, 	//0B	VT		^K	\v	Vertical Tab
	0, 	//0C	FF		^L	\f	Form feed
	VK_RETURN, 	//0D	CR		^M	\r	Carriage return [ g ]
	0, 	//0E	SO		^N		Shift Out
	0, 	//0F	SI		^O		Shift In
	0, 	//10	DLE		^P		Data Link Escape
	0, 	//11	DC1		^Q		Device Control 1 ( oft. XON )
	0, 	//12	DC2		^R		Device Control 2
	0, 	//13	DC3		^S		Device Control 3 ( oft. XOFF )
	0, 	//14	DC4		^T		Device Control 4
	0, 	//15	NAK		^U		Negative Acknowledgement
	0, 	//16	SYN		^V		Synchronous idle
	0, 	//17	ETB		^W		End of Transmission Block
	0, 	//18	CAN		^X		Cancel
	0, 	//19	EM		^Y		End of Medium
	0, 	//1A	SUB		^Z		Substitute
	VK_ESCAPE, 	//1B	ESC		^ [ 	\e [ h ] 	Escape [ i ]
	0, 	//1C	FS		^\		File Separator
	0, 	//1D	GS		^ ] 		Group Separator
	0, 	//1E	RS		^^ [ j ] 		Record Separator
	0, 	//1F	US		^_		Unit Separator
	' ', 	//20	Space
	'1', 	//21	!
	VK_OEM_7, 	//22	"
	'3', 	//23	#
	'4', 	//24	$
	'5', 	//25	%
	'7', 	//26	&
	VK_OEM_7, 	//27	'
	'9', 	//28	(
	'0', 	//29	)
	'8', 	//2A	 *
	VK_OEM_PLUS, 	//2B	+
	VK_OEM_COMMA, 	//2C	,
	VK_OEM_MINUS, 	//2D	-
	VK_OEM_PERIOD, 	//2E	.
	VK_OEM_2, 		//2F	/
	'0', 	//30	0
	'1', 	//31	1
	'2', 	//32	2
	'3', 	//33	3
	'4', 	//34	4
	'5', 	//35	5
	'6', 	//36	6
	'7', 	//37	7
	'8', 	//38	8
	'9', 	//39	9
	VK_OEM_1, 		//3A	:
	VK_OEM_1, 		//3B	;
	VK_OEM_COMMA, 	//3C	<
	VK_OEM_PLUS, 	//3D	=
	VK_OEM_PERIOD, 	//3E	>
	VK_OEM_2, 		//3F	?
	'2', 	//40	@
	'A', 	//41	A
	'B', 	//42	B
	'C', 	//43	C
	'D', 	//44	D
	'E', 	//45	E
	'F', 	//46	F
	'G', 	//47	G
	'H', 	//48	H
	'I', 	//49	I
	'J', 	//4A	J
	'K', 	//4B	K
	'L', 	//4C	L
	'M', 	//4D	M
	'N', 	//4E	N
	'O', 	//4F	O
	'P', 	//50	P
	'Q', 	//51	Q
	'R', 	//52	R
	'S', 	//53	S
	'T', 	//54	T
	'U', 	//55	U
	'V', 	//56	V
	'W', 	//57	W
	'X', 	//58	X
	'Y', 	//59	Y
	'Z', 	//5A	Z
	VK_OEM_4, 		//5B	[
	VK_OEM_5, 		//5C	\
	VK_OEM_6, 		//5D	]
	'6', 	//5E	^
	VK_OEM_MINUS, 	//5F	_
	VK_OEM_3, 		//60	`
	'A', 	//61	a
	'B', 	//62	b
	'C', 	//63	c
	'D', 	//64	d
	'E', 	//65	e
	'F', 	//66	f
	'G', 	//67	g
	'H', 	//68	h
	'I', 	//69	i
	'J', 	//6A	j
	'K', 	//6B	k
	'L', 	//6C	l
	'M', 	//6D	m
	'N', 	//6E	n
	'O', 	//6F	o
	'P', 	//70	p
	'Q', 	//71	q
	'R', 	//72	r
	'S', 	//73	s
	'T', 	//74	t
	'U', 	//75	u
	'V', 	//76	v
	'W', 	//77	w
	'X', 	//78	x
	'Y', 	//79	y
	'Z', 	//7A	z
	VK_OEM_4, 	//7B	{
	VK_OEM_5, 	//7C	|
	VK_OEM_6, 	//7D	}
	VK_OEM_3, 	//7E	~
} ;

# define add( character ) \
	if ( character <= 0x7E && _keyVals [ character ] ) \
		mcr_StringKey_set_nonshifted ( character, \
				_keyVals [ character ], 250000000 ) ;
# define arange( iterator, charMin, charMax ) \
	for ( iterator = charMin ; iterator <= charMax ; iterator ++ ) \
	{ \
		add ( iterator ) ; \
	}

static void add_nonshifts ( )
{
	int i ;
	arange ( i, 0x08, 0x0A ) ; 	// \b - \n
	add ( 0x0D ) ; 	// \r
	add ( 0x1B ) ; 	// ^ [
	add ( 0x20 ) ; 	// Space
	add ( 0x27 ) ; 	// '
	arange ( i, 0x2C, 0x39 ) ; 	//, - 9
	add ( 0x3B ) ; 	// ;
	arange ( i, 0x5B, 0x5D ) ; // [ - ]
	arange ( i, 0x60, 0x7A ) ; // ` - z
}

# undef add
# undef arange
# define add( character ) \
	if ( character <= 0x7E && _keyVals [ character ] ) \
		mcr_StringKey_set_shifted ( character, \
				_keyVals [ character ], 250000000 ) ;
# define arange( iterator, charMin, charMax ) \
	for ( iterator = charMin ; iterator <= charMax ; iterator ++ ) \
	{ \
		add ( iterator ) ; \
	}

static void add_shifts ( )
{
	int i ;
	arange ( i, 0x21, 0x26 ) ; 	// ! - &
	arange ( i, 0x28, 0x2B ) ; 	// ( - +
	add ( 0x3A ) ; 	// :
	arange ( i, 0x3C, 0x5A ) ; 	// < - Z
	add ( 0x5E ) ; 	// ^
	add ( 0x5F ) ; 	// _
	arange ( i, 0x7B, 0x7E ) ; 	// { - ~
}

static void get_strings_redirect ( mcr_SafeString * ssPt,
		mcr_Array * envMemPt )
{
	mcr_Array str = mcr_SafeString_get ( ssPt ) ;
	if ( MCR_STR_ISEMPTY ( & str ) )
		mcr_Array_free ( & str ) ;
	else
		mcr_Array_push ( envMemPt, & str ) ;
}

static void putenv_redirect ( mcr_Array * envStr, ... )
{
	if ( ! MCR_STR_ISEMPTY ( envStr ) )
		putenv ( envStr->array ) ;
}

static void arr_free_redirect ( mcr_Array * arrPt, ... )
{
	mcr_Array_free ( arrPt ) ;
}

static void add_keymods ( )
{
	// Left defaults
	const unsigned int types [ ] = {
		MCR_ALT, MCR_ALTGR, MCR_COMPOSE,
		MCR_CTRL, MCR_FN, MCR_FRONT,
		MCR_SHIFT, MCR_META
	} ;
	const int modKeys [ ] = {
		KEY_LEFTALT, KEY_RIGHTALT, KEY_COMPOSE,
		KEY_LEFTCTRL, KEY_FN, KEY_FRONT,
		KEY_LEFTSHIFT, KEY_LEFTMETA
	} ;
	// Right extras
	const unsigned int extraTypes [ ] = {
		MCR_OPTION, MCR_CTRL, MCR_SHIFT, MCR_META
	} ;
	const int extraModKeys [ ] = {
		KEY_OPTION, KEY_RIGHTCTRL, KEY_RIGHTSHIFT, KEY_RIGHTMETA
	} ;
	int i ;
	int len = ( int ) sizeof ( types ) / sizeof ( int ) ;
	for ( i = 0 ; i < len ; i ++ )
	{
		mcr_Mod_set_key ( types [ i ], modKeys [ i ] ) ;
	}
	len = ( int ) sizeof ( extraTypes ) / sizeof ( int ) ;
	for ( i = 0 ; i < ( int ) ( sizeof ( extraTypes ) / sizeof ( int ) ) ; i ++ )
	{
		mcr_Mod_add_key ( extraTypes [ i ], extraModKeys [ i ] ) ;
	}
}

static void add_keynames ( )
{
	const char * names [ ] =
	{
		"None", //0x00
		"LButton", //0x01
		"RButton", //0x02
		"Cancel", //0x03
		"MButton", //0x04
		"XButton1", //0x05
		"XButton2", //0x06
		"", //0x07
		"Back", //0x08
		"Tab", //0x09
		"", //0x0A
		"", //0x0B
		"Clear", //0x0C
		"Return", //0x0D
		"", //0x0E
		"", //0x0F
		"Shift", //0x10
		"Control", //0x11
		"Alt", //0x12 ! ! ! ! menu
		"Pause", //0x13
		"Capital", //0x14
		"Kana", //0x15 ! ! ! ! Hangul
		"", //0x16
		"Junja", //0x17
		"Final", //0x18
		"Hanja", //0x19 ! ! ! Kanji
		"", //0x1A
		"Escape", //0x1B
		"Convert", //0x1C
		"Nonconvert", //0x1D
		"Accept", //0x1E
		"ModeChangee", //0x1F
		"Space", //0x20
		"Prior", //0x21
		"Next", //0x22
		"End", //0x23
		"Home", //0x24
		"Left", //0x25
		"Up", //0x26
		"Right", //0x27
		"Down", //0x28
		"Select", //0x29
		"Print", //0x2A
		"Execute", //0x2B
		"Snapshot", //0x2C
		"Insert", //0x2D
		"Delete", //0x2E
		"Help", //0x2F
		"0", //0x30
		"1", //0x31
		"2", //0x32
		"3", //0x33
		"4", //0x34
		"5", //0x35
		"6", //0x36
		"7", //0x37
		"8", //0x38
		"9", //0x39
		"", //0x3A
		"", //0x3B
		"", //0x3C
		"", //0x3D
		"", //0x3E
		"", //0x3F
		"", //0x40
		"A", //0x41
		"B", //0x42
		"C", //0x43
		"D", //0x44
		"E", //0x45
		"F", //0x46
		"G", //0x47
		"H", //0x48
		"I", //0x49
		"J", //0x4A
		"K", //0x4B
		"L", //0x4C
		"M", //0x4D
		"N", //0x4E
		"O", //0x4F
		"P", //0x50
		"Q", //0x51
		"R", //0x52
		"S", //0x53
		"T", //0x54
		"U", //0x55
		"V", //0x56
		"W", //0x57
		"X", //0x58
		"Y", //0x59
		"Z", //0x5A
		"LWin", //0x5B
		"RWin", //0x5C
		"Apps", //0x5D
		"", //0x5E
		"Sleep", //0x5F
		"Numpad0", //0x60
		"Numpad1", //0x61
		"Numpad2", //0x62
		"Numpad3", //0x63
		"Numpad4", //0x64
		"Numpad5", //0x65
		"Numpad6", //0x66
		"Numpad7", //0x67
		"Numpad8", //0x68
		"Numpad9", //0x69
		"Multiply", //0x6A
		"Add", //0x6B
		"Separator", //0x6C
		"Subtract", //0x6D
		"Decimal", //0x6E
		"Divide", //0x6F
		"F1", //0x70
		"F2", //0x71
		"F3", //0x72
		"F4", //0x73
		"F5", //0x74
		"F6", //0x75
		"F7", //0x76
		"F8", //0x77
		"F9", //0x78
		"F10", //0x79
		"F11", //0x7A
		"F12", //0x7B
		"F13", //0x7C
		"F14", //0x7D
		"F15", //0x7E
		"F16", //0x7F
		"F17", //0x80
		"F18", //0x81
		"F19", //0x82
		"F20", //0x83
		"F21", //0x84
		"F22", //0x85
		"F23", //0x86
		"F24", //0x87
		"", //0x88
		"", //0x89
		"", //0x8A
		"", //0x8B
		"", //0x8C
		"", //0x8D
		"", //0x8E
		"", //0x8F
		"Numlock", //0x90
		"Scroll", //0x91
		"", //0x92-96
		"", //0x93
		"", //0x94
		"", //0x95
		"", //0x96
		"", //0x97-9F
		"", //0x98
		"", //0x99
		"", //0x9A
		"", //0x9B
		"", //0x9C
		"", //0x9D
		"", //0x9E
		"", //0x9F
		"LShift", //0xA0
		"RShift", //0xA1
		"LControl", //0xA2
		"RControl", //0xA3
		"LAlt", //0xA4 ! ! ! Menu
		"RAlt", //0xA5 ! ! ! Menu
		"Browser_Back", //0xA6
		"Browser_Forward", //0xA7
		"Browser_Refresh", //0xA8
		"Browser_Stop", //0xA9
		"Browser_Search", //0xAA
		"Browser_Favorites", //0xAB
		"Browser_Home", //0xAC
		"Volume_Mute", //0xAD
		"Volume_Down", //0xAE
		"Volume_Up", //0xAF
		"Media_Next_Track", //0xB0
		"Media_Prev_Track", //0xB1
		"Media_Stop", //0xB2
		"Media_Play_Pause", //0xB3
		"Launch_Mail", //0xB4
		"Launch_Media_Select", //0xB5
		"Launch_App1", //0xB6
		"Launch_App2", //0xB7
		"", //0xB8
		"", //0xB9
		" ; ", //0xBA US ' ; :'
		"Plus", //0xBB '+'
		"Comma", //0xBC ', '
		"Minus", //0xBD '-'
		"Period", //0xBE '.'
		"/", //0xBF US '/?'
		"`", //0xC0 US '`‾'
		"", //0xC1
		"", //0xC2
		"", //0xC3
		"", //0xC4
		"", //0xC5
		"", //0xC6
		"", //0xC7
		"", //0xC8
		"", //0xC9
		"", //0xCA
		"", //0xCB
		"", //0xCC
		"", //0xCD
		"", //0xCE
		"", //0xCF
		"", //0xD0
		"", //0xD1
		"", //0xD2
		"", //0xD3
		"", //0xD4
		"", //0xD5
		"", //0xD6
		"", //0xD7
		"", //0xD8
		"", //0xD9
		"", //0xDA
		" [ ", //0xDB US ' [ {'
		"\\", //0xDC US '¥|'
		" ] ", //0xDD US ' ] }'
		"'", //0xDE US 'single-quote/double-quote'
		"Oem_8", //0xDF
		"", //0xE0
		"", //0xE1
		"¥", //0xE2 ¥|
		"", //0xE3
		"", //0xE4
		"ProcessKey", //0xE5
		"", //0xE6
		"Packet", //0xE7
		"", //0xE8
		"", //0xE9-F5
		"", //0xEA
		"", //0xEB
		"", //0xEC
		"", //0xED
		"", //0xEE
		"", //0xEF
		"", //0xF0
		"", //0xF1
		"", //0xF2
		"", //0xF3
		"", //0xF4
		"", //0xF5
		"Attn", //0xF6
		"Crsel", //0xF7
		"Exsel", //0xF8
		"Ereof", //0xF9
		"Play", //0xFA
		"Zoom", //0xFB
		"", //0xFC
		"Pa1", //0xFD
		"Clear", //0xFE
		//"None", //0xFF
	} ;
	const int extraKeys [ ] =
	{
	} ;
	const char * extraNames [ ] [ 6 ] =
	{
	} ;
	const size_t extraLens [ ] =
	{
	} ;
	int count = sizeof ( names ) / sizeof ( char * ) ;
	int i ;
	for ( i = 0 ; i < count ; i ++ )
	{
		mcr_Key_set_name ( i, names [ i ] ) ;
	}
	count = sizeof ( extraKeys ) / sizeof ( int ) ;
	for ( i = 0 ; i < count ; i ++ )
	{
		mcr_Key_add_names ( extraKeys [ i ], extraNames [ i ],
				extraLens [ i ] ) ;
	}

}

static void add_echokeys ( )
{
	const int echoFlags [ ] =
	{
	} ;
	const int count = sizeof ( echoFlags ) / sizeof ( int ) ;
	for ( int i = 0 ; i < count ; i ++ )
	{
	}
}

static void add_echonames ( )
{
	const char * names [ ] =
	{
	} ;
	const char * addNames [ ] =
	{
	} ;
	int count = sizeof ( names ) / sizeof ( char * ) ;
	int i = 0, echoCode = 0 ;
	for ( ; i < count ; i ++ )
	{
		mcr_Echo_set_name ( echoCode, setname ) ;
		if ( addNames [ i ] [ 0 ] != '\0' )
		{
			mcr_Echo_add_name ( echoCode, setname ) ;
		}
	}
}
