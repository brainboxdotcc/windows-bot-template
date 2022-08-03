/* Some user friendly sanity checks to cut down on silly support issues */

#if !defined(_MSC_VER) || (_MSC_VER < 1929)
	#error "This template is only for Microsoft Visual C++ 2019 and later. To build a D++ bot in Visual Studio Code, or on any other platform or compiler please use https://github.com/brainboxdotcc/templatebot"
#endif

#if (!defined(_MSVC_LANG) || _MSVC_LANG < 201703L)
	#error "D++ bots require C++17 or later. Please enable C++17 under project properties."
#endif

#if !defined(DPP_WIN_TEMPLATE) && !defined(DPP_CI)
	#error "You must compile this template using its .sln file. You cannot just double click the .cpp file and compile it on its own. Ensure you checked out the full source code of the template!"
#endif
