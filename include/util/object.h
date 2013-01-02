




	/*! \brief A base class for templates which allows a template to
	 * also be object-oriented.
	 *
	 * A pointer may be given for this base class, and dynamically casted
	 * into a template specification.  A specified template must still be
	 * defined as a class, or given a typedef ( as is done here for
	 * subtypes of ICall ).
	 */
	class TemplateBase
	{
	public :
		virtual ~TemplateBase () { }
	};

	/*! \brief ICall is an interface template.  It has one pure virtual
	 * function, call, with a variable set of template types.
	 *
	 * Classes which inherit from ICall must implement the following function<br>
	 * T call ( Params ) ; <br>
	 * Where T is the first class template parameter, and Params is a variable list.
	 * <br>
	 * ICall <void> requires the function void call () ;
	 * <br>
	 * ICall <void, int> requires the function void call ( int ) ;
	 */
	template < typename T, typename ...Params >
	class ICall : virtual public TemplateBase
	{
	public :
		virtual ~ICall ( ) { }
		virtual T call ( Params... ) = 0 ;
	};



