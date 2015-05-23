/* include/nativeless/macro.h - Include all macrolibrary modules.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# ifdef __cplusplus
extern "C" {
# endif

# include "intercept/nativeless/intercept.h"
# ifdef MCR_EXTRAS
# include "extras/nativeless/extras.h"
# endif

# ifdef __cplusplus
}
# endif
