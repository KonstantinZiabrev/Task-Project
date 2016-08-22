#pragma once

#include <WinSock2.h>
#include <windows.h>
#include <shlobj.h> 
#include "Com_inner.h"

static const wchar_t* MENU_NAME = L"My shell extension";
static const wchar_t* HELP_STRING = L"My_shell_extension";
static const wchar_t* CANONICAL_VERB = L"My_shell_extension_verb";

class Com_component : public IShellExtInit, public IContextMenu {
public:
	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	// IShellExtInit
	IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);

	// IContextMenu
	IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
	IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
	IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax);

	Com_component();
protected:
	~Com_component();
private:
	ULONG _ref_count;
	std::wstring collector;
	Com_inner _com_inner;
};
