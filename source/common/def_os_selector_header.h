#if defined(_WIN32) // OS Selector
/* OS:Windows */
#ifdef DINOBEI_DLLEXPORT
#define DINOBEI_DLLTYPE _declspec(dllexport)
#else
#define DINOBEI_DLLTYPE _declspec(dllimport)
#endif
#else
/* OS:Linux */
#endif // OS Selector End
