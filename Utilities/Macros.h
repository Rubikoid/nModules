/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Macros.h
 *  The nModules Project
 *
 *  Utility macros
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

// Lets you skip to a certain part in the function if a CHECKHR fails
//#define CHECKHR(var,exp) if (FAILED(var = (exp))) { goto chkeckhr_end; }
//#define CHECKHR_END() chkeckhr_end:

// Safe way to delete objects
#define SAFEDELETE(obj) if (obj != nullptr) { delete obj; obj = nullptr; }

// Safe way to release objects which require ->Release()
#define SAFERELEASE(x) if (x != nullptr) { (x)->Release(); x = nullptr; }

// Safe way to free memory
#define SAFEFREE(x) if (x != nullptr) { free(x); x = nullptr; }

// Returns the failed HRESULT on failure
#define RETURNONFAIL(hr, x) if (FAILED((hr) = (x))) { return hr; }

// Easy DLL import/export
#define EXPORT_CDECL(type) EXTERN_C __declspec(dllexport) type __cdecl
#define EXPORT_STDCALL(type) EXTERN_C __declspec(dllexport) type __stdcall