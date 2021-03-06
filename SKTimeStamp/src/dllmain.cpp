// SkTimeStamp - Change file dates easily, directly from explorer

// Copyright (C) 2012 - Stefan Kueng

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "stdafx.h"
#include "ShellExt.h"
#include "ShellExtClassFactory.h"
#include <olectl.h>

HINSTANCE   g_hmodThisDll = NULL;           ///< handle to this DLL itself.
UINT        g_cRefThisDll = 0;              ///< reference count of this DLL.


extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance,
                               DWORD dwReason,
                               LPVOID /*lpReserved*/)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Extension DLL one-time initialization
        g_hmodThisDll = hInstance;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


STDAPI DllCanUnloadNow(void)
{
    return (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = NULL;

    if (IsEqualIID(rclsid, CLSID_SKTIMESTAMP))
    {
        CShellExtClassFactory *pcf = new CShellExtClassFactory();
        return pcf->QueryInterface(riid, ppvOut);
    }

    return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllInstall(__in BOOL /*bInstall*/, __in LPCWSTR /*pszCmdLine*/)
{
    return S_OK;
}

STDAPI DllRegisterServer(void)
{
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    TCHAR szModule[MAX_PATH*4] = {0};
    // Get this dll's path and file name.
    DWORD retval = GetModuleFileName(g_hmodThisDll, szModule, _countof(szModule));
    if (retval == NULL)
    {
        CoUninitialize();
        return SELFREG_E_CLASS;
    }

    if (RegSetValue(HKEY_CLASSES_ROOT, _T("*\\shellex\\PropertySheetHandlers\\SKTimeStamp"), REG_SZ, _T(SKTIMESTAMP_GUID), (DWORD)_tcslen(_T(SKTIMESTAMP_GUID))*sizeof(TCHAR)) != ERROR_SUCCESS)
    {
        CoUninitialize();
        return SELFREG_E_CLASS;
    }
    if (RegSetValue(HKEY_CLASSES_ROOT, _T("Directory\\shellex\\PropertySheetHandlers\\SKTimeStamp"), REG_SZ, _T(SKTIMESTAMP_GUID), (DWORD)_tcslen(_T(SKTIMESTAMP_GUID))*sizeof(TCHAR)) != ERROR_SUCCESS)
    {
        CoUninitialize();
        return SELFREG_E_CLASS;
    }
    if (RegSetValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Classes\\CLSID\\") _T(SKTIMESTAMP_GUID), REG_SZ, _T("SKTimeStamp"), (DWORD)_tcslen(_T("SKTimeStamp"))*sizeof(TCHAR)) != ERROR_SUCCESS)
    {
        CoUninitialize();
        return SELFREG_E_CLASS;
    }
    if (RegSetValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Classes\\CLSID\\") _T(SKTIMESTAMP_GUID) _T("InProcServer32"), REG_SZ, szModule, (DWORD)_tcslen(szModule)*sizeof(TCHAR)) != ERROR_SUCCESS)
    {
        CoUninitialize();
        return SELFREG_E_CLASS;
    }
    if (SHSetValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Classes\\CLSID\\") _T(SKTIMESTAMP_GUID) _T("InProcServer32"), _T("ThreadingModel"), REG_SZ, _T("Apartment"), 20) != ERROR_SUCCESS)
    {
        CoUninitialize();
        return SELFREG_E_CLASS;
    }
    if (SHSetValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), _T(SKTIMESTAMP_GUID), REG_SZ, _T("SKTimeStamp"), 24) != ERROR_SUCCESS)
    {
        CoUninitialize();
        return SELFREG_E_CLASS;
    }
    CoUninitialize();
    return S_OK;
}

STDAPI DllUnregisterServer(void)
{
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SHDeleteKey(HKEY_CLASSES_ROOT, _T("*\\shellex\\PropertySheetHandlers\\SKTimeStamp")) != ERROR_SUCCESS)
    {
        CoUninitialize();
        return SELFREG_E_CLASS;
    }
    if (SHDeleteKey(HKEY_CLASSES_ROOT, _T("Directory\\shellex\\PropertySheetHandlers\\SKTimeStamp")) != ERROR_SUCCESS)
    {
        CoUninitialize();
        return SELFREG_E_CLASS;
    }
    if (SHDeleteKey(HKEY_CURRENT_USER, _T("SOFTWARE\\Classes\\CLSID\\") _T(SKTIMESTAMP_GUID)) != ERROR_SUCCESS)
    {
        CoUninitialize();
        return SELFREG_E_CLASS;
    }
    if (SHDeleteValue(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), _T(SKTIMESTAMP_GUID)) != ERROR_SUCCESS)
    {
        CoUninitialize();
        return SELFREG_E_CLASS;
    }
    CoUninitialize();
    return S_OK;
}
