
# include "signal/keystring.h"

namespace funlibrary
{
	const int KeyString::_keyVals [ ] = {
		0, //Null character
		0, //01	SOH	␁	^A		Start of Header
		0, //02	STX	␂	^B		Start of Text
		0, //03	ETX	␃	^C		End of Text
		0, //04	EOT	␄	^D		End of Transmission
		0, //05	ENQ	␅	^E		Enquiry
		0, //06	ACK	␆	^F		Acknowledgment
		0, //07	BEL	␇	^G	\a	Bell
		KEY_BACKSPACE, //08	BS	␈	^H	\b	Backspace[d][e]
		KEY_TAB, //09	HT	␉	^I	\t	Horizontal Tab[f]
		KEY_LINEFEED, //0A	LF	␊	^J	\n	Line feed
		0, //0B	VT	␋	^K	\v	Vertical Tab
		0, //0C	FF	␌	^L	\f	Form feed
		KEY_ENTER, //0D	CR	␍	^M	\r	Carriage return[g]
		0, //0E	SO	␎	^N		Shift Out
		0, //0F	SI	␏	^O		Shift In
		0, //10	DLE	␐	^P		Data Link Escape
		0, //11	DC1	␑	^Q		Device Control 1 (oft. XON)
		0, //12	DC2	␒	^R		Device Control 2
		0, //13	DC3	␓	^S		Device Control 3 (oft. XOFF)
		0, //14	DC4	␔	^T		Device Control 4
		0, //15	NAK	␕	^U		Negative Acknowledgement
		0, //16	SYN	␖	^V		Synchronous idle
		0, //17	ETB	␗	^W		End of Transmission Block
		0, //18	CAN	␘	^X		Cancel
		0, //19	EM	␙	^Y		End of Medium
		0, //1A	SUB	␚	^Z		Substitute
		KEY_ESC, //1B	ESC	␛	^[	\e[h]	Escape[i]
		0, //1C	FS	␜	^\		File Separator
		0, //1D	GS	␝	^]		Group Separator
		0, //1E	RS	␞	^^[j]		Record Separator
		0, //1F	US	␟	^_		Unit Separator
		KEY_SPACE, //20	␠ Space
		KEY_1, //21	!
		KEY_APOSTROPHE, //22	"
		KEY_3, //23	#
		KEY_4, //24	$
		KEY_5, //25	%
		KEY_7, //26	&
		KEY_APOSTROPHE, //27	'
		KEY_9, //28	(
		KEY_0, //29	)
		KEY_8, //2A	*
		KEY_EQUAL, //2B	+
		KEY_COMMA, //2C	,
		KEY_MINUS, //2D	-
		KEY_DOT, //2E	.
		KEY_SLASH, //2F	/
		KEY_0, //30	0
		KEY_1, //31	1
		KEY_2, //32	2
		KEY_3, //33	3
		KEY_4, //34	4
		KEY_5, //35	5
		KEY_6, //36	6
		KEY_7, //37	7
		KEY_8, //38	8
		KEY_9, //39	9
		KEY_SEMICOLON, //3A	:
		KEY_SEMICOLON, //3B	;
		KEY_COMMA, //3C	<
		KEY_EQUAL, //3D	=
		KEY_DOT, //3E	>
		KEY_SLASH, //3F	?
		KEY_2, //40	@
		KEY_A, //41	A
		KEY_B, //42	B
		KEY_C, //43	C
		KEY_D, //44	D
		KEY_E, //45	E
		KEY_F, //46	F
		KEY_G, //47	G
		KEY_H, //48	H
		KEY_I, //49	I
		KEY_J, //4A	J
		KEY_K, //4B	K
		KEY_L, //4C	L
		KEY_M, //4D	M
		KEY_N, //4E	N
		KEY_O, //4F	O
		KEY_P, //50	P
		KEY_Q, //51	Q
		KEY_R, //52	R
		KEY_S, //53	S
		KEY_T, //54	T
		KEY_U, //55	U
		KEY_V, //56	V
		KEY_W, //57	W
		KEY_X, //58	X
		KEY_Y, //59	Y
		KEY_Z, //5A	Z
		KEY_LEFTBRACE, //5B	[
		KEY_BACKSLASH, //5C	'\'
		KEY_RIGHTBRACE, //5D	]
		KEY_6, //5E	^
		KEY_MINUS, //5F	_
		KEY_GRAVE, //60	`
		KEY_A, //61	a
		KEY_B, //62	b
		KEY_C, //63	c
		KEY_D, //64	d
		KEY_E, //65	e
		KEY_F, //66	f
		KEY_G, //67	g
		KEY_H, //68	h
		KEY_I, //69	i
		KEY_J, //6A	j
		KEY_K, //6B	k
		KEY_L, //6C	l
		KEY_M, //6D	m
		KEY_N, //6E	n
		KEY_O, //6F	o
		KEY_P, //70	p
		KEY_Q, //71	q
		KEY_R, //72	r
		KEY_S, //73	s
		KEY_T, //74	t
		KEY_U, //75	u
		KEY_V, //76	v
		KEY_W, //77	w
		KEY_X, //78	x
		KEY_Y, //79	y
		KEY_Z, //7A	z
		KEY_LEFTBRACE, //7B	{
		KEY_BACKSLASH, //7C	|
		KEY_RIGHTBRACE, //7D	}
		KEY_MINUS //7E	~
	} ;

