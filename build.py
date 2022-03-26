from __future__ import annotations

import argparse
import atexit
from concurrent.futures import ThreadPoolExecutor
import functools
import json
import logging
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

from compiler import AllocationMap, BuildKind, CompilerFactory, CompilerKind, Define, KernelKind, Region
from prmparser import PrmFile

TMPDIR = Path("tmp-compiler")


@atexit.register
def clean_resources():
    if TMPDIR.is_dir():
        pass  # shutil.rmtree(__TMPDIR)


class BootType(str, Enum):
    DOL = "DOL"
    ISO = "ISO"
    NONE = "NONE"


class IndentedFormatter(logging.Formatter):
    IndentionWidth = 4

    def format(self, record):
        """Format a record with added indentation."""
        indent = " " * self.IndentionWidth
        head, *trailing = super().format(record).splitlines(True)
        return head + ''.join(indent + line for line in trailing)


class FilePatcher:
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
        optimize: str = "-O2",
        shines: int = 120,
        logger: Optional[logging.Logger] = None
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
        
        if logger is None:
            logger = logging.Logger("Default-Logger")

        self.compiler = CompilerFactory.create(
            patcher,
            compiler,
            startAddr,
            optimize,
            buildKind,
            region,
            logger
        )

    @property
    def logger(self) -> logging.Logger:
        return self.compiler.logger

    @logger.setter
    def logger(self, logger: logging.Logger):
        self.compiler.logger = logger

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
        return self.compiler.is_release()

    def is_debug(self) -> bool:
        return self.compiler.is_debug()

    def is_booting(self) -> bool:
        return self.bootType != BootType.NONE

    def is_iso_boot(self) -> bool:
        return self.bootType == BootType.ISO

    def is_dol_boot(self) -> bool:
        return self.bootType == BootType.DOL

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
                self.logger.info("Attempting to boot Dolphin by DOL at %s...", dolphin)
                subprocess.Popen(
                    f"\"{dolphin}\" -e \"{self.gameDir / 'sys/main.dol'}\" " + " ".join(options), shell=True)
            elif self.is_iso_boot():
                isoPath = self._build_iso(config)
                self.logger.info("Attempting to boot Dolphin by ISO at %s...", dolphin)
                subprocess.Popen(
                    f"\"{dolphin}\" -e \"{isoPath}\" " + " ".join(options), shell=True)

    def _build_iso(self, config: dict) -> Path:
        isoPath = Path(config["buildpath"])

        self.logger.info("%s -> %s", self.gameDir, isoPath)

        iso = GamecubeISO.from_root(self.gameDir, True)
        iso.build(isoPath, preCalc=False)

        return isoPath

    def _patch_dol(self) -> bool:
        srcDolPath = self.solutionRegionDir / "system/main.dol"
        destDolPath = self.gameDir / "sys/main.dol"
        modulesDest = self.gameDir / \
            ("files/Kuribo!/Mods/" if self.compiler.is_kuribo() else "files/")

        if srcDolPath.exists():
            if self.is_release():
                self.logger.info("Generating %d shines RELEASE build", self.maxShines)
            elif self.is_debug():
                self.logger.info("Generating %d shines DEBUG build", self.maxShines)

            with srcDolPath.open("rb") as dol:
                _dolData = DolFile(dol)

            modules = self.compiler.run(Path("src"), _dolData)
            with ThreadPoolExecutor() as executor:
                # Rename all modules to be in the Kuribo! folder
                # executor.map(lambda f: os.rename(*f), [(str(m), str(modulesDest / m.name)) for m in modules])
                for m in modules:
                    os.unlink(modulesDest / m.name)
                    self.logger.info("Moving module %s to %s", m.name, modulesDest / m.name)
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

    def _replace_files_from_config(self, solutionPath: Path):
        REPLACE_LOG = ""
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
                    REPLACE_LOG += f"{relativePath} -> No destination found\n"
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

            REPLACE_LOG += f"{relativePath} -> {destPath}\n"

        if REPLACE_LOG.strip() != "":
            self.logger.info(REPLACE_LOG.strip())

    def _rename_files_from_config(self, solutionPath: Path):
        RENAME_LOG = ""
        _renamed = []

        with Path(solutionPath / ".config.json").open("r") as f:
            config = json.load(f)

        for f in self.gameDir.rglob("*"):
            for path in config["rename"]:
                rename = config["rename"][path]

                if fnmatch(f, self.gameDir / path):
                    RENAME_LOG += f"{f.relative_to(self.gameDir)} -> {f.parent / rename}\n"
                    f.rename(f.parent / rename)
                    _renamed.append(f)

        if RENAME_LOG.strip() != "":
            self.logger.info(RENAME_LOG.strip())

    def _delete_files_from_config(self, solutionPath: Path):
        DELETE_LOG = ""
        with Path(solutionPath / ".config.json").open("r") as f:
            config = json.load(f)

        for f in self.gameDir.rglob("*"):
            for path in config["delete"]:
                if fnmatch(f, self.gameDir / path):
                    DELETE_LOG += f"{f} -> DELETED\n"
                    f.unlink()

        if DELETE_LOG.strip() != "":
            self.logger.info(DELETE_LOG.strip())

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
        if not path.exists():
            return

        bnr = BNR(path, BNR.Regions.AMERICA)
        bnr.save_bnr(self.gameDir / "files/opening.bnr")

    def _create_signatures(self, dol: DolFile, name: str, encoding: str = "ascii"):
        for section in dol.sections:
            while (offset := section.data.getvalue().find("Super Mario Sunshine".encode(encoding))) != -1:
                section.data.seek(offset)
                section.data.write(name.encode(encoding))

def main():
    parser = argparse.ArgumentParser(
        "SMS-Patcher", description="C++ Patcher for SMS NTSC-U, using Kamek by Treeki")

    parser.add_argument("gamefolder", help="root folder of GCR extracted ISO")
    parser.add_argument("-p", "--projectfolder",
                        help="project folder used to patch game", metavar="PATH")
    parser.add_argument("-s", "--startaddr", help="Starting address for the linker and code",
                        default="0x80000000", type=lambda x: int(x, 0), metavar="ADDR")
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
                        choices=[BootType.DOL, BootType.ISO, BootType.NONE], default=BootType.NONE)
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
        logger = logging.Logger(args.out.split(".")[0].upper())
        logFormatter = IndentedFormatter(
            fmt="%(name)s [%(funcName)s] | %(asctime)s | %(levelname)s:\n%(message)s\n",
            datefmt="%m-%d-%Y %H:%M:%S"
        )
        fhandle = logging.FileHandler(args.out)
        fhandle.setFormatter(logFormatter)
        logger.addHandler(fhandle)
        fhandle.stream.write(f"== {logger.name} - NEW SESSION ==\n\n")
    else:
        logger = None

    patcher = FilePatcher(
        CompilerKind(args.compiler),
        KernelKind(args.patcher),
        build,
        args.gamefolder,
        args.projectfolder,
        args.region,
        BootType(args.boot),
        args.startaddr,
        f"-O{args.optimize_level}",
        args.shines,
        logger
    )
    patcher.patch_game()


if __name__ == "__main__":
    main()
