#pragma once

#include <unknwn.h>   
#include <windows.h>


class Class_factory : public IClassFactory
{
public:
	IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	IFACEMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv);
	IFACEMETHODIMP LockServer(BOOL fLock);

	Class_factory();

protected:
	~Class_factory();

private:
	ULONG _ref_count;
};