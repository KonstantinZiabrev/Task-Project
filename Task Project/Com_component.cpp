#include "Com_component.h"

#include <Shlwapi.h>
#include <strsafe.h>

extern ULONG component_ref_counter;



Com_component::Com_component() :
	_ref_count(1){

	InterlockedIncrement(&component_ref_counter);
}

Com_component::~Com_component() {
	InterlockedDecrement(&component_ref_counter);
}

IFACEMETHODIMP Com_component::QueryInterface(REFIID riid, void **ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(Com_component, IContextMenu),
		QITABENT(Com_component, IShellExtInit),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) Com_component::AddRef()
{
	return InterlockedIncrement(&_ref_count);
}

IFACEMETHODIMP_(ULONG) Com_component::Release()
{
	if (InterlockedDecrement(&_ref_count) == 0)
	{
		delete this;
	}
	return _ref_count;
}

IFACEMETHODIMP Com_component::Initialize(
	LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID)
{
	if (NULL == pDataObj)
	{
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stm;
	wchar_t helper[MAX_PATH];
	_com_inner.reset();
	if (SUCCEEDED(pDataObj->GetData(&fe, &stm)))
	{
		HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
		if (hDrop != NULL)
		{
			UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
			for (unsigned i = 0; i < nFiles; ++i) {
				if (DragQueryFile(hDrop, i, helper, ARRAYSIZE(helper)) != 0) {
					_com_inner.add_filename(helper);
				}
				else {
					hr = E_FAIL;
					break;
				}
			}
			GlobalUnlock(stm.hGlobal);
		}

		ReleaseStgMedium(&stm);
	}
	
	if (hr != E_FAIL && _com_inner.records_processing() == false) {
		hr = E_FAIL;
	}
	return hr;
}

IFACEMETHODIMP Com_component::QueryContextMenu(
	HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	InsertMenu(hMenu, indexMenu, MF_BYPOSITION,
		idCmdFirst, MENU_NAME);

	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 1);
}

IFACEMETHODIMP Com_component::GetCommandString(UINT_PTR idCommand,
	UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax)
{
	if (idCommand != 0) {
	return E_INVALIDARG;
	}

	switch (uFlags) {
	case GCS_HELPTEXTW:
		StringCchCopy(reinterpret_cast<PWSTR>(pszName), cchMax, HELP_STRING);
		break;
	case GCS_VERBW:StringCchCopy(reinterpret_cast<PWSTR>(pszName), cchMax, CANONICAL_VERB);
		break;
	}
	return S_OK;
}


IFACEMETHODIMP Com_component::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	if (0 != HIWORD(pCmdInfo->lpVerb))
		return E_INVALIDARG;
	switch (LOWORD(pCmdInfo->lpVerb))
	{
	case 0:
	{


		MessageBox(pCmdInfo->hwnd, _com_inner.compose_answer().c_str(), L"SimpleShlExt", MB_ICONINFORMATION);

		return S_OK;
	}
	break;

	default:
		return E_INVALIDARG;
		break;
	}

}
