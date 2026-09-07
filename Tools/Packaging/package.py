#!/usr/bin/env python3

"""Afterlife Packaging Tool.

The final step of a build's production. Compiles the game + engine with the
"Packaging" CMake configuration and copies over all the game data into a
"packedBuild-{Target}" directory ready to ship.
"""

import glob
import os
import platform
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GAME_DATA = ROOT / "GameData"
BIN_DIR = ROOT / "bin"

BUILD_TESTING = ROOT / "cmake-build-testing"
BUILD_PACKAGING = ROOT / "cmake-build-packaging"

CONFIG_TESTING = "Testing"
CONFIG_PACKAGING = "Packaging"

HOST = platform.system()

DATA_DIRS = ["Input", "Sectors", "Shaders"]
PACKAGE_YAMLS = ["Assets", "Textures", "Meshes", "Sounds", "Music"]
PACKAGE_SELECTABLE = ["Meshes", "Textures", "Sounds", "Music"]

SUPPORTED_TARGETS = ["Linux", "Windows"]
STUB_TARGETS = [
    "MacOS",
    "Playstation5",
    "Playstation6",
    "XboxSeries",
    "XboxHelix",
    "NintendoS2",
    "NintendoS3",
]
TARGETS = SUPPORTED_TARGETS + STUB_TARGETS

if os.name == "nt":
    os.system("")

RESET = "\033[0m"
BOLD = "\033[1m"
RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
CYAN = "\033[36m"


# ----------------------------------------------------------------------------
# Output helpers
# ----------------------------------------------------------------------------

def info(msg):
    print(msg)


def ok(msg):
    print(f"{GREEN}{msg}{RESET}")


def warn(msg):
    print(f"{YELLOW}{msg}{RESET}")


def err(msg):
    print(f"{RED}{msg}{RESET}")


def section(msg):
    print(f"{BOLD}{CYAN}== {msg} =={RESET}")


# ----------------------------------------------------------------------------
# CMake detection (the project requires CMake >= 4.0, which the system cmake
# may not satisfy; CLion bundles a recent enough copy)
# ----------------------------------------------------------------------------

def _cmake_version(cmake):
    try:
        out = subprocess.run([cmake, "--version"], capture_output=True, text=True).stdout
        match = re.search(r"version\s+(\d+)\.(\d+)", out)
        if match:
            return (int(match.group(1)), int(match.group(2)))
    except Exception:
        pass
    return None


def _clion_cmake_candidates():
    home = Path.home()
    if os.name == "nt":
        patterns = [
            "C:/Program Files/JetBrains/CLion */bin/cmake/win/x64/bin/cmake.exe",
            str(home / "AppData/Local/Programs/*/bin/cmake/win/x64/bin/cmake.exe"),
            str(home / "AppData/Local/JetBrains/Toolbox/apps/CLion/ch-0/*/bin/cmake/win/x64/bin/cmake.exe"),
        ]
    else:
        patterns = [
            str(home / "Apps/Jetbrains/clion-*/bin/cmake/linux/x64/bin/cmake"),
            str(home / "Apps/JetBrains/clion-*/bin/cmake/linux/x64/bin/cmake"),
            str(home / ".local/share/JetBrains/Toolbox/apps/CLion/ch-0/*/bin/cmake/linux/x64/bin/cmake"),
            str(home / "clion-*/bin/cmake/linux/x64/bin/cmake"),
            "/opt/clion-*/bin/cmake/linux/x64/bin/cmake",
        ]
    for pat in patterns:
        for p in sorted(glob.glob(pat)):
            yield Path(p)


def find_cmake():
    override = os.environ.get("CMAKE")
    if override:
        p = Path(override)
        if p.exists():
            return str(p)
        warn(f"CMAKE env var points to a missing binary: {override}")

    on_path = shutil.which("cmake")
    if on_path and (_cmake_version(on_path) or (0, 0)) >= (4, 0):
        return on_path

    for p in _clion_cmake_candidates():
        if (_cmake_version(str(p)) or (0, 0)) >= (4, 0):
            return str(p)

    return on_path or "cmake"


CMAKE = find_cmake()


# ----------------------------------------------------------------------------
# Raw keyboard input (arrow keys + enter)
# ----------------------------------------------------------------------------

