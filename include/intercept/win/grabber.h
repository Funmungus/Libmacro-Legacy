/* include/intercept/win/grabber.h - Take blocking access of a hook.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file intercept/win/grabber.h
 * \brief Hooks are for intercepting events in Windows.
 * */

# ifndef MCR_WIN_GRABBER_H
# define MCR_WIN_GRABBER_H

# include "intercept/win/def.h"

typedef struct mcr_Grabber
{
	mtx_t lock ;
	HMODULE hModule ;
	HHOOK id ;
	HOOKPROC proc ;
	int type ;
	HANDLE hThread ;
	DWORD dwThread ;
} mcr_Grabber ;

MCR_API void mcr_Grabber_init ( mcr_Grabber * grabPt ) ;
MCR_API void mcr_Grabber_init_with ( mcr_Grabber * grabPt, int type,
		HOOKPROC proc ) ;
MCR_API void mcr_Grabber_free ( mcr_Grabber * grabPt ) ;
MCR_API int mcr_Grabber_is_enabled ( mcr_Grabber * grabPt ) ;
MCR_API void mcr_Grabber_enable ( mcr_Grabber * grabPt, int enable ) ;

# define MCR_GRABBER_ENABLED( grabPt ) \
	( grabPt->id != NULL )

# endif
