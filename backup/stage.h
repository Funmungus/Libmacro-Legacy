/*! \file hotkey/stage.h
 * \brief Stages for \ref StagedHotkey
 * TODO : Decide whether to make activated private.
 * */

# ifndef MCR_STAGE_H
# define MCR_STAGE_H

# include "hotkey/def.h"
# include "hotkey/signalmod.h"
# include "hotkey/keymod.h"

namespace macro
{
	struct IStage ;
	typedef IStage * ( * stage_return_t ) ( ) ;
	// ! \brief getObject function for signal name, which becomes Stage<T>
	// ! \brief for named signal type.
	extern std::map<std::string, stage_return_t> StageMap ;
	// ! \brief Use HotMap to get a new Hot object, or NULL if none found.
	inline std::shared_ptr < IStage > stageFromSignal ( const std::string & signalName )
	{
		auto it = StageMap.find ( signalName ) ;
		if ( it == StageMap.end ( ) ) return NULL ;
		return std::shared_ptr < IStage > ( it->second ( ) ) ;
	}
	// TODO remove in favor of SDK structure.
	std::map<std::string, stage_return_t> defaultStageMap ( ) ;

	struct IStage
	{
	public :
		// ! \brief The mods to intercept for this to activate.
		int interceptMods ;
		// ! \brief If activated, and blocking, that activating signal will
		// ! be nullified.
		bool blocking ;
		// ! \brief Whether this stage is activated or not.
		bool activated ;
		/*!
		 * \brief For comparison of intercept signals this is an integral
		 * error margin that is acceptable to activate.
		 **/
		int measurementError ;

		virtual const ISignal * getIntercept ( ) const = 0 ;
		virtual void setIntercept ( ISignal * interceptPt ) = 0 ;

//		virtual int getInterceptMods ( ) const = 0 ;
//		virtual void setInterceptMods ( const int mods ) = 0 ;

//		virtual bool isBlocking ( ) const = 0 ;
//		virtual void enableBlocking ( const bool blocking ) = 0 ;

//		virtual bool isActivated ( ) const = 0 ;
//		virtual void enableActivated ( const bool activated ) = 0 ;

//		virtual int getMeasurementError ( ) const = 0 ;
//		virtual void setMeasurementError ( const int measurementError ) = 0 ;

		/*!
		 * \brief
		 * \return True if signal and modifier match. This stage's activated
		 * member will not be changed.
		 **/
		virtual bool isMe ( const ISignal * interceptPt, const int mods,
								bool & blockingReturn ) const = 0 ;
		/*!
		 * \brief
		 * \ref IStage#isMe ( const ISignal *, const int, bool & ) const, but this
		 * stage's activated member will be set to the same value.
		 * \param blockingReturn After this function, this parameter will be
		 * the same value as \ref IStage#isBlocking ( ) const.
		 **/
		virtual bool activateMe ( const ISignal * interceptPt,
										const int mods, bool & blockingReturn ) = 0 ;

		/*!
		 * \brief myType
		 * \return True if interceptPt is the same type as this stage.
		 */
		virtual bool myType ( const ISignal * interceptPt, bool & blockingReturn ) const = 0 ;
		/*!
		 * \brief activateUnspecific This stage's activated member will be
		 * the same as \ref IStage#myType ( const ISignal * ), and return value
		 * will be the same.
		 * \return Same value as
		 */
		virtual bool activateUnspecific ( const ISignal * interceptPt, bool & blockingReturn ) = 0 ;

		// Equality of most types is straight forward, but how to handle
		// the ISignal intercept pointer? We can at least determine same type
		// by ISignal#type ( ).
		MCR_EQUALITY_OPERATORS ( IStage )
//		inline bool operator == ( const IStage & rhs ) const ;
//		inline bool operator != ( const IStage & rhs ) const ;
//		inline bool operator < ( const IStage & rhs ) const ;
//		inline bool operator > ( const IStage & rhs ) const ;
//		inline bool operator <= ( const IStage & rhs ) const ;
//		inline bool operator >= ( const IStage & rhs ) const ;
	} ;

	// ! \brief Stages of activation, for StagedHotkey.
	template < typename T >
	struct Stage : public IStage
	{
	public :
		//
		// types
		//
		typedef bool ( & check_blocking_t ) ( const Stage<T> & obj,
										const ISignal * interceptPt,
										const int mods, bool & blockingReturn ) ;
		//
		// static
		//
		static check_blocking_t IsMeFunction ;

		//
		// members
		//
		// ! \brief The signal to intercept for this to activate.
		T intercept ;

		// ! \brief ctor
		Stage ( const ISignal * intercept = NULL, const int interceptMods = 0,
				const bool blocking = false, const bool activated = false,
				const int measurementError = 10 ) ;

		Stage ( const Stage<T> & copytron ) ;

		Stage<T> & operator= ( const Stage<T> & rhs ) ;


		const ISignal * getIntercept ( ) const ;
		void setIntercept ( ISignal * interceptPt ) ;

//		int getInterceptMods ( ) const ;
//		void setInterceptMods ( const int mods ) ;

//		bool isBlocking ( ) const ;
//		void enableBlocking ( const bool blocking ) ;

//		bool isActivated ( ) const ;
//		void enableActivated ( const bool activated ) ;

//		int getMeasurementError ( ) const ;
//		void setMeasurementError ( const int measurementError ) ;