def read_key():
    if os.name == "nt":
        import msvcrt
        ch = msvcrt.getwch()
        if ch in ("\x00", "\xe0"):
            return {"H": "up", "P": "down", "K": "left", "M": "right"}.get(msvcrt.getwch(), ch)
        if ch == "\r":
            return "enter"
        if ch == " ":
            return "space"
        if ch == "\x03":
            return "ctrl-c"
        return ch

    import termios
    import tty
    fd = sys.stdin.fileno()
    old = termios.tcgetattr(fd)
    try:
        tty.setraw(fd)
        ch = sys.stdin.read(1)
        if ch == "\x1b":
            seq = sys.stdin.read(2)
            return {"[A": "up", "[B": "down", "[C": "right", "[D": "left"}.get(seq, "esc")
        if ch in ("\r", "\n"):
            return "enter"
        if ch == " ":
            return "space"
        if ch == "\x03":
            return "ctrl-c"
        return ch
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old)


def clear():
    sys.stdout.write("\033[2J\033[H")
    sys.stdout.flush()


def draw_menu(title, options, cursor, checked=None, toggleable=None):
    lines = [f"{BOLD}{title}{RESET}", ""]
    for i, opt in enumerate(options):
        marker = ">" if i == cursor else " "
        if checked is not None and i in toggleable:
            box = "[x]" if i in checked else "[ ]"
            lines.append(f"  {marker} {box} {opt}")
        else:
            lines.append(f"  {marker}     {opt}")
    lines.append("")
    if checked is not None:
        lines.append("  Up/Down: move   Space/Enter: toggle   Enter on Proceed/Back: confirm")
    else:
        lines.append("  Up/Down: move   Enter: select")
    return "\n".join(lines)


def menu_select(title, options):
    cursor = 0
    while True:
        clear()
        print(draw_menu(title, options, cursor))
        key = read_key()
        if key == "up":
            cursor = (cursor - 1) % len(options)
        elif key == "down":
            cursor = (cursor + 1) % len(options)
        elif key == "enter":
            return cursor
        elif key == "ctrl-c":
            sys.exit(130)


def menu_packages():
    options = PACKAGE_SELECTABLE + ["Proceed", "Back"]
    n_packages = len(PACKAGE_SELECTABLE)
    toggleable = set(range(n_packages))
    cursor = 0
    checked = set()
    while True:
        clear()
        print(draw_menu("Select packages to update", options, cursor, checked, toggleable))
        key = read_key()
        if key == "up":
            cursor = (cursor - 1) % len(options)
        elif key == "down":
            cursor = (cursor + 1) % len(options)
        elif key == "space":
            if cursor in toggleable:
                checked.symmetric_difference_update({cursor})
        elif key == "enter":
            if cursor in toggleable:
                checked.symmetric_difference_update({cursor})
            elif options[cursor] == "Proceed":
                return ("proceed", [options[i] for i in sorted(checked)])
            else:
                return ("back", None)
        elif key == "ctrl-c":
            sys.exit(130)


def wait_for_enter():
    print(f"\n{BOLD}Done. Press Enter to return to the menu.{RESET}")
    while True:
        key = read_key()
        if key == "enter":
            return
        if key == "ctrl-c":
            sys.exit(130)


# ----------------------------------------------------------------------------
# Target / path helpers
# ----------------------------------------------------------------------------

def select_target():
    idx = menu_select("Select target platform", TARGETS)
    return TARGETS[idx]


def target_system(target):
    return "Windows" if target == "Windows" else "Linux"


def binary_path(target, base):
    ext = ".exe" if target == "Windows" else ""
    return BIN_DIR / f"{target_system(target)}-x64" / f"{base}{ext}"


def packed_dir(target):
    return ROOT / f"packedBuild-{target}"


def check_host(target):
    if target_system(target) != HOST:
        warn(f"Target '{target}' does not match this machine's OS '{HOST}'.")
        warn("No cross-compilation toolchain is configured, so compilation is not possible.")
        return False
    return True


def infer_target():
    existing = [t for t in ("Linux", "Windows") if packed_dir(t).exists()]
    if not existing:
        return None
    if len(existing) == 1:
        return existing[0]
    if HOST in existing:
        return HOST
    return "Linux"


def resolve_target():
    target = infer_target()
    if target is None:
        warn("No packed build found. Run a Full or Incremental build first.")
        return None
    info(f"Detected target: {target}")
    return target


# ----------------------------------------------------------------------------
# Build helpers
# ----------------------------------------------------------------------------

