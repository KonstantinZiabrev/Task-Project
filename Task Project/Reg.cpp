/****************************** Module Header ******************************\
Module Name:  Reg.cpp
Project:      CppShellExtContextMenuHandler
Copyright (c) Microsoft Corporation.

The file implements the reusable helper functions to register and unregister 
in-process COM components and shell context menu handlers in the registry.

RegisterInprocServer - register the in-process component in the registry.
UnregisterInprocServer - unregister the in-process component in the registry.
RegisterShellExtContextMenuHandler - register the context menu handler.
UnregisterShellExtContextMenuHandler - unregister the context menu handler.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "Reg.h"
#include <strsafe.h>


#pragma region Registry Helper Functions

//
//   FUNCTION: SetHKCRRegistryKeyAndValue
//
//   PURPOSE: The function creates a HKCR registry key and sets the specified 
//   registry value.
//
//   PARAMETERS:
//   * pszSubKey - specifies the registry key under HKCR. If the key does not 
//     exist, the function will create the registry key.
//   * pszValueName - specifies the registry value to be set. If pszValueName 
//     is NULL, the function will set the default value.
//   * pszData - specifies the string data of the registry value.
//
//   RETURN VALUE: 
//   If the function succeeds, it returns S_OK. Otherwise, it returns an 
//   HRESULT error code.
// 
HRESULT SetHKCRRegistryKeyAndValue(PCWSTR pszSubKey, PCWSTR pszValueName, 
    PCWSTR pszData)
{
    HRESULT hr;
    HKEY hKey = NULL;

    // Creates the specified registry key. If the key already exists, the 
    // function opens it. 
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, pszSubKey, 0, 
        NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));

    if (SUCCEEDED(hr))
    {
        if (pszData != NULL)
        {
            // Set the specified value of the key.
            DWORD cbData = lstrlen(pszData) * sizeof(*pszData);
            hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, pszValueName, 0, 
                REG_SZ, reinterpret_cast<const BYTE *>(pszData), cbData));
        }

        RegCloseKey(hKey);
    }

    return hr;
}