		inline bool isMe ( const ISignal * interceptPt, const int mods,
								bool & blockingReturn ) const ;
		/*!
		 * \brief Default for \ref Stage::IsMeFunction.
		 * This is unspecific, but it will at least verify modifier, and then
		 * use \ref Stage#myType ( const ISignal *, bool & )
		 */
		static inline bool isMeCannotSpecify ( const Stage<T> & obj, const ISignal * interceptPt, const int mods,
										bool & blockingReturn ) ;

		inline bool activateMe ( const ISignal * interceptPt,
										const int mods, bool & blockingReturn ) ;

		inline bool myType ( const ISignal * interceptPt, bool & blockingReturn ) const ;
		inline bool activateUnspecific ( const ISignal * interceptPt, bool & blockingReturn ) ;
	} ;

	class StageContainer : public ISignal
	{
	public :
		typedef std::vector < std::shared_ptr < IStage > >
			stage_vector_t ;

		stage_vector_t stages ;

		StageContainer ( const stage_vector_t & set ) ;
		StageContainer ( const StageContainer & copytron ) ;
		StageContainer & operator = ( const StageContainer & copytron ) ;

		/*!
		 * \brief If \ref StageContainer#willActivate ( const ISignal *, const int )
		 * return true, and \ref StageContainer#send ( ).
		 * Otherwise \ref StageContainer#activateStages ( const ISignal *, const int ).
		 **/
		inline bool activateMe ( const ISignal * interceptPt, const int mods,
								bool & blockingReturn ) ;
		/*!
		 * \brief \return True if given signal and modifiers will activate
		 * the final stage.
		 **/
		inline bool willActivate ( const ISignal * interceptPt, const int mods,
								bool & blockingReturn ) ;
		/*!
		 * \brief This will traverse all activated stages and
		 * \ref IStage#activateMe ( const ISignal *, const int ) for both that
		 * stage, and the next stage, until the final stage.
		 **/
		inline void activateStages ( const ISignal * interceptPt, const int mods,
									bool & blockingReturn ) ;

		/* ISignal implementation */
		/*! \brief \ref StageContainer::Name */
		MCR_API std::string type ( ) const ;
		/*! \brief Deactivate all contained stages.
		 **/
		inline bool send ( ) ;
		// ! \brief \ref ISignal#operator ( ) ( )
		inline bool operator ( ) ( ) ;
		// ! \brief \ref ISignal#dispatch ( )
		inline bool	dispatch ( ) ;

		// Equality determined by equality of contained stages?
		// If stages are of same type, they may be false-equal.
		inline bool operator == ( const StageContainer & rhs ) const ;
		inline bool operator != ( const StageContainer & rhs ) const ;
		inline bool operator < ( const StageContainer & rhs ) const ;
		inline bool operator > ( const StageContainer & rhs ) const ;
		inline bool operator <= ( const StageContainer & rhs ) const ;
		inline bool operator >= ( const StageContainer & rhs ) const ;
	} ;

	// ! \brief Stages of activation, for StagedHotkey. Specified for base type.
	template <>
	struct Stage<ISignal> : public IStage
	{
	public :
		//
		// types
		//
		typedef bool ( & check_blocking_t ) ( const Stage<ISignal> & obj,
										const ISignal * interceptPt,
										const int mods, bool & blockingReturn ) ;
		//
		// static
		//
		static check_blocking_t IsMeFunction ;

		//
		// members
		//
		// ! \brief The signal to intercept for this to activate.
		ISignal * interceptPt ;
//		// ! \brief The mods to intercept for this to activate.
//		int interceptMods ;
//		// ! \brief If activated, and blocking, that activating signal will
//		// ! be nullified.
//		bool blocking ;
//		// ! \brief Whether this stage is activated or not.
//		bool activated ;
//		/*!
//		 * \brief For comparison of intercept signals this is an integral
//		 * error margin that is acceptable to activate.
//		 **/
//		int measurementError ;

		// ! \brief ctor
		Stage ( ISignal * interceptPt = NULL, const int interceptMods = 0,
				const bool blocking = false, const bool activated = false,
				const int measurementError = 10 ) ;

		Stage ( const Stage<ISignal> & copytron ) ;

		Stage<ISignal> & operator= ( const Stage<ISignal> & rhs ) ;

		const ISignal * getIntercept ( ) const ;
		void setIntercept ( ISignal * interceptPt ) ;

//		int getInterceptMods ( ) const ;
//		void setInterceptMods ( const int mods ) ;

//		bool isBlocking ( ) const ;
//		void enableBlocking ( const bool blocking ) ;

//		bool isActivated ( ) const ;
//		void enableActivated ( const bool activated ) ;

//		int getMeasurementError ( ) const ;
//		void setMeasurementError ( const int measurementError ) ;

		inline bool isMe ( const ISignal * interceptPt, const int mods,
								bool & blockingReturn ) const ;
		inline bool activateMe ( const ISignal * interceptPt,
										const int mods, bool & blockingReturn ) ;

		inline bool myType ( const ISignal * interceptPt, bool & blockingReturn ) const ;
		inline bool activateUnspecific ( const ISignal * interceptPt, bool & blockingReturn ) ;
	} ;

	template < class T >
	bool stageIsMe ( const Stage < T > & obj, const ISignal * interceptedPt,
					const int mods, bool & blockingReturn ) ;

}

# include "hotkey/inline/stage.h"

# endif
