#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <dirent.h>
#include <errno.h>
#include <shellapi.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char exe_dir[MAX_PATH];
	GetModuleFileNameA(NULL, exe_dir, MAX_PATH);

	// Strip executable filename to isolate launcher's directory
	char *last_slash = strrchr(exe_dir, '\\');
	if (last_slash != NULL)
		*last_slash = '\0';

	char app_path[MAX_PATH];
	char data_path[MAX_PATH];
	char zed_exe_path[MAX_PATH];

	snprintf(app_path, sizeof(app_path), "%s\\app", exe_dir);
	snprintf(data_path, sizeof(data_path), "%s\\data", exe_dir);
	snprintf(zed_exe_path, sizeof(zed_exe_path), "%s\\app\\zed.exe", exe_dir);

	DIR *app_dir = opendir(app_path);
	DIR *data_dir = opendir(data_path);

	if (data_dir)
		closedir(data_dir);
	else
		CreateDirectoryA(data_path, NULL);

	if (app_dir)
	{
		closedir(app_dir);

		char params[8192];
		if (lpCmdLine != NULL && *lpCmdLine != '\0')
		{
			snprintf(params, sizeof(params), "--user-data-dir \"%s\" %s", data_path, lpCmdLine);
		}
		else
		{
			snprintf(params, sizeof(params), "--user-data-dir \"%s\"", data_path);
		}

		ShellExecuteA(NULL, "open", zed_exe_path, params, exe_dir, SW_SHOWNORMAL);
	}
	else if (ENOENT == errno)
		MessageBox(NULL, "The Zed App Folder Is Not Found!", NULL, MB_OK | MB_ICONWARNING);
	else
		MessageBox(NULL, "Failed To Access Zed Files", NULL, MB_OK | MB_ICONWARNING);

	return 0;
}
