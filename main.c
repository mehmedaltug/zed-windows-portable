#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h>
#include <urlmon.h>
#include <dirent.h>
#include <errno.h>
#include <shellapi.h>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "comctl32.lib")

typedef struct {
	IBindStatusCallbackVtbl *lpVtbl;
	ULONG refCount;
	HWND hwndDlg;
	HWND hwndStatus;
	HWND hwndProgress;
} CustomDownloadCallback;

static HRESULT STDMETHODCALLTYPE CB_QueryInterface(IBindStatusCallback *This, REFIID riid, void **ppvObject)
{
	if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IBindStatusCallback)) {
		*ppvObject = This;
		return S_OK;
	}
	*ppvObject = NULL;
	return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE CB_AddRef(IBindStatusCallback *This) {return 1;}
static ULONG STDMETHODCALLTYPE CB_Release(IBindStatusCallback *This) {return 1;}
static HRESULT STDMETHODCALLTYPE CB_OnStartBinding(IBindStatusCallback *This, DWORD dwReserved, IBinding *pib) {return S_OK;}
static HRESULT STDMETHODCALLTYPE CB_GetPriority(IBindStatusCallback *This, LONG *pnPriority) {return S_OK;}
static HRESULT STDMETHODCALLTYPE CB_OnLowResource(IBindStatusCallback *This, DWORD reserved) {return S_OK;}
static HRESULT STDMETHODCALLTYPE CB_OnProgress(IBindStatusCallback *This, ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
{
	CustomDownloadCallback *pThis = (CustomDownloadCallback *)This;
	if (ulProgressMax > 0) {
		int pos = (int)(((double)ulProgress / (double)ulProgressMax) * 100.0);
		SendMessageA(pThis->hwndProgress, PBM_SETPOS, pos, 0);

		char status_str[256];
		snprintf(status_str, sizeof(status_str), "Downloading Zed... %.1f MB / %.1f MB (%d%%)",
			(double)ulProgress / (1024.0 * 1024.0),
			(double)ulProgressMax / (1024.0 * 1024.0),
			pos);
		SetWindowTextA(pThis->hwndStatus, status_str);
	}
	else if (ulProgress > 0) {
		char status_str[256];
		snprintf(status_str, sizeof(status_str), "Downloading Zed... %.1f MB",
			(double)ulProgress / (1024.0 * 1024.0));
		SetWindowTextA(pThis->hwndStatus, status_str);
	}

	MSG msg;
	while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE CB_OnStopBinding(IBindStatusCallback *This, HRESULT hresult, LPCWSTR szError) {return S_OK;}
static HRESULT STDMETHODCALLTYPE CB_GetBindInfo(IBindStatusCallback *This, DWORD *grfBINDF, BINDINFO *pbindinfo) {return S_OK;}
static HRESULT STDMETHODCALLTYPE CB_OnDataAvailable(IBindStatusCallback *This, DWORD grfBSCF, DWORD dwSize, FORMATETC *pformatetc, STGMEDIUM *pstgmed) {return S_OK;}
static HRESULT STDMETHODCALLTYPE CB_OnObjectAvailable(IBindStatusCallback *This, REFIID riid, IUnknown *pUnk) {return S_OK;}

static IBindStatusCallbackVtbl g_CallbackVtbl = {
	CB_QueryInterface,
	CB_AddRef,
	CB_Release,
	CB_OnStartBinding,
	CB_GetPriority,
	CB_OnLowResource,
	CB_OnProgress,
	CB_OnStopBinding,
	CB_GetBindInfo,
	CB_OnDataAvailable,
	CB_OnObjectAvailable
};

HWND CreateProgressWindow(HINSTANCE hInstance, HWND *outStatus, HWND *outProgress)
{
	INITCOMMONCONTROLSEX icce;
	icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icce.dwICC = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&icce);

	WNDCLASSA wc = {0};
	wc.lpfnWndProc = DefWindowProcA;
	wc.hInstance = hInstance;
	wc.lpszClassName = "ZedDownloadProgressWindow";
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassA(&wc);

	int width = 420;
	int height = 140;
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int x = (screenWidth - width) / 2;
	int y = (screenHeight - height) / 2;

	HWND hwnd = CreateWindowExA(
		WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
		"ZedDownloadProgressWindow",
		"Zed Launcher",
		WS_POPUP | WS_CAPTION | WS_VISIBLE,
		x, y, width, height,
		NULL, NULL, hInstance, NULL
	);

	*outStatus = CreateWindowExA(
		0, "STATIC", "Connecting to server...",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		20, 20, 365, 20,
		hwnd, (HMENU)101, hInstance, NULL
	);

	*outProgress = CreateWindowExA(
		0, PROGRESS_CLASSA, NULL,
		WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
		20, 48, 365, 22,
		hwnd, (HMENU)102, hInstance, NULL
	);

	SendMessageA(*outProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));

	UpdateWindow(hwnd);
	return hwnd;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char exe_dir[MAX_PATH];
	GetModuleFileNameA(NULL, exe_dir, MAX_PATH);

	char *last_slash = strrchr(exe_dir, '\\');
	if (last_slash != NULL)
		*last_slash = '\0';

	char app_path[MAX_PATH];
	char data_path[MAX_PATH];
	char zed_exe_path[MAX_PATH];

	snprintf(app_path, sizeof(app_path), "%s\\app", exe_dir);
	snprintf(data_path, sizeof(data_path), "%s\\data", exe_dir);
	snprintf(zed_exe_path, sizeof(zed_exe_path), "%s\\app\\zed.exe", exe_dir);

	DIR *data_dir = opendir(data_path);
	if (data_dir)
		closedir(data_dir);
	else
		CreateDirectoryA(data_path, NULL);

	DIR *app_dir = opendir(app_path);

	if (!app_dir) {
		int choice = MessageBoxA(
			NULL,
			"Zed folder was not found. Would you like to download and install the latest release?",
			"Zed Launcher - Download Required",
			MB_YESNO | MB_ICONQUESTION
		);

		if (choice != IDYES)
			return 0;

		HWND hwndStatus, hwndProgress;
		HWND hwndDlg = CreateProgressWindow(hInstance, &hwndStatus, &hwndProgress);

		CustomDownloadCallback cb;
		cb.lpVtbl = &g_CallbackVtbl;
		cb.refCount = 1;
		cb.hwndDlg = hwndDlg;
		cb.hwndStatus = hwndStatus;
		cb.hwndProgress = hwndProgress;

		char zip_path[MAX_PATH];
		char extracted_zed_path[MAX_PATH];
		snprintf(zip_path, sizeof(zip_path), "%s\\zed.zip", exe_dir);
		snprintf(extracted_zed_path, sizeof(extracted_zed_path), "%s\\zed", exe_dir);

		const char *download_url = "https://github.com/deevus/zed-windows-builds/releases/latest/download/zed.zip";

		HRESULT hr = URLDownloadToFileA(NULL, download_url, zip_path, 0, (IBindStatusCallback *)&cb);

		if (SUCCEEDED(hr)) {
			SetWindowTextA(hwndStatus, "Extracting Zed package... Please wait.");
			SendMessageA(hwndProgress, PBM_SETPOS, 100, 0);
			UpdateWindow(hwndDlg);

			char cmd[MAX_PATH * 3];
			snprintf(cmd, sizeof(cmd),
				"powershell.exe -NoProfile -ExecutionPolicy Bypass -Command \"Expand-Archive -LiteralPath '%s' -DestinationPath '%s' -Force\"",
				zip_path, exe_dir);

			STARTUPINFOA si;
			PROCESS_INFORMATION pi;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;
			ZeroMemory(&pi, sizeof(pi));

			if (CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, exe_dir, &si, &pi))	{
				while (WaitForSingleObject(pi.hProcess, 100) == WAIT_TIMEOUT) {
					MSG msg;
					while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
						TranslateMessage(&msg);
						DispatchMessageA(&msg);
					}
				}
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}

			DeleteFileA(zip_path);
			MoveFileA(extracted_zed_path, app_path);

			DestroyWindow(hwndDlg);
			app_dir = opendir(app_path);
		} else {
			DestroyWindow(hwndDlg);
			MessageBoxA(NULL, "Failed to download Zed package.", "Download Error", MB_OK | MB_ICONERROR);
			return 1;
		}
	}

	if (app_dir) {
		closedir(app_dir);

		char params[8192];
		if (lpCmdLine != NULL && *lpCmdLine != '\0')
			snprintf(params, sizeof(params), "--user-data-dir \"%s\" %s", data_path, lpCmdLine);
		else
			snprintf(params, sizeof(params), "--user-data-dir \"%s\"", data_path);

		ShellExecuteA(NULL, "open", zed_exe_path, params, exe_dir, SW_SHOWNORMAL);
	} else
		MessageBoxA(NULL, "Failed To Access or Extract Zed Files", "Error", MB_OK | MB_ICONWARNING);

	return 0;
}