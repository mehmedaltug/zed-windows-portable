# Zed Portable Launcher for Windows

A lightweight, native C portable launcher for [Zed Editor](https://github.com/zed-industries/zed) on Windows. It isolates user settings, extensions, and configuration into a local `data` directory, keeping your installation completely portable while fully supporting command-line arguments.

If `app/` is not found, the launcher can automatically download, extract, and set up the latest Zed Windows build for you with a native progress GUI.

---

## Features

* **Zero-Config Portability:** Automatically forces Zed to store user data in `.\data` using `--user-data-dir ".\data"`.
* **CLI Argument Forwarding:** Seamlessly passes command-line arguments, options, files, and project directories straight to `zed.exe`.
* **Automatic Setup & Installer:** Prompts to automatically download and extract the latest Zed release if the `app/` folder is missing.
* **Native GUI Progress Dialog:** Displays real-time download progress (MB/total MB, percentage bar, and status updates) without external window dependencies.
* **Clean Background Extraction:** Unpacks `zed.zip` silently using Windows PowerShell and renames the folder seamlessly.
* **Lightweight C Binary:** Written in pure Win32 C with zero heavy dependencies or external runtimes.

---

## Directory Structure

### Before First Run

```
portable-root/
└── zed-launcher.exe    # Portable launcher

```

### After First Run / Auto-Download

```
portable-root/
├── zed-launcher.exe    # Portable launcher
├── app/                # Extracted Zed application (contains zed.exe)
└── data/               # Portable user settings and data

```

---

## Usage

You can use `zed-launcher.exe` as a drop-in replacement for `zed.exe`. Any arguments passed to the launcher are appended to Zed's startup command.

```bash
# Open Zed in portable mode
zed-launcher.exe

# Open the current folder in Zed
zed-launcher.exe .

# Open a specific file or directory
zed-launcher.exe C:\Projects\MyProject

# Pass additional flags to Zed
zed-launcher.exe --version

```

---

## How It Works

1. **Folder Check:** Checks for `.\app` and `.\data` in the directory where `zed-launcher.exe` resides.
2. **Auto-Download (if needed):**
* If `.\app` is missing, prompts you with a dialog to download the latest build from [deevus/zed-windows-builds](https://github.com/deevus/zed-windows-builds).
* Shows a native Win32 progress dialog while downloading `zed.zip`.
* Extracts `zed.zip` in the background, renames `zed` to `app`, and cleans up temporary zip files.


3. **Launch & Passthrough:** Creates `.\data` if missing and launches `.\app\zed.exe --user-data-dir ".\data" [YOUR_ARGUMENTS]`.

---

## Building from Source

### Prerequisites

* **MinGW-w64** (`gcc`, `windres`, `make`)
* An icon file named `app.ico` in the source root directory

### Source Files

Ensure your project directory contains:

* `main.c` — Main C source code
* `resource.rc` — Resource definition file (`1 ICON "app.ico"`)
* `app.ico` — Application icon
* `Makefile` — Build configuration

### Build Commands

To build the executable with the embedded icon:

```bash
make

```

To clean build artifacts:

```bash
make clean

```

---

## Credits

* Unofficial Windows binaries supplied by [deevus/zed-windows-builds](https://github.com/deevus/zed-windows-builds).
* Zed Editor developed by [Zed Industries](https://zed.dev/).
