#pragma once

#ifdef IIDLL_EXPORTS
__declspec(dllexport)
#else
__declspec(dllimport)
#endif // EXPORT
void Inject();
