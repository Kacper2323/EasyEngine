#pragma once
#include <iostream>

/*
these flags need to be declared in the main file and initialized with 0
*/
extern bool suppressTraceFlag;
extern bool suppressWarningFlag;


namespace MSG
{
	/*
	Stop printing Trace messages after the function has been called.
	*/
	inline void suppressTrace()
	{
		suppressTraceFlag = 1;
	}

	/*
	Resume printing Trace messages after the function has been called.
	*/
	inline void releaseTrace()
	{
		suppressTraceFlag = 0;
	}
	
	/*
	Stop printing Warning messages after the function has been called.
	*/
	inline void suppressWarnings()
	{
		suppressWarningFlag = 1;
	}

	/*
	Resume printing Warning messages after the function has been called.
	*/
	inline void releaseWarnings()
	{
		suppressWarningFlag = 0;
	}


	/*
	Print out an error message to the console.
	Cannot be suppressed.
	\param any 'coutable' types in order in which they are meant to be printed
	*/
	template<typename ...TArgs>
	void ERROR(TArgs... message)
	{
		std::cout << "\x1b[31m";	//sets a red color
		(std::cout << ... << message);
		std::cout << "\033[0m" << std::endl;
	}
	
	/*
	Print out a warning message to the console.
	Can be suppressed with suppressWarings() and
	unsupressed with releseWarnings().
	\param any 'coutable' types in order in which they are meant to be printed
	*/
	template<typename ...TArgs>
	void WARNING(TArgs... message)
	{
		if (suppressWarningFlag) { return; };

		std::cout << "\x1b[33m";	//sets a yellow color
		(std::cout << ... << message);
		std::cout << "\033[0m" << std::endl;
	}
	
	/*
	Print out a trace message to the console.
	Can be suppressed with suppressTrace() and
	unsupressed with releseTrace().
	\param any 'coutable' types in order in which they are meant to be printed
	*/
	template<typename ...TArgs>
	void TRACE(TArgs... message)
	{
		if (suppressTraceFlag) { return; };

		std::cout << "\x1b[32m";	//sets a green color
		(std::cout << ... << message);
		std::cout << "\033[0m" << std::endl;
	}

}