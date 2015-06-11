/* include/hotkey/hotkey.h - Hotkey module, trigger actions from signals.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file hotkey/hotkey.h
 * Hotkeys trigger actions.
 * \ref mcr_Hot
 * \ref mcr_Dispatch
 * */

# include "hotkey/dispatchstandard.h"
# include "hotkey/hotstandard.h"

MCR_API void mcr_hotkey_initialize ( ) ;

MCR_API void mcr_hotkey_cleanup ( ) ;
