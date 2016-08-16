#include "Class_factory.h"

#include <Shlwapi.h>
#include "Com_component.h"

extern ULONG component_ref_counter;

Class_factory::Class_factory():
_ref_count(1){
	InterlockedIncrement(&component_ref_counter);
}

Class_factory::~Class_factory()
{
	InterlockedDecrement(&component_ref_counter);
}

IFACEMETHODIMP Class_factory::QueryInterface(REFIID riid, void **ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(Class_factory, IClassFactory),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) Class_factory::AddRef()
{
	return InterlockedIncrement(&_ref_count);
}

IFACEMETHODIMP_(ULONG) Class_factory::Release()
{
	if (InterlockedDecrement(&_ref_count) == 0)
	{
		delete this;
	}
	return _ref_count;
}

IFACEMETHODIMP Class_factory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv)
{
	HRESULT hr = CLASS_E_NOAGGREGATION;

	if (pUnkOuter == NULL)
	{
		hr = E_OUTOFMEMORY;
	}

	Com_component* component = new  (std::nothrow) Com_component();
	if (component) {
		hr = component->QueryInterface(riid, ppv);
		component->Release();
	}

	return hr;
}

IFACEMETHODIMP Class_factory::LockServer(BOOL fLock)
{
	if (fLock)
	{
		InterlockedIncrement(&component_ref_counter);
	}
	else
	{
		InterlockedDecrement(&component_ref_counter);
	}
	return S_OK;
}