
# include "hotkey/stage.h"

namespace macro
{
	Stage<ISignal>::check_blocking_t Stage<ISignal>::IsMeFunction =
			stageIsMe<ISignal> ;

	StageContainer::StageContainer ( const stage_vector_t & set )
		: stages ( set )
	{}

	StageContainer::StageContainer ( const StageContainer & copytron )
		: stages ( copytron.stages )
	{}

	StageContainer & StageContainer::operator = ( const StageContainer & copytron )
	{
		if ( this != & copytron )
		{
			stages = copytron.stages ;
		}
		return * this ;
	}

	/* ISignal implementation */
	std::string StageContainer::type ( ) const
	{
		return Signal < StageContainer >::Name ;
	}

	Stage<ISignal>::Stage ( ISignal * interceptPt, const int interceptMods,
			const bool blocking, const bool activated,
			const int measurementError )
		: interceptPt ( interceptPt ), interceptMods ( interceptMods ),
		blocking ( blocking ), activated ( activated ),
		measurementError ( measurementError )
	{}

	Stage<ISignal>::Stage ( const Stage<ISignal> & copytron )
		: interceptPt ( copytron.interceptPt ),
		interceptMods ( copytron.interceptMods ),
		blocking ( copytron.blocking ), activated ( copytron.activated ),
		measurementError ( copytron.measurementError )
	{}

	Stage<ISignal> & Stage<ISignal>::operator= ( const Stage<ISignal> & rhs )
	{
		if ( this != & copytron )
		{
			interceptPt = copytron.interceptPt ;
			interceptMods = copytron.interceptMods ;
			blocking = copytron.blocking ;
			activated = copytron.activated ;
		}
		return * this ;
	}

	const ISignal * Stage<ISignal>::getIntercept ( ) const
	{
		return interceptPt ;
	}
	void Stage<ISignal>::setIntercept ( ISignal * interceptPt )
	{
		this->interceptPt = interceptPt ;
	}

//	int Stage<ISignal>::getInterceptMods ( ) const
//	{
//		return interceptMods ;
//	}
//	void Stage<ISignal>::setInterceptMods ( const int mods )
//	{
//		interceptMods = mods ;
//	}

//	bool Stage<ISignal>::isBlocking ( ) const
//	{
//		return blocking ;
//	}
//	void Stage<ISignal>::enableBlocking ( const bool blocking )
//	{
//		this->blocking = blocking ;
//	}

//	bool Stage<ISignal>::isActivated ( ) const
//	{
//		return activated ;
//	}
//	void Stage<ISignal>::enableActivated ( const bool activated )
//	{
//		this->activated = activated ;
//	}

//	int Stage<ISignal>::getMeasurementError ( ) const
//	{
//		return measurementError ;
//	}
//	void Stage<ISignal>::setMeasurementError ( const int measurementError )
//	{
//		this->measurementError = measurementError ;
//	}

	std::map<std::string, stage_return_t> defaultStageMap ( )
	{
		std::string names [ ] =
		{
			Signal<ISignal>::Name,
			Signal<Alarm>::Name, Signal<HIDEcho>::Name,
			Signal<Key>::Name, Signal<MoveCursor>::Name,
			Signal<NoOp>::Name, Signal<Scroll>::Name,
			Signal<KeyMod>::Name, Signal<SignalMod>::Name
		} ;
		std::map<std::string, stage_return_t> ret ;
		stage_return_t fncs [ ] =
		{
			& getObject<IStage, Stage<ISignal>>,
			& getObject<IStage, Stage<Alarm>>, & getObject<IStage, Stage<HIDEcho>>,
			& getObject<IStage, Stage<Key>>, & getObject<IStage, Stage<MoveCursor>>,
			& getObject<IStage, Stage<NoOp>>, 	& getObject<IStage, Stage<Scroll>>,
			& getObject<IStage, Stage<KeyMod>>, & getObject<IStage, Stage<SignalMod>>
		} ;
		unsigned int len = sizeof names / sizeof ( std::string ) ;
		std::string name, origin ;
		for ( unsigned int i = 0 ; i < len ; i++ )
		{
			origin = name = names [ i ] ;
			std::transform ( name.begin ( ), name.end ( ), name.begin ( ), toupper ) ;
			ret.erase ( name ) ;
			ret.insert ( std::make_pair ( name, fncs [ i ] ) ) ;
			std::transform ( name.begin ( ), name.end ( ), name.begin ( ), tolower ) ;
			ret.erase ( name ) ;
			ret.insert ( std::make_pair ( name, fncs [ i ] ) ) ;
			ret.erase ( origin ) ;
			ret.insert ( std::make_pair ( origin, fncs [ i ] ) ) ;
		}
		return ret ;
	}

	std::map<std::string, stage_return_t> StageMap = defaultStageMap ( ) ;
}
