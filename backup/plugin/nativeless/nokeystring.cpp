
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
		VK_BACK, //08	BS	␈	^H	\b	Backspace [ d ] [ e ]
		VK_TAB, //09	HT	␉	^I	\t	Horizontal Tab [ f ]
		VK_RETURN, //0A	LF	␊	^J	\n	Line feed
		0, //0B	VT	␋	^K	\v	Vertical Tab
		0, //0C	FF	␌	^L	\f	Form feed
		0, //0D	CR	␍	^M	\r	Carriage return [ g ]
		0, //0E	SO	␎	^N		Shift Out
		0, //0F	SI	␏	^O		Shift In
		0, //10	DLE	␐	^P		Data Link Escape
		0, //11	DC1	␑	^Q		Device Control 1 ( oft. XON )
		0, //12	DC2	␒	^R		Device Control 2
		0, //13	DC3	␓	^S		Device Control 3 ( oft. XOFF )
		0, //14	DC4	␔	^T		Device Control 4
		0, //15	NAK	␕	^U		Negative Acknowledgement
		0, //16	SYN	␖	^V		Synchronous idle
		0, //17	ETB	␗	^W		End of Transmission Block
		0, //18	CAN	␘	^X		Cancel
		0, //19	EM	␙	^Y		End of Medium
		0, //1A	SUB	␚	^Z		Substitute
		VK_ESCAPE, //1B	ESC	␛	^ [ 	\e [ h ] 	Escape [ i ]
		0, //1C	FS	␜	^\		File Separator
		0, //1D	GS	␝	^ ] 		Group Separator
		0, //1E	RS	␞	^^ [ j ] 		Record Separator
		0, //1F	US	␟	^_		Unit Separator
		VK_SPACE, //20	␠ Space
		'1', //21	!
		VK_OEM_7, //22	"
		'3', //23	#
		'4', //24	$
		'5', //25	%
		'7', //26	&
		VK_OEM_7, //27	'
		'9', //28	(
		'0', //29	)
		'8', //2A	 *
		VK_OEM_PLUS, //2B	+
		VK_OEM_COMMA, //2C	,
		VK_OEM_MINUS, //2D	-
		VK_OEM_PERIOD, //2E	.
		VK_OEM_2, //2F	/
		'0', //30	0
		'1', //31	1
		'2', //32	2
		'3', //33	3
		'4', //34	4
		'5', //35	5
		'6', //36	6
		'7', //37	7
		'8', //38	8
		'9', //39	9
		VK_OEM_1, //3A	:
		VK_OEM_1, //3B	;
		VK_OEM_COMMA, //3C	<
		VK_OEM_PLUS, //3D	=
		VK_OEM_PERIOD, //3E	>
		VK_OEM_2, //3F	?
		'2', //40	@
		'A', //41	A
		'B', //42	B
		'C', //43	C
		'D', //44	D
		'E', //45	E
		'F', //46	F
		'G', //47	G
		'H', //48	H
		'I', //49	I
		'J', //4A	J
		'K', //4B	K
		'L', //4C	L
		'M', //4D	M
		'N', //4E	N
		'O', //4F	O
		'P', //50	P
		'Q', //51	Q
		'R', //52	R
		'S', //53	S
		'T', //54	T
		'U', //55	U
		'V', //56	V
		'W', //57	W
		'X', //58	X
		'Y', //59	Y
		'Z', //5A	Z
		VK_OEM_4, //5B	[
		VK_OEM_5, //5C	'\'
		VK_OEM_6, //5D	]
		'6', //5E	^
		VK_OEM_MINUS, //5F	_
		VK_OEM_3, //60	`
		'A', //61	a
		'B', //62	b
		'C', //63	c
		'D', //64	d
		'E', //65	e
		'F', //66	f
		'G', //67	g
		'H', //68	h
		'I', //69	i
		'J', //6A	j
		'K', //6B	k
		'L', //6C	l
		'M', //6D	m
		'N', //6E	n
		'O', //6F	o
		'P', //70	p
		'Q', //71	q
		'R', //72	r
		'S', //73	s
		'T', //74	t
		'U', //75	u
		'V', //76	v
		'W', //77	w
		'X', //78	x
		'Y', //79	y
		'Z', //7A	z
		VK_OEM_4, //7B	{
		VK_OEM_5, //7C	|
		VK_OEM_6, //7D	}
		VK_OEM_3, //7E	~
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
		false, //08	BS	␈	^H	\b	Backspace [ d ] [ e ]
		false, //09	HT	␉	^I	\t	Horizontal Tab [ f ]
		false, //0A	LF	␊	^J	\n	Line feed
		false, //0B	VT	␋	^K	\v	Vertical Tab
		false, //0C	FF	␌	^L	\f	Form feed
		false, //0D	CR	␍	^M	\r	Carriage return [ g ]
		false, //0E	SO	␎	^N		Shift Out
		false, //0F	SI	␏	^O		Shift In
		false, //10	DLE	␐	^P		Data Link Escape
		false, //11	DC1	␑	^Q		Device Control 1 ( oft. XON )
		false, //12	DC2	␒	^R		Device Control 2
		false, //13	DC3	␓	^S		Device Control 3 ( oft. XOFF )
		false, //14	DC4	␔	^T		Device Control 4
		false, //15	NAK	␕	^U		Negative Acknowledgement
		false, //16	SYN	␖	^V		Synchronous idle
		false, //17	ETB	␗	^W		End of Transmission Block
		false, //18	CAN	␘	^X		Cancel
		false, //19	EM	␙	^Y		End of Medium
		false, //1A	SUB	␚	^Z		Substitute
		false, //1B	ESC	␛	^ [ 	\e [ h ] 	Escape [ i ]
		false, //1C	FS	␜	^\		File Separator
		false, //1D	GS	␝	^ ] 		Group Separator
		false, //1E	RS	␞	^^ [ j ] 		Record Separator
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
		true, //2A	 *
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
