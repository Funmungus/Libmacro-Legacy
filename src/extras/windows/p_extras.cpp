/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "mcr/extras/windows/p_extras.h"
#include "mcr/extras/extras.h"

namespace mcr
{
static void localAddKeys(Libmacro *ctx);

void Libmacro::initialize()
{
	localAddKeys(this);
}

void Libmacro::deinitialize()
{

}

static void add(Libmacro *ctx, int character, bool shiftFlag,
				const int keyVals[])
{
	if (character <= 0x7F)
		ctx->setCharacterKey(character, keyVals[character], 20, shiftFlag);
}

static void arange(Libmacro *ctx, int charMin, int charMax,
				   bool shiftFlag, const int keyVals[])
{
	for (int i = charMin; i <= charMax; i++) {
		add(ctx, i, shiftFlag, keyVals);
	}
}

static void localAddKeys(Libmacro *ctx)
{
	const int keyVals[] = {
		0,		/* Null character */
		0,		/*01    SOH             ^ A             Start of Header */
		0,		/*02    STX             ^ B             Start of Text */
		0,		/*03    ETX             ^ C             End of Text */
		0,		/*04    EOT             ^ D             End of Transmission */
		0,		/*05    ENQ             ^ E             Enquiry */
		0,		/*06    ACK             ^ F             Acknowledgment */
		0,		/*07    BEL             ^ G     \a      Bell */
		VK_BACK,	/*08    BS              ^ H     \b      Backspace [ d ][ e ] */
		VK_TAB,		/*09    HT              ^ I     \t      Horizontal Tab [ f ] */
		VK_RETURN,	/*0A    LF              ^ J     \n      Line feed */
		0,		/*0B    VT              ^ K     \v      Vertical Tab */
		0,		/*0C    FF              ^ L     \f      Form feed */
		0,		/*0D    CR              ^ M     \r      Carriage return [ g ] */
		0,		/*0E    SO              ^ N             Shift Out */
		0,		/*0F    SI              ^ O             Shift In */
		0,		/*10    DLE             ^ P             Data Link Escape */
		0,		/*11    DC1             ^ Q             Device Control 1 ( oft. XON ) */
		0,		/*12    DC2             ^ R             Device Control 2 */
		0,		/*13    DC3             ^ S             Device Control 3 ( oft. XOFF ) */
		0,		/*14    DC4             ^ T             Device Control 4 */
		0,		/*15    NAK             ^ U             Negative Acknowledgement */
		0,		/*16    SYN             ^ V             Synchronous idle */
		0,		/*17    ETB             ^ W             End of Transmission Block */
		0,		/*18    CAN             ^ X             Cancel */
		0,		/*19    EM              ^ Y             End of Medium */
		0,		/*1A    SUB             ^ Z             Substitute */
		VK_ESCAPE,	/*1B    ESC             ^ [     \e [ h ]        Escape [ i ] */
		0,		/*1C    FS              ^\              File Separator */
		0,		/*1D    GS              ^ ]             Group Separator */
		0,		/*1E    RS              ^^ [ j ]                Record Separator */
		0,		/*1F    US              ^ _             Unit Separator */
		' ',		/*20    Space */
		'1',		/*21    ! */
		VK_OEM_7,	/*22    " */
		'3',		/*23    # */
		'4',		/*24    $ */
		'5',		/*25    % */
		'7',		/*26    & */
		VK_OEM_7,	/*27    ' */
		'9',		/*28    ( */
		'0',		/*29    ) */
		'8',		/*2A    * */
		VK_OEM_PLUS,	/*2B    + */
		VK_OEM_COMMA,	/*2C    , */
		VK_OEM_MINUS,	/*2D    - */
		VK_OEM_PERIOD,	/*2E    . */
		VK_OEM_2,	/*2F    / */
		'0',		/*30    0 */
		'1',		/*31    1 */
		'2',		/*32    2 */
		'3',		/*33    3 */
		'4',		/*34    4 */
		'5',		/*35    5 */
		'6',		/*36    6 */
		'7',		/*37    7 */
		'8',		/*38    8 */
		'9',		/*39    9 */
		VK_OEM_1,	/*3A    : */
		VK_OEM_1,	/*3B    ; */
		VK_OEM_COMMA,	/*3C    < */
		VK_OEM_PLUS,	/*3D    = */
		VK_OEM_PERIOD,	/*3E    > */
		VK_OEM_2,	/*3F    ? */
		'2',		/*40    @ */
		'A',		/*41    A */
		'B',		/*42    B */
		'C',		/*43    C */
		'D',		/*44    D */
		'E',		/*45    E */
		'F',		/*46    F */
		'G',		/*47    G */
		'H',		/*48    H */
		'I',		/*49    I */
		'J',		/*4A    J */
		'K',		/*4B    K */
		'L',		/*4C    L */
		'M',		/*4D    M */
		'N',		/*4E    N */
		'O',		/*4F    O */
		'P',		/*50    P */
		'Q',		/*51    Q */
		'R',		/*52    R */
		'S',		/*53    S */
		'T',		/*54    T */
		'U',		/*55    U */
		'V',		/*56    V */
		'W',		/*57    W */
		'X',		/*58    X */
		'Y',		/*59    Y */
		'Z',		/*5A    Z */
		VK_OEM_4,	/*5B    [ */
		VK_OEM_5,	/*5C */
		VK_OEM_6,	/*5D    ] */
		'6',		/*5E    ^ */
		VK_OEM_MINUS,	/*5F    _ */
		VK_OEM_3,	/*60    ` */
		'A',		/*61    a */
		'B',		/*62    b */
		'C',		/*63    c */
		'D',		/*64    d */
		'E',		/*65    e */
		'F',		/*66    f */
		'G',		/*67    g */
		'H',		/*68    h */
		'I',		/*69    i */
		'J',		/*6A    j */
		'K',		/*6B    k */
		'L',		/*6C    l */
		'M',		/*6D    m */
		'N',		/*6E    n */
		'O',		/*6F    o */
		'P',		/*70    p */
		'Q',		/*71    q */
		'R',		/*72    r */
		'S',		/*73    s */
		'T',		/*74    t */
		'U',		/*75    u */
		'V',		/*76    v */
		'W',		/*77    w */
		'X',		/*78    x */
		'Y',		/*79    y */
		'Z',		/*7A    z */
		VK_OEM_4,	/*7B    { */
		VK_OEM_5,	/*7C    | */
		VK_OEM_6,	/*7D    } */
		VK_OEM_3,	/*7E    ~ */
	};
	/* \b - Space */
	arange(ctx, 0x00, 0x20, false, keyVals);
	/* ! - & */
	arange(ctx, 0x21, 0x26, true, keyVals);
	/* ' */
	add(ctx, 0x27, false, keyVals);
	/* ( - + */
	arange(ctx, 0x28, 0x2B, true, keyVals);
	/*, - 9 */
	arange(ctx, 0x2C, 0x39, false, keyVals);
	/* : */
	add(ctx, 0x3A, true, keyVals);
	/*; */
	add(ctx, 0x3B, false, keyVals);
	/* < - Z */
	arange(ctx, 0x3C, 0x5A, true, keyVals);
	/* [ - ] */
	arange(ctx, 0x5B, 0x5D, false, keyVals);
	/* ^ */
	add(ctx, 0x5E, true, keyVals);
	/* _ */
	add(ctx, 0x5F, true, keyVals);
	/* ` - z */
	arange(ctx, 0x60, 0x7A, false, keyVals);
	/* { - ~ */
	arange(ctx, 0x7B, 0x7E, true, keyVals);
}
}
