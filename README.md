# KPatcher

![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?logo=cplusplus&logoColor=white)
![Windows](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows&logoColor=white)
![MFC](https://img.shields.io/badge/Framework-MFC-68217A?logo=microsoft&logoColor=white)
![Visual Studio](https://img.shields.io/badge/IDE-Visual%20Studio%202015%2B-5C2D91?logo=visualstudio&logoColor=white)
![Win32](https://img.shields.io/badge/Arch-Win32-6e6e6e)
![ZLib](https://img.shields.io/badge/Lib-zlib-lightgrey)
![UnRAR](https://img.shields.io/badge/Lib-UnRAR-lightgrey)
![HTTP/FTP](https://img.shields.io/badge/Protocol-HTTP%20%7C%20FTP-22863a)
![Ragnarok Online](https://img.shields.io/badge/Game-Ragnarok%20Online-blue)
[![GitHub Stars](https://img.shields.io/github/stars/VaderTi/Patcher?style=social)](https://github.com/VaderTi/Patcher/stargazers)
[![GitHub Forks](https://img.shields.io/github/forks/VaderTi/Patcher?style=social)](https://github.com/VaderTi/Patcher/network/members)

**KPatcher** is a skinnable game patcher and launcher written in C++ (MFC/Win32), designed for **Ragnarok Online** private and official servers. It downloads patch files from a remote HTTP or FTP server, applies them to the game's GRF archives and loose files, and can automatically launch the game client afterwards.

**Supported GRF versions:** all versions  
**Supported patch archives:** RGZ and RAR  
**Supported transfer protocols:** HTTP and FTP (including username/password authentication)

---

## Table of Contents

- [Features](#features)
  - [KPatcher Features](#kpatcher-features)
  - [Config Tool Features](#config-tool-features)
- [Setup Guide](#setup-guide)
- [Architecture Overview](#architecture-overview)
- [Project Structure](#project-structure)
- [Third-Party Libraries](#third-party-libraries)
- [Configuration](#configuration)
  - [local.ini – bootstrap settings](#localini--bootstrap-settings)
  - [settings.ini – runtime settings](#settingsini--runtime-settings)
  - [skin.ini – UI layout](#skinini--ui-layout)
  - [info.inf – patch state](#infoinf--patch-state)
- [Patch List Format](#patch-list-format)
- [Supported Patch Types](#supported-patch-types)
- [Skinning System](#skinning-system)
- [Custom Buttons](#custom-buttons)
- [Localization](#localization)
- [Building](#building)
- [Deployment](#deployment)
- [Debug Mode](#debug-mode)

---

## Features

### KPatcher Features

1. **Fast GRF/GPF merge** – efficiently merges patch archives directly into the target GRF file.
2. **GRF defragmentation** – reclaims wasted space inside GRF archives after repeated patching.
3. **Delete files from GRF by mask** – removes entries matching a wildcard pattern from any GRF archive.
4. **Delete client files by mask** – removes loose files or directories from the game folder using a wildcard mask.
5. **Unpack RGZ/RAR archives** – decompresses zlib-based RGZ archives and extracts RAR archives.
6. **Patch any GRF in the client folder** – each patch entry can target a different GRF file, not just the default one.
7. **Unique auto-update** – the patcher can update itself and related files before applying game patches.
8. **Official patch server support** – built-in button to pull patches from the Korean Ragnarok Online servers (`webpatch.ragnarok.co.kr`).
9. **Simple skinning** – fully customisable window shape and UI layout driven by a `skin.ini` file.
10. **Remote settings and auto-update** – `settings.ini` and `update.ini` are fetched from a configurable remote URL so server-side changes are picked up automatically.
11. **Fully embedded skin** – the skin folder can be compiled into the EXE as a compressed resource blob and extracted at runtime.
12. **New Login System support** – can pass credentials to the game client on launch.

### Config Tool Features

The **Config Tool** (`ConfigTool`) is a companion utility for preparing a release build of KPatcher:

1. **Embed configuration file** – bakes `settings.ini` (or `Localhost.kpsf`) directly into the patcher EXE.
2. **Embed language file** – compiles the localisation string file into the EXE resource table.
3. **Embed skin** – packages the skin folder as a compressed resource blob inside the EXE.
4. **Change patcher icon** – replaces the default application icon in the EXE with a custom one.
5. **Get CRC of any file** – computes the CRC checksum of a file for use in `update.ini` integrity checks.

---

## Setup Guide

Follow these steps to configure and release KPatcher for your private server:

1. **Edit `Localhost.kpsf`** – set the bootstrap URL that points to your web server where `settings.ini` is hosted.
2. **Prepare a language file** – if no translation exists for your language, create one based on an existing language file and add it to the project.
3. **Create your skin** – design a background image (`skin.png`) and button images, then configure their positions in `skin.ini`. Use `#FF00FF` as the transparency colour for non-rectangular window edges.
4. **Upload server-side files** – place `settings.ini`, `update.ini`, the patch list (`patch.lst`), and all patch files on your web server.
5. **Edit `settings.ini`** – configure `PatchListURL`, `FileServerURL`, `Grf`, `NoticeURL`, the executable name, and any other runtime settings.
6. **Edit `update.ini`** – list any patcher self-update entries with their CRC checksums (obtainable via Config Tool).
7. **Build with Config Tool** – use the Config Tool to embed the config file, language file, and skin into the final `Patcher.exe`, and optionally replace the icon.

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                         CPatcher (CWinApp)                       │
│  owns: CSettings · CSkin · CLanguage · CPatcherForm              │
└────────────┬────────────────────────────────────────────────────┘
             │ creates & shows modal dialog
             ▼
┌─────────────────────────────────────────────────────────────────┐
│                     CPatcherForm (CDialog)                        │
│  UI controls (read from skin.ini):                               │
│   · CBrowser  – in-app notice/news page                          │
│   · CLabel    – Status text, Info text                           │
│   · CProgressCtrl – Current-file progress, Total progress        │
│   · CImgButton – Start, Exit, KRO, custom buttons                │
│                                                                   │
│  On init: calls StartThread() → spawns background PatchThread    │
└────────────┬────────────────────────────────────────────────────┘
             │ background MFC thread
             ▼
┌─────────────────────────────────────────────────────────────────┐
│                       PatchThread (UINT)                          │
│  orchestrates:                                                    │
│   CPatchThread::LoadSettings()                                   │
│   CPatchThread::GetPatchList()   – download patch.lst via HTTP/FTP│
│   CPatchThread::ParsePatchList() – parse into _PatchList vector  │
│   CPatchThread::DownloadPatches()– bulk-download new patches      │
│   CPatchThread::ProcessPatches() – apply each patch              │
└────────────┬────────────────────────────────────────────────────┘
             │ uses
             ├── CInternet  – WinInet HTTP & FTP wrapper
             ├── GrfLib     – GRF archive open / merge / delete
             ├── UnRAR      – RAR archive extraction
             └── ZLib       – RGZ (deflate) extraction
```

### Class Responsibilities

| Class | File | Responsibility |
|---|---|---|
| `CPatcher` | `Patcher.h/.cpp` | MFC application entry point; owns global state |
| `CPatcherForm` | `PatcherForm.h/.cpp` | Main dialog; builds UI from skin config; routes button actions |
| `CSettings` | `Settings.h/.cpp` | Loads `local.ini` (bootstrap) and `settings.ini` (remote config) |
| `CSkin` | `Skin.h/.cpp` | Reads `skin.ini`, stores positions/sizes of every UI element |
| `CLanguage` | `Language.h/.cpp` | Loads localised strings from compiled string-table resources |
| `CPatchThread` | `PatchThread.h/.cpp` | Download + apply patch logic; tracks last applied patch ID |
| `CInternet` | `Internet.h/.cpp` | WinInet-based HTTP/FTP downloader with progress callbacks |
| `CIni` | `Ini.h/.cpp` | INI file parser / writer |
| `CRes` | `Res.h/.cpp` | Reads/writes zlib-compressed resource blobs embedded in the EXE |
| `CBrowser` | `Browser.h/.cpp` | `CHtmlView` wrapper; navigation hook to block external popups |
| `CLabel` | `Label.h/.cpp` | Owner-draw static text with custom font/colour |
| `CImgButton` | `ImgButton.h/.cpp` | Owner-draw button with Normal / Hover / Pressed / Disabled PNG states |

---

## Project Structure

```
Patcher/
├── Patcher.sln                  Visual Studio solution
├── App/                         Output directory (built EXEs land here)
│   ├── Patcher.exe              Release build
│   ├── Patcher_d.exe            Debug build
│   ├── settings.ini             Example runtime configuration
│   ├── info.inf                 Patch state file (last applied patch ID)
│   └── Default.skin/           Default UI skin
│       ├── skin.ini             UI layout configuration
│       ├── skin.png             Background image (chroma-key transparent)
│       ├── start_u/f/d.png      Start button states
│       ├── exit_u/f/d.png       Exit button states
│       └── kro_u/f/d.png        KRO button states
└── SRC/
    ├── Patcher/                 Source files
    │   ├── Patcher.cpp/.h       Application class
    │   ├── PatcherForm.cpp/.h   Main dialog
    │   ├── PatchThread.cpp/.h   Patch download & apply logic
    │   ├── Thread.cpp/.h        MFC thread launcher
    │   ├── Settings.cpp/.h      Configuration loader
    │   ├── Skin.cpp/.h          Skin/layout loader
    │   ├── Language.cpp/.h      Localisation
    │   ├── Internet.cpp/.h      HTTP/FTP downloader
    │   ├── Ini.cpp/.h           INI parser
    │   ├── Res.cpp/.h           Embedded resource manager
    │   ├── Browser.cpp/.h       Embedded browser control
    │   ├── Label.cpp/.h         Custom text label
    │   ├── ImgButton.cpp/.h     Image button control
    │   ├── Helpers.cpp/.h       UI helpers, file utilities, UnRAR wrapper
    │   ├── StdAfx.h/.cpp        Precompiled header
    │   ├── Resource.h           String/dialog resource IDs
    │   ├── Patcher.rc           Resource script (icons, strings)
    │   ├── Patcher.vcxproj      Visual Studio project file
    │   └── Res/                 Resources embedded in EXE
    │       ├── local.ini        Default bootstrap INI (compiled into EXE)
    │       └── skin.png         Fallback skin image
    └── libs/
        ├── grflib/              GRF archive library (GrfLib.h + .lib)
        ├── unrar/               UnRAR library (unrar.hpp + .lib)
        └── zlib/                ZLib compression (zlib.h + .lib)
```

---

## Third-Party Libraries

| Library | Location | Purpose |
|---|---|---|
| **GrfLib** | `SRC/libs/grflib/` | Create, open, merge, and delete files inside Ragnarok Online `.grf` archives |
| **UnRAR** | `SRC/libs/unrar/` | Extract `.rar` patch archives |
| **ZLib** | `SRC/libs/zlib/` | Decompress `.rgz` patch archives (raw deflate streams) |

All three are provided as pre-built static `.lib` files for Win32 (separate Debug `_d` and Release variants).

---

## Configuration

### `local.ini` – bootstrap settings

Compiled into the EXE as an embedded resource (`Res/local.ini`). On first run it is extracted, read, then deleted. It tells the patcher where to download the real `settings.ini` from.

```ini
[Settings::Remote]
Url      = http://your-server.com/patches/   ; Base URL for remote settings
Settings = settings.ini                       ; Name of the remote settings file
Update   = update.ini                         ; Name of the auto-update manifest (reserved)
```

### `settings.ini` – runtime settings

Placed next to the executable (or downloaded from the URL above). Controls all runtime behaviour.

```ini
[Settings::Main]
ServerName = Patcher v 4.0    ; Display name (informational)
ServerUID  = PATCHER          ; Unique ID – used as section name in info.inf

[Settings::Browsers]
NoticeURL  = http://your-server.com/notice.html  ; Loaded into the embedded browser

[Settings::PatchServer::Main]
PatchListURL  = http://your-server.com/          ; URL of the directory containing the patch list
PatchListName = patch.lst                         ; File name of the patch list
FileServerURL = http://your-server.com/patches/  ; Base URL for downloading patch files
                                                  ; (supports http:// and ftp://)
Grf           = server.grf                        ; Target GRF archive for GPF/GDF patches
PatchStore    = info.inf                          ; File to persist last applied patch ID

[Settings::PatchServer::KRO]
; StartType = 1 enables this block; 0 (default) disables the KRO button
PatchListURL  = http://webpatch.ragnarok.co.kr/patch/
PatchListName = patch.txt
FileServerURL = ftp://ragnarok.nowcdn.co.kr:20021/Patch/
Grf           = data.grf
PatchStore    = patch.inf

[Settings::Executable]
AutoStart = 0             ; 1 = start ExeName automatically after patching
ExeName   = client.exe    ; Executable to launch (Start button / AutoStart)
ExeParam  =               ; Extra command-line parameters passed to ExeName
```

### `skin.ini` – UI layout

Located in `Default.skin/skin.ini` (or any folder named `*.skin` alongside the EXE). All coordinates are in pixels relative to the top-left of the background image.

```ini
[Patcher::Style]
Skin = Skin.png              ; Background image; #FF00FF pixels become transparent

[Browser::Notice]            ; Embedded web browser (notice/news page)
Left   = 11
Top    = 155
Width  = 390
Height = 229

[Text::Status]               ; Status line – "Connecting…", "Downloading…", etc.
Left   = 13
Top    = 405
Width  = 400
Height = 15
Color  = FFFF80              ; Font colour as RRGGBB hex
Size   = 8                   ; Font size in points
Bold   = 1                   ; 1 = bold, 0 = normal

[Text::Info]                 ; Per-file detail line (same fields as Text::Status)
Left   = 13
Top    = 425
Width  = 400
Height = 15
Color  = FFFF80
Size   = 8
Bold   = 1

[ProgressBar::Current]       ; Progress bar for the current file
Left   = 13
Top    = 450
Width  = 485
Height = 15

[ProgressBar::Total]         ; Progress bar across all patches
Left   = 13
Top    = 467
Width  = 485
Height = 15

[Button::Start]              ; Launches the game client
Left    = 11
Top     = 103
Active  = start_u.png        ; Normal state (PNG relative to skin folder)
Hovered = start_f.png        ; Mouse-over state
Pressed = start_d.png        ; Mouse-down state

[Button::Exit]               ; Closes the patcher
Left    = 420
Top     = 346
Active  = exit_u.png
Hovered = exit_f.png
Pressed = exit_d.png

[Button::KRO Patches]        ; Downloads official KRO patches
Left    = 421
Top     = 155
Active  = kro_u.png
Hovered = kro_f.png
Pressed = kro_d.png

[Patcher::Customs]
CBC = 0                      ; Number of custom buttons (0–10)
```

### `info.inf` – patch state

Tracks the last successfully applied patch ID so the patcher can skip already-applied patches on subsequent runs.

```ini
[PATCHER]           ; Section name = ServerUID from settings.ini
LastPatchID = 3
```

---

## Patch List Format

### Main server (`patch.lst`)

Each line follows the format:

```
<id>:<type>:<filename>[:<grf>]
```

| Field | Description |
|---|---|
| `id` | Integer patch sequence number |
| `type` | `gpf`, `grf`, `rgz`, `rar`, `gdf`, `cdf` |
| `filename` | Name of the patch file on the file server |
| `grf` | *(optional)* Target GRF archive; defaults to `Grf` setting |

**Examples:**

```
1:gpf:patch_001.gpf:server.grf
2:rgz:patch_002.rgz
3:rar:patch_003.rar
4:gdf:obsolete_file.txt:server.grf
5:cdf:old_dir/
```

Lines starting with `#`, `;`, `//`, or whitespace are treated as comments and ignored.

### KRO server (`patch.txt`)

Two-column space-separated format used by the official Korean Ragnarok Online patch server:

```
<id> <filename>
```

The patch type is inferred from the file extension.

---

## Supported Patch Types

| Type code | Extension(s) | Processing |
|---|---|---|
| `GPF` | `.gpf`, `.grf` | Merged into the target GRF archive via `GrfMerge()` |
| `RGZ` | `.rgz` | Decompressed with ZLib and extracted to the game directory |
| `RAR` | `.rar` | Extracted to the game directory via UnRAR |
| `GDF` | `.gdf` | Deletes a named entry from a GRF archive via `GrfDelete()` |
| `CDF` | `.cdf` | Deletes loose files / directories from the game directory |

---

## Skinning System

1. The patcher looks for a folder named `Default.skin\` next to the EXE.
2. It reads `skin.ini` from that folder to get coordinates and image paths for every UI element.
3. The main background (`skin.png`) is loaded as a `CImage`. Every pixel that is exactly `#FF00FF` (RGB 255, 0, 255) is excluded from the window region, creating a non-rectangular, transparent-edged window.
4. Buttons use GDI+ `Image` objects; they support 32-bit PNG with per-pixel alpha, so smooth edges and semi-transparency are possible.
5. The skin folder can be re-distributed alongside the EXE or embedded in the EXE as a compressed resource blob and extracted at startup via `CSkin::SaveSkin()`.

---

## Custom Buttons

Up to **10** custom buttons can be added in `skin.ini`:

```ini
[Patcher::Customs]
CBC = 2           ; number of custom buttons

[Button::Custom 1]
Left       = 10
Top        = 500
Active     = btn1_u.png
Hovered    = btn1_f.png
Pressed    = btn1_d.png
Action     = 3          ; 0=nothing · 1=run client · 2=run app · 3=open URL
FirstParam = https://your-website.com
SecondParam=            ; additional args (for actions 1 & 2) or ignored (for 3)

[Button::Custom 2]
Action     = 2
FirstParam = tools\repair.exe
SecondParam= --silent
```

---

## Localization

String resources are compiled into the EXE for three languages and selected automatically based on `GetUserDefaultLangID()`:

| Language | LANGID |
|---|---|
| Russian | `MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT)` |
| English | `MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT)` |
| French | `MAKELANGID(LANG_FRENCH, SUBLANG_DEFAULT)` |

Any other system language falls back to **English**. All localised strings are declared in `Language.h` and loaded via `CLanguage::Load()` from the RC string table.

---

## Building

### Prerequisites

- **Visual Studio 2015** (toolset `v140`) or newer with the **MFC** and **C++ for Windows** workloads installed.
- Windows SDK (target Windows 7 or later).
- No external package manager is required; all third-party libraries are included as pre-built `.lib` files under `SRC/libs/`.

### Steps

1. Open `Patcher.sln` in Visual Studio.
2. Select the desired configuration:
   - **Debug | Win32** – builds `App/Patcher_d.exe`, links MFC as a DLL, opens a console window for diagnostic output.
   - **Release | Win32** – builds `App/Patcher.exe`, links MFC statically, full optimisations enabled.
3. Build the solution (`Ctrl+Shift+B`).

The output EXE is placed in the `App/` directory automatically.

### Project settings summary

| Setting | Debug | Release |
|---|---|---|
| Toolset | v140 | v140 |
| Platform | Win32 | Win32 |
| MFC | Dynamic (DLL) | Static |
| Optimisation | Disabled | Max Speed + LTCG |
| Runtime library | MTd (DLL) | MT (static) |
| Output | `App/Patcher_d.exe` | `App/Patcher.exe` |

---

## Deployment

A minimal deployment alongside the game client requires:

```
<game root>/
├── Patcher.exe
├── settings.ini          ; configure your patch server URLs here
├── info.inf              ; auto-created on first run
└── Default.skin/
    ├── skin.ini
    ├── Skin.png
    ├── start_u.png · start_f.png · start_d.png
    ├── exit_u.png  · exit_f.png  · exit_d.png
    └── kro_u.png   · kro_f.png   · kro_d.png
```

On startup the patcher:

1. Extracts the embedded `local.ini` to determine where to fetch `settings.ini`.
2. Downloads and parses `settings.ini` from the remote URL (or reads it locally if present).
3. Loads the skin, creates the main window, and starts the patch thread.
4. The patch thread downloads the patch list, compares it with `info.inf`, downloads only new files into a temporary `updates/` sub-directory, applies them, and cleans up.
5. If `AutoStart = 1` is set, the configured executable is launched automatically and the patcher closes.

---

## Debug Mode

When compiled in **Debug** configuration (`_DEBUG` defined):

- A console window is allocated at startup and freed on exit.
- All `DbgMsg(...)` calls print timing and progress information to the console (e.g., skin drawing time, bytes downloaded, patches processed).
- The debug EXE is named `Patcher_d.exe` to avoid overwriting a production binary.
