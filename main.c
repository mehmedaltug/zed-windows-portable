#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include <errno.h>
#include <shellapi.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DIR *app_dir = opendir("app");
	DIR *data_dir = opendir("data");

	if (data_dir)
		closedir(data_dir);
	else
		mkdir("data");

	if (app_dir)
	{
		closedir(app_dir);
        ShellExecuteA(NULL, "open", "app\\zed.exe", "--user-data-dir .\\data", NULL, SW_SHOWNORMAL);
	}
	else if (ENOENT == errno)
		MessageBox(NULL, "The Zed App Folder Is Not Found!", NULL, MB_OK | MB_ICONWARNING);
	else
		MessageBox(NULL, "Failed To Access Zed Files", NULL, MB_OK | MB_ICONWARNING);

	return 0;
}