def configure(build_dir, config):
    args = [CMAKE, "-S", str(ROOT), "-B", str(build_dir), f"-DCMAKE_BUILD_TYPE={config}"]
    if not (build_dir / "CMakeCache.txt").exists() and shutil.which("ninja"):
        args += ["-G", "Ninja"]
    print(f"Configuring the {config} build ...")
    return subprocess.run(args, cwd=ROOT).returncode == 0


def build(build_dir, config, clean):
    args = [CMAKE, "--build", str(build_dir), "--config", config]
    if clean:
        args.append("--clean-first")
    print(f"Compiling the {config} build{' from scratch' if clean else ''} ...")
    return subprocess.run(args, cwd=ROOT).returncode == 0


# ----------------------------------------------------------------------------
# Copy helpers
# ----------------------------------------------------------------------------

def wipe(packed):
    if packed.exists():
        shutil.rmtree(packed)
    packed.mkdir(parents=True)


def copy_licenses(packed):
    src = GAME_DATA / "Licenses"
    if src.exists():
        shutil.copytree(src, packed / "Licenses", dirs_exist_ok=True)
        info("Copied Licenses")
    else:
        warn("Missing GameData/Licenses, skipping.")


def copy_steam_and_libs(packed, target):
    src = GAME_DATA / "steam_appid.txt"
    if src.exists():
        shutil.copy2(src, packed / "steam_appid.txt")
        info("Copied steam_appid.txt")
    else:
        warn("Missing GameData/steam_appid.txt, skipping.")

    if target == "Windows":
        dlls = sorted(GAME_DATA.glob("*.dll"))
        if dlls:
            for dll in dlls:
                shutil.copy2(dll, packed / dll.name)
            info("Copied all DLL files")
        else:
            warn("No DLL files found in GameData, skipping.")
    else:
        src = GAME_DATA / "libsteam_api.so"
        if src.exists():
            shutil.copy2(src, packed / "libsteam_api.so")
            info("Copied libsteam_api.so")
        else:
            warn("Missing GameData/libsteam_api.so, skipping.")


def copy_data(packed):
    data = packed / "Data"
    data.mkdir(parents=True, exist_ok=True)
    for name in DATA_DIRS:
        src = GAME_DATA / "Data" / name
        if src.exists():
            shutil.copytree(src, data / name, dirs_exist_ok=True)
            info(f"Copied Data/{name}")
        else:
            warn(f"Missing GameData/Data/{name}, skipping.")
    copy_packages_full(data)


def copy_packages_full(data):
    src = GAME_DATA / "Data" / "Packages"
    dst = data / "Packages"
    dst.mkdir(parents=True, exist_ok=True)

    missing = []
    for name in PACKAGE_YAMLS:
        f = src / f"{name}.yaml"
        if f.exists():
            shutil.copy2(f, dst / f.name)
        else:
            missing.append(f"{name}.yaml")

    for pkg in sorted(src.glob("*.pkg")):
        shutil.copy2(pkg, dst / pkg.name)

    if missing:
        warn("The following package files are missing:")
        for m in missing:
            warn(f"  - {m}")

    info("Copied Data/Packages")


def copy_packages_selected(data, names):
    src = GAME_DATA / "Data" / "Packages"
    dst = data / "Packages"
    dst.mkdir(parents=True, exist_ok=True)

    missing = []
    for name in names:
        for ext in (".yaml", ".pkg"):
            f = src / f"{name}{ext}"
            if f.exists():
                shutil.copy2(f, dst / f.name)
            else:
                missing.append(f"{name}{ext}")

    if missing:
        warn("The following package files are missing:")
        for m in missing:
            warn(f"  - {m}")

    info(f"Copied packages: {', '.join(names)}")


def copy_sectors(packed):
    src = GAME_DATA / "Data" / "Sectors"
    dst = packed / "Data" / "Sectors"
    if src.exists():
        dst.mkdir(parents=True, exist_ok=True)
        shutil.copytree(src, dst, dirs_exist_ok=True)
        info("Copied Data/Sectors")
    else:
        warn("Missing GameData/Data/Sectors, skipping.")


def copy_shaders(packed):
    src = GAME_DATA / "Data" / "Shaders"
    dst = packed / "Data" / "Shaders"
    if src.exists():
        dst.mkdir(parents=True, exist_ok=True)
        shutil.copytree(src, dst, dirs_exist_ok=True)
        info("Copied Data/Shaders")
    else:
        warn("Missing GameData/Data/Shaders, skipping.")


def copy_binary(packed, target):
    pkg_bin = binary_path(target, "Afterlife_Engine-Package")
    if pkg_bin.exists():
        shutil.copy2(pkg_bin, packed / pkg_bin.name)
        info(f"Copied {pkg_bin.name}")
    else:
        warn(f"Packaging binary not found at {pkg_bin}, skipping.")


def copy_full(target):
    packed = packed_dir(target)
    wipe(packed)
    info(f"Creating {packed.name} ...")
    copy_licenses(packed)
    copy_steam_and_libs(packed, target)
    copy_data(packed)
    copy_binary(packed, target)


# ----------------------------------------------------------------------------
# Operations
# ----------------------------------------------------------------------------

def op_full_build():
    target = select_target()
    if target in STUB_TARGETS:
        warn(f"Compilation information for {target} is not available for now.")
        return
    if not check_host(target):
        return

    section("Testing build")
    if not configure(BUILD_TESTING, CONFIG_TESTING) or not build(BUILD_TESTING, CONFIG_TESTING, clean=True):
        err("Testing build failed, aborting packaging.")
        return

    test_bin = binary_path(target, "Afterlife_Engine-Testing")
    if not test_bin.exists():
        err(f"Testing binary not found at {test_bin}, aborting packaging.")
        return
    print("Running the testing build ...")
    result = subprocess.run([str(test_bin)], cwd=ROOT)
    if result.returncode != 0:
        err(f"Testing build returned non-zero exit code {result.returncode}, aborting packaging.")
        return
    ok("Testing build passed.")

    section("Packaging build")
    if not configure(BUILD_PACKAGING, CONFIG_PACKAGING) or not build(BUILD_PACKAGING, CONFIG_PACKAGING, clean=True):
        err("Packaging build failed, aborting packaging.")
        return

    copy_full(target)
    ok(f"Full build packaged into packedBuild-{target}.")


def op_incremental_build():
    target = select_target()
    if target in STUB_TARGETS:
        warn(f"Compilation information for {target} is not available for now.")
        return
    if not check_host(target):
        return

    section("Packaging build")
    if not configure(BUILD_PACKAGING, CONFIG_PACKAGING) or not build(BUILD_PACKAGING, CONFIG_PACKAGING, clean=True):
        err("Packaging build failed, aborting packaging.")
        return

    copy_full(target)
    ok(f"Incremental build packaged into packedBuild-{target}.")


def op_executable_update():
    target = resolve_target()
    if target is None:
        return
    if not check_host(target):
        return

    section("Packaging build")
    if not configure(BUILD_PACKAGING, CONFIG_PACKAGING) or not build(BUILD_PACKAGING, CONFIG_PACKAGING, clean=False):
        err("Packaging build failed, aborting update.")
        return

    copy_binary(packed_dir(target), target)
    ok("Executable updated.")


def op_data_update():
    target = resolve_target()
    if target is None:
        return

    copy_data(packed_dir(target))
    ok("Data updated.")


def op_sector_update():
    target = resolve_target()
    if target is None:
        return

    copy_sectors(packed_dir(target))
    ok("Sectors updated.")


def op_shader_update():
    target = resolve_target()
    if target is None:
        return

    copy_shaders(packed_dir(target))
    ok("Shaders updated.")


def op_package_update():
    target = resolve_target()
    if target is None:
        return

    packed = packed_dir(target)
    while True:
        action, selected = menu_packages()
        if action == "back":
            return
        if not selected:
            warn("Nothing selected, try again.")
            continue
        copy_packages_selected(packed / "Data", selected)
        ok("Package update complete.")
        return


# ----------------------------------------------------------------------------
# Entry point
# ----------------------------------------------------------------------------

def main():
    options = [
        "Full build",
        "Incremental build",
        "Executable update",
        "Data update",
        "Sector update",
        "Shader update",
        "Package update",
        "Quit",
    ]
    operations = [
        op_full_build,
        op_incremental_build,
        op_executable_update,
        op_data_update,
        op_sector_update,
        op_shader_update,
        op_package_update,
    ]

    while True:
        idx = menu_select("Afterlife Packaging Tool", options)
        if idx == len(operations):
            return
        operations[idx]()
        wait_for_enter()


if __name__ == "__main__":
    main()
