
# include "signal/scriptsignal.h"

namespace macro
{
	const std::string ScriptSignal::name = "scriptsignal" ;
	bool ( * ScriptSignal::dispatcher ) ( ScriptSignal & ) ;

	std::string ScriptSignal::type ( ) const
	{
		return name ;
	}
}
