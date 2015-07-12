/* include/util/lnx/priv.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# ifndef MCR_LNX_PRIV_H
# define MCR_LNX_PRIV_H

# include "util/lnx/def.h"

MCR_API void mcr_setuid ( int uid ) ;

MCR_API int mcr_deactivate_root ( ) ;
MCR_API int mcr_activate_root ( ) ;
MCR_API int mcr_activate_user ( ) ;

# endif