	const bool KeyString::_needShift [ ] = {
		false, //Null character
		false, //01	SOH	␁	^A		Start of Header
		false, //02	STX	␂	^B		Start of Text
		false, //03	ETX	␃	^C		End of Text
		false, //04	EOT	␄	^D		End of Transmission
		false, //05	ENQ	␅	^E		Enquiry
		false, //06	ACK	␆	^F		Acknowledgment
		false, //07	BEL	␇	^G	\a	Bell
		false, //08	BS	␈	^H	\b	Backspace[d][e]
		false, //09	HT	␉	^I	\t	Horizontal Tab[f]
		false, //0A	LF	␊	^J	\n	Line feed
		false, //0B	VT	␋	^K	\v	Vertical Tab
		false, //0C	FF	␌	^L	\f	Form feed
		false, //0D	CR	␍	^M	\r	Carriage return[g]
		false, //0E	SO	␎	^N		Shift Out
		false, //0F	SI	␏	^O		Shift In
		false, //10	DLE	␐	^P		Data Link Escape
		false, //11	DC1	␑	^Q		Device Control 1 (oft. XON)
		false, //12	DC2	␒	^R		Device Control 2
		false, //13	DC3	␓	^S		Device Control 3 (oft. XOFF)
		false, //14	DC4	␔	^T		Device Control 4
		false, //15	NAK	␕	^U		Negative Acknowledgement
		false, //16	SYN	␖	^V		Synchronous idle
		false, //17	ETB	␗	^W		End of Transmission Block
		false, //18	CAN	␘	^X		Cancel
		false, //19	EM	␙	^Y		End of Medium
		false, //1A	SUB	␚	^Z		Substitute
		false, //1B	ESC	␛	^[	\e[h]	Escape[i]
		false, //1C	FS	␜	^\		File Separator
		false, //1D	GS	␝	^]		Group Separator
		false, //1E	RS	␞	^^[j]		Record Separator
		false, //1F	US	␟	^_		Unit Separator
		false, //20	␠ Space
		true, //21	!
		true, //22	"
		true, //23	#
		true, //24	$
		true, //25	%
		true, //26	&
		false, //27	'
		true, //28	(
		true, //29	)
		true, //2A	*
		true, //2B	+
		false, //2C	,
		false, //2D	-
		false, //2E	.
		false, //2F	/
		false, //30	0
		false, //31	1
		false, //32	2
		false, //33	3
		false, //34	4
		false, //35	5
		false, //36	6
		false, //37	7
		false, //38	8
		false, //39	9
		true, //3A	:
		false, //3B	;
		true, //3C	<
		true, //3D	=
		true, //3E	>
		true, //3F	?
		true, //40	@
		true, //41	A
		true, //42	B
		true, //43	C
		true, //44	D
		true, //45	E
		true, //46	F
		true, //47	G
		true, //48	H
		true, //49	I
		true, //4A	J
		true, //4B	K
		true, //4C	L
		true, //4D	M
		true, //4E	N
		true, //4F	O
		true, //50	P
		true, //51	Q
		true, //52	R
		true, //53	S
		true, //54	T
		true, //55	U
		true, //56	V
		true, //57	W
		true, //58	X
		true, //59	Y
		true, //5A	Z
		false, //5B	[
		false, //5C	'\'
		false, //5D	]
		true, //5E	^
		true, //5F	_
		false, //60	`
		false, //61	a
		false, //62	b
		false, //63	c
		false, //64	d
		false, //65	e
		false, //66	f
		false, //67	g
		false, //68	h
		false, //69	i
		false, //6A	j
		false, //6B	k
		false, //6C	l
		false, //6D	m
		false, //6E	n
		false, //6F	o
		false, //70	p
		false, //71	q
		false, //72	r
		false, //73	s
		false, //74	t
		false, //75	u
		false, //76	v
		false, //77	w
		false, //78	x
		false, //79	y
		false, //7A	z
		true, //7B	{
		true, //7C	|
		true, //7D	}
		true //7E	~
	} ;
}
