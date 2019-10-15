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

#include "mcr/extras/linux/p_extras.h"
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

static inline void add(Libmacro *ctx, int character, bool shiftFlag,
					   const int keyVals[])
{
	if (character <= 0x7E)
		ctx->setCharacterKey(character, keyVals[character], 20, shiftFlag);
}

static inline void arange(Libmacro *ctx, int charMin, int charMax,
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
		KEY_BACKSPACE,	/*08    BS              ^ H     \b      Backspace [ d ][ e ] */
		KEY_TAB,	/*09    HT              ^ I     \t      Horizontal Tab [ f ] */
		KEY_ENTER,	/*0A    LF              ^ J     \n      Line feed */
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
		KEY_ESC,	/*1B    ESC             ^ [     \e [ h ]        Escape [ i ] */
		0,		/*1C    FS              ^\              File Separator */
		0,		/*1D    GS              ^ ]             Group Separator */
		0,		/*1E    RS              ^^ [ j ]                Record Separator */
		0,		/*1F    US              ^ _             Unit Separator */
		KEY_SPACE,	/*20    Space */
		KEY_1,		/*21    ! */
		KEY_APOSTROPHE,	/*22    " */
		KEY_3,		/*23    # */
		KEY_4,		/*24    $ */
		KEY_5,		/*25    % */
		KEY_7,		/*26    & */
		KEY_APOSTROPHE,	/*27    ' */
		KEY_9,		/*28    ( */
		KEY_0,		/*29    ) */
		KEY_8,		/*2A    * */
		KEY_EQUAL,	/*2B    + */
		KEY_COMMA,	/*2C    , */
		KEY_MINUS,	/*2D    - */
		KEY_DOT,	/*2E    . */
		KEY_SLASH,	/*2F    / */
		KEY_0,		/*30    0 */
		KEY_1,		/*31    1 */
		KEY_2,		/*32    2 */
		KEY_3,		/*33    3 */
		KEY_4,		/*34    4 */
		KEY_5,		/*35    5 */
		KEY_6,		/*36    6 */
		KEY_7,		/*37    7 */
		KEY_8,		/*38    8 */
		KEY_9,		/*39    9 */
		KEY_SEMICOLON,	/*3A    : */
		KEY_SEMICOLON,	/*3B    ; */
		KEY_COMMA,	/*3C    < */
		KEY_EQUAL,	/*3D    = */
		KEY_DOT,	/*3E    > */
		KEY_SLASH,	/*3F    ? */
		KEY_2,		/*40    @ */
		KEY_A,		/*41    A */
		KEY_B,		/*42    B */
		KEY_C,		/*43    C */
		KEY_D,		/*44    D */
		KEY_E,		/*45    E */
		KEY_F,		/*46    F */
		KEY_G,		/*47    G */
		KEY_H,		/*48    H */
		KEY_I,		/*49    I */
		KEY_J,		/*4A    J */
		KEY_K,		/*4B    K */
		KEY_L,		/*4C    L */
		KEY_M,		/*4D    M */
		KEY_N,		/*4E    N */
		KEY_O,		/*4F    O */
		KEY_P,		/*50    P */
		KEY_Q,		/*51    Q */
		KEY_R,		/*52    R */
		KEY_S,		/*53    S */
		KEY_T,		/*54    T */
		KEY_U,		/*55    U */
		KEY_V,		/*56    V */
		KEY_W,		/*57    W */
		KEY_X,		/*58    X */
		KEY_Y,		/*59    Y */
		KEY_Z,		/*5A    Z */
		KEY_LEFTBRACE,	/*5B    [ */
		KEY_BACKSLASH,	/*5C    '\' */
		KEY_RIGHTBRACE,	/*5D    ] */
		KEY_6,		/*5E    ^ */
		KEY_MINUS,	/*5F    _ */
		KEY_GRAVE,	/*60    ` */
		KEY_A,		/*61    a */
		KEY_B,		/*62    b */
		KEY_C,		/*63    c */
		KEY_D,		/*64    d */
		KEY_E,		/*65    e */
		KEY_F,		/*66    f */
		KEY_G,		/*67    g */
		KEY_H,		/*68    h */
		KEY_I,		/*69    i */
		KEY_J,		/*6A    j */
		KEY_K,		/*6B    k */
		KEY_L,		/*6C    l */
		KEY_M,		/*6D    m */
		KEY_N,		/*6E    n */
		KEY_O,		/*6F    o */
		KEY_P,		/*70    p */
		KEY_Q,		/*71    q */
		KEY_R,		/*72    r */
		KEY_S,		/*73    s */
		KEY_T,		/*74    t */
		KEY_U,		/*75    u */
		KEY_V,		/*76    v */
		KEY_W,		/*77    w */
		KEY_X,		/*78    x */
		KEY_Y,		/*79    y */
		KEY_Z,		/*7A    z */
		KEY_LEFTBRACE,	/*7B    { */
		KEY_BACKSLASH,	/*7C    | */
		KEY_RIGHTBRACE,	/*7D    } */
		KEY_MINUS	/*7E    ~ */
	};

	/* \b - \n */
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
