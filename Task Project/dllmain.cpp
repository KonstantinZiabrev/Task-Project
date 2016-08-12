#include <windows.h>
#include <Guiddef.h>
#include <string>
#include "Reg.h"
#include "Class_factory.h"


// {F75F8B95-A1CA-4495-9270-1A5920EC8C27}
static const GUID CLSID_MyShellExt =
{ 0xf75f8b95, 0xa1ca, 0x4495,{ 0x92, 0x70, 0x1a, 0x59, 0x20, 0xec, 0x8c, 0x27 } };

HINSTANCE	current_module = NULL;
ULONG		component_ref_counter = 0;



BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		current_module = hModule;
		DisableThreadLibraryCalls(hModule);
	}
	return TRUE;
}

STDAPI DllCanUnloadNow(void)
{
	return component_ref_counter > 0 ? S_FALSE : S_OK;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
	HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;
	if (IsEqualCLSID(CLSID_MyShellExt, rclsid))
	{
		hr = E_OUTOFMEMORY;

		Class_factory *class_factory = new Class_factory();
		if (class_factory)
		{
			hr = class_factory->QueryInterface(riid, ppv);
			class_factory->Release();
		}
	}

	return hr;
}

STDAPI DllRegisterServer(void)
{
	HRESULT hr;

	wchar_t szModule[MAX_PATH];
	if (GetModuleFileName(current_module, szModule, ARRAYSIZE(szModule)) == 0)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	wchar_t szCLSID[MAX_PATH];
	StringFromGUID2(CLSID_MyShellExt, szCLSID, ARRAYSIZE(szCLSID));

	// Create the HKCR\CLSID\{<CLSID>} key.

	std::wstring register_key = L"CLSID\\" + std::wstring(szCLSID);

	//Set the friendly name
	hr = SetHKCRRegistryKeyAndValue(register_key.c_str(), NULL, L"My Com component friendly name");

	// Create the HKCR\CLSID\{<CLSID>}\InprocServer32 key.
	register_key = L"CLSID\\" + std::wstring(szCLSID) + L"\\InprocServer32";

	//Set the default value to compponent address 
	hr = SetHKCRRegistryKeyAndValue(register_key.c_str(), NULL, szModule);

	//Set the threading model
	hr = SetHKCRRegistryKeyAndValue(register_key.c_str(), L"ThreadingModel", L"Apartment");

	// Create the key HKCR\<File Type>\shellex\ContextMenuHandlers\{<CLSID>}

	register_key = L"*\\shellex\\ContextMenuHandlers\\" + std::wstring(szCLSID);

	//Set it
	hr = SetHKCRRegistryKeyAndValue(register_key.c_str(), NULL, L"My shell ext");

	return hr;
}




STDAPI DllUnregisterServer(void)
{
	HRESULT hr = S_OK;

	wchar_t szModule[MAX_PATH];
	if (GetModuleFileName(current_module, szModule, ARRAYSIZE(szModule)) == 0)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	wchar_t szCLSID[MAX_PATH];
	StringFromGUID2(CLSID_MyShellExt, szCLSID, ARRAYSIZE(szCLSID));

	// Delete the HKCR\CLSID\{<CLSID>} key.
	std::wstring register_key = L"CLSID\\" + std::wstring(szCLSID);

	hr = HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, register_key.c_str()));

	//Delete the shell extension registry
	register_key = L"*\\shellex\\ContextMenuHandlers\\" + std::wstring(szCLSID);

	hr = HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, register_key.c_str()));

	return hr;
}
