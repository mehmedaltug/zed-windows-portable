#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include <errno.h>

int main()
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
		system("start /b app\\zed.exe --user-data-dir .\\data && exit");
	}
	else if (ENOENT == errno)
		MessageBox(NULL, "The Zed App Folder Is Not Found!", NULL, MB_OK | MB_ICONWARNING);
	else
		MessageBox(NULL, "Failed To Access Zed Files", NULL, MB_OK | MB_ICONWARNING);

	return 0;
}