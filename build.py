from __future__ import annotations

import argparse
import atexit
from concurrent.futures import ThreadPoolExecutor
import functools
import json
from multiprocessing.pool import ThreadPool
import os
import shutil
import subprocess
import sys
import time
from enum import Enum
from fnmatch import fnmatch
from io import BytesIO, StringIO
from pathlib import Path
from typing import IO, Any, Dict, Generator, List, Optional, Union
from pathos.multiprocessing import ProcessPool

import oead
import psutil
from dolreader.dol import DolFile
from dolreader.section import TextSection
from pyisotools.bnrparser import BNR
from pyisotools.iso import GamecubeISO

from compiler import AllocationMap, BuildKind, CompilerFactory, CompilerKind, Define, KernelKind
from prmparser import PrmFile

__TMPDIR = Path("tmp-compiler")


@atexit.register
def clean_resources():
    if __TMPDIR.is_dir():
        pass  # shutil.rmtree(__TMPDIR)


def wrap_printer(msg: str = "") -> function:
    def decorater_inner(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            print("")
            if len(msg.strip()) > 0:
                print(f"====== {msg} ======".center(128))
            print("-"*128)
            value = func(*args, **kwargs)
            print("-"*128)
            return value
        return wrapper
    return decorater_inner


class Region(str, Enum):
    US = "US"
    EU = "EU"
    JP = "JP"
    KR = "KR"
    ANY = "ANY"


class FileStream(object):
    def __init__(self, path: str, stream: IO):
        self.path = path
        self.stream = stream

    def __del__(self) -> None:
        if not self.stream.closed:
            self.stream.close()


class FilePatcher:
    class BootType(str, Enum):
        DOL = "DOL"
        ISO = "ISO"
        NONE = "NONE"

    MainAllocationMap = AllocationMap()

    def __init__(
        self,
        compiler: CompilerKind,
        patcher: KernelKind,
        buildKind: BuildKind,
        gameDir: str,
        projectDir: str = Path.cwd(),
        region: Region = Region.US,
        bootfrom: BootType = BootType.DOL,
        startAddr: int = 0x80000000,
        shines: int = 120
    ):

        if isinstance(startAddr, str):
            startAddr = int(startAddr, 16)

        self.projectDir = Path(projectDir)
        self.gameDir = Path(gameDir)
        self.buildKind = buildKind
        self.maxShines = shines
        self.bootType = bootfrom
        self.region = region

        self._fileTables = {}
        self._init_tables()

        print(
            self._get_matching_filepath(
                self.solutionRegionDir / "main.dol"
            )
        )

        self.compiler = CompilerFactory.create(
            patcher,
            compiler,
            startAddr,
            buildKind,
            region
        )

    @property
    def solutionRegionDir(self) -> Path:
        if self.is_release():
            return self.projectDir / "assets/bin/release" / self.region.lower()
        elif self.is_debug():
            return self.projectDir / "assets/bin/debug" / self.region.lower()

    @property
    def solutionAnyDir(self) -> Path:
        if self.is_release():
            return self.projectDir / "assets/bin/release/any"
        elif self.is_debug():
            return self.projectDir / "assets/bin/debug/any"

    def is_release(self) -> bool:
        return self.buildKind in {BuildKind.RELEASE, BuildKind.RELEASE_DEBUG}

    def is_debug(self) -> bool:
        return self.buildKind == BuildKind.DEBUG

    def is_booting(self) -> bool:
        return self.bootType != FilePatcher.BootType.NONE

    def is_iso_boot(self) -> bool:
        return self.bootType == FilePatcher.BootType.ISO

    def is_dol_boot(self) -> bool:
        return self.bootType == FilePatcher.BootType.DOL

    def is_ignored(self, path: Path) -> bool:
        with Path(self.solutionRegionDir / ".config.json").open("r") as f:
            config = json.load(f)

        for glob in config["ignore"]:
            if fnmatch(str(path).lower(), glob.strip().lower()):
                return True

        with Path(self.solutionAnyDir / ".config.json").open("r") as f:
            config = json.load(f)

        for glob in config["ignore"]:
            if fnmatch(str(path).lower(), glob.strip().lower()):
                return True

        return False

    def by_region(self, ntscu, pal, ntscj, ntsck) -> Any:
        if self.region == Region.US:
            return ntscu
        elif self.region == Region.EU:
            return pal
        elif self.region == Region.JP:
            return ntscj
        elif self.region == Region.KR:
            return ntsck
        return None

    def patch_game(self):
        with (self.solutionRegionDir / ".config.json").open("r") as f:
            config = json.load(f)

        with ThreadPool() as pp:
            proc = pp.apply_async(self._patch_dol)

            self._replace_files_from_config(self.solutionAnyDir)
            self._replace_files_from_config(self.solutionRegionDir)
            self._delete_files_from_config(self.solutionAnyDir)
            self._delete_files_from_config(self.solutionRegionDir)
            self._rename_files_from_config(self.solutionAnyDir)
            self._rename_files_from_config(self.solutionRegionDir)

            patched = proc.get()

        if patched and self.is_booting():
            for proc in psutil.process_iter():
                if proc.name() == "Dolphin.exe":
                    proc.kill()
                    while proc.is_running():
                        time.sleep(0.01)

            if self.is_release():
                options = ("-C Dolphin.General.DebugModeEnabled=True",
                           "-C Dolphin.General.OnScreenDisplayMessages=True",
                           "-C Dolphin.General.UsePanicHandlers=True",
                           "-C Dolphin.Core.CPUCore=1",
                           "-C Dolphin.Core.Fastmem=True",
                           "-C Dolphin.Core.CPUThread=False",
                           "-C Dolphin.Core.MMU=True",
                           "-C Logger.Logs.OSREPORT=True",
                           "-C Logger.Options.WriteToConsole=True",
                           "-C Logger.Options.WriteToWindow=True")
            else:
                options = ("-C Dolphin.General.ShowLag=True",
                           "-C Dolphin.General.ExtendedFPSInfo=True",
                           "-C Dolphin.General.DebugModeEnabled=True",
                           "-C Dolphin.General.OnScreenDisplayMessages=True",
                           "-C Dolphin.General.UsePanicHandlers=True",
                           "-C Dolphin.Core.CPUCore=1",
                           "-C Dolphin.Core.Fastmem=True",
                           "-C Dolphin.Core.CPUThread=False",
                           "-C Dolphin.Core.MMU=True",
                           "-C Logger.Logs.OSREPORT=True",
                           "-C Logger.Options.WriteToConsole=True",
                           "-C Logger.Options.WriteToWindow=True")

            dolphin = Path(config["dolphinpath"])

            if self.is_dol_boot():
                print(f"\nAttempting to boot Dolphin by DOL at {dolphin}...\n")
                subprocess.Popen(
                    f"\"{dolphin}\" -e \"{self.dest}\" " + " ".join(options), shell=True)
            elif self.is_iso_boot():
                print(f"\nAttempting to boot Dolphin by ISO at {dolphin}...\n")
                isoPath = self._build_iso(config)
                subprocess.Popen(
                    f"\"{dolphin}\" -e \"{isoPath}\" " + " ".join(options), shell=True)

    @wrap_printer("ISO BUILDING")
    def _build_iso(self, config: dict) -> Path:
        isoPath = Path(config["buildpath"])

        print(f"{self.gameDir} -> {isoPath}")

        iso = GamecubeISO.from_root(self.gameDir, True)
        iso.build(isoPath, preCalc=False)

        print("-"*128)
        return isoPath

    @wrap_printer("DOL PATCHING")
    def _patch_dol(self) -> bool:
        srcDolPath = self.solutionRegionDir / "system/main.dol"
        destDolPath = self.gameDir / "sys/main.dol"
        modulesDest = self.gameDir / \
            ("files/Kuribo!/Mods/" if self.compiler.is_kuribo() else "files/")

        if srcDolPath.exists():
            if self.is_release():
                print(f"Generating {self.maxShines} shines RELEASE build")
            elif self.is_debug():
                print(f"Generating {self.maxShines} shines DEBUG build")

            with srcDolPath.open("rb") as dol:
                _dolData = DolFile(dol)

            modules = self.compiler.run(Path("src"), _dolData)
            with ThreadPoolExecutor() as executor:
                # Rename all modules to be in the Kuribo! folder
                # executor.map(lambda f: os.rename(*f), [(str(m), str(modulesDest / m.name)) for m in modules])
                for m in modules:
                    os.unlink(modulesDest / m.name)
                    m.rename(modulesDest / m.name)

                self._create_signatures(_dolData, "Super Mario Eclipse\0")

                if self.compiler.is_kuribo():
                    tmpbin = Path(
                        "assets/bin", f"kuriboloader-{self.region.lower()}.bin")

                    data = BytesIO(tmpbin.read_bytes())
                    rawData = data.getvalue()
                    injectaddr = (int.from_bytes(rawData[
                        :4], "big", signed=False) & 0x1FFFFFC) | 0x80000000
                    codelen = int.from_bytes(
                        rawData[4:8], "big", signed=False) * 8

                    blockstart = 0x80004A00
                    if not _dolData.is_mapped(blockstart):
                        loaderSection = TextSection(blockstart, rawData[8:])
                        _dolData.append_section(loaderSection)
                    else:
                        _dolData.seek(blockstart)
                        _dolData.write(rawData[8:])
                    _dolData.insert_branch(blockstart, injectaddr)
                    _dolData.insert_branch(
                        injectaddr + 4, blockstart + (codelen - 4))

                _dolData.write_uint32(self.by_region(
                    0x802A73F0, 0, 0, 0), 0x60000000)
                _dolData.write_uint32(self.by_region(
                    0x802A7404, 0, 0, 0), 0x60000000)

                with destDolPath.open("wb") as dest:
                    _dolData.save(dest)

            return True
        return False

    @wrap_printer("REPLACING | COPYING")
    def _replace_files_from_config(self, solutionPath: Path):
        with Path(solutionPath / ".config.json").open("r") as f:
            config = json.load(f)

        bnrPath = (solutionPath / config["gamebanner"]).resolve()

        self._compile_bnr_to_game(bnrPath)
        destPath = self.gameDir / "opening.bnr"

        for f in solutionPath.rglob("*"):
            relativePath = f.relative_to(Path.cwd())

            if f.name.lower() == ".config.json":
                continue

            destPath = self._get_matching_filepath(f)

            if destPath is None:
                if f.is_file() and self.is_ignored(f.relative_to(solutionPath)):
                    print(f"{relativePath} -> No destination found")
                continue

            if not destPath.parent.exists():
                destPath.parent.mkdir(parents=True, exist_ok=True)

            if f.suffix.lower() in (".szs", ".arc"):
                destPath = destPath.with_suffix(".szs")
                with f.open("rb") as archive:
                    data = BytesIO(archive.read())

                data.seek(0)
                if data.read(4) != b"Yaz0":
                    data = oead.yaz0.compress(data.getvalue())
                    destPath.write_bytes(data)
                else:
                    destPath.write_bytes(f.read_bytes())
            elif f.parent.name.lower() == "smeconfigs" and f.suffix == ".txt":
                prm = PrmFile.from_text(f.read_text())
                destPath.with_suffix(".sme").write_bytes(prm.to_bytes())
            else:
                destPath.write_bytes(f.read_bytes())

            print(f"{relativePath} -> {destPath}")

    @wrap_printer("RENAMING")
    def _rename_files_from_config(self, solutionPath: Path):
        _renamed = []

        with Path(solutionPath / ".config.json").open("r") as f:
            config = json.load(f)

        for f in self.gameDir.rglob("*"):
            for path in config["rename"]:
                rename = config["rename"][path]

                if fnmatch(f, self.gameDir / path):
                    print(f"{f.relative_to(self.gameDir)} -> {f.parent / rename}")
                    f.rename(f.parent / rename)
                    _renamed.append(f)

    @wrap_printer("DELETING")
    def _delete_files_from_config(self, solutionPath: Path):
        with Path(solutionPath / ".config.json").open("r") as f:
            config = json.load(f)

        for f in self.gameDir.rglob("*"):
            for path in config["delete"]:
                if fnmatch(f, self.gameDir / path):
                    print(f"{f} -> DELETED")
                    f.unlink()

    def _get_translated_filepath(self, relativePath: Union[str, Path]) -> Path:
        return self.gameDir / relativePath

    def _init_tables(self):
        for f in self.gameDir.rglob("*"):
            if f.is_file():
                self._fileTables.setdefault(f.suffix, []).append(f)

    def _get_matching_filepath(self, path: Path) -> Path:
        retpath = self._get_path_from_config(self.solutionRegionDir, path)
        if retpath:
            return retpath
        retpath = self._get_path_from_config(self.solutionAnyDir, path)
        if retpath:
            return retpath

        try:
            for f in self._fileTables[path.suffix]:
                if f.name == path.name:
                    return self.gameDir / f
        except KeyError:
            return None

    def _get_path_from_config(self, solutionPath: Path, path: Path) -> Path:
        if self.is_release():
            parentGlob = f"*/release/{solutionPath.name}/"
        elif self.is_debug():
            parentGlob = f"*/debug/{solutionPath.name}/"
        else:
            raise ValueError("Invalid State!")

        with Path(solutionPath / ".config.json").open("r") as f:
            config = json.load(f)

        for _set in config["userfiles"]:
            glob = list(_set.keys())[0]
            if fnmatch(str(path).lower(), parentGlob + glob.strip().lower()):
                if _set[glob]["rename"].strip() == "":
                    return self.gameDir / _set[glob]["destination"].strip() / path.name
                else:
                    return self.gameDir / _set[glob]["destination"].strip() / _set[glob]["rename"].strip()

        return None

    def _compile_bnr_to_game(self, path: Path):
        print(path)
        if not path.exists():
            return

        bnr = BNR(path, BNR.Regions.AMERICA)
        bnr.save_bnr(self.gameDir / "files/opening.bnr")

    def _create_signatures(self, dol: DolFile, name: str, encoding: str = "ascii"):
        for section in dol.sections:
            while (offset := section.data.getvalue().find("Super Mario Sunshine".encode(encoding))) != -1:
                section.data.seek(offset)
                section.data.write(name.encode(encoding))

    def _alloc_from_heap(self, dol: DolFile, size: int):
        size = (size + 31) & -32

        for packet in _ALLOC_LO_INFO.group(self.region):
            dol.seek(packet.address)
            for instr in packet.instructions:
                dol.write_uint32(
                    dol.tell(), instr.as_translated(self.startaddr + size))

        for packet in _ALLOC_HI_INFO.group(self.region):
            return
            dol.seek(packet.address)
            for instr in packet.instructions:
                dol.write_uint32(
                    dol.tell(), instr.as_translated(0x211000))


def main():
    parser = argparse.ArgumentParser(
        "SMS-Patcher", description="C++ Patcher for SMS NTSC-U, using Kamek by Treeki")

    parser.add_argument("gamefolder", help="root folder of GCR extracted ISO")
    parser.add_argument("-p", "--projectfolder",
                        help="project folder used to patch game", metavar="PATH")
    parser.add_argument("-s", "--startaddr", help="Starting address for the linker and code",
                        default=0x80000000, type=lambda x: int(x, 0), metavar="ADDR")
    parser.add_argument("-b", "--build", help="Build type; R=Release, D=Debug",
                        choices=["R", "D", "RD"], default="D")
    parser.add_argument("-c", "--compiler", choices=[
                        CompilerKind.CODEWARRIOR, CompilerKind.CLANG, CompilerKind.GCC],
                        default=CompilerKind.CODEWARRIOR, help="Compiler to build with")
    parser.add_argument("-o", "--optimize-level", help="Optimization strength",
                        default="2")
    parser.add_argument("-r", "--region", help="Game region",
                        choices=[Region.US, Region.EU, Region.JP, Region.KR], default=Region.US)
    parser.add_argument("-P", "--patcher", help="Game patcher",
                        choices=[KernelKind.KAMEK, KernelKind.KURIBO], default=KernelKind.KAMEK)
    parser.add_argument("--boot", help="What to boot from",
                        choices=[FilePatcher.BootType.DOL, FilePatcher.BootType.ISO, FilePatcher.BootType.NONE], default=FilePatcher.BootType.NONE)
    parser.add_argument(
        "--shines", help="Max shines allowed", type=int, default=120)
    parser.add_argument("--out", help="File to output to")

    args = parser.parse_args()

    if args.shines < 0 or args.shines > 999:
        parser.error(
            f"Shine count is beyond the inclusive range (0 - 999): {args.shines}")

    if not args.projectfolder:
        args.projectfolder = Path.cwd()

    if not args.startaddr:
        args.startaddr = 0x80000000

    if args.build == "D":
        build = BuildKind.DEBUG
    elif args.build == "RD":
        build = BuildKind.RELEASE_DEBUG
    else:
        build = BuildKind.RELEASE

    if args.out:
        out = FileStream(args.out, open(args.out, "w"))
    else:
        out = None

    patcher = FilePatcher(
        CompilerKind(args.compiler),
        KernelKind(args.patcher),
        build,
        args.gamefolder,
        args.projectfolder,
        args.region,
        FilePatcher.BootType(args.boot),
        args.startaddr,
        args.shines
    )

    sys.stdout = out.stream
    sys.stderr = out.stream

    patcher.patch_game()

    sys.stdout = sys.__stdout__
    sys.stderr = sys.__stderr__


if __name__ == "__main__":
    main()
