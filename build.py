from __future__ import annotations

import argparse
import atexit
import functools
import json
import shutil
import subprocess
import sys
import time
from fnmatch import fnmatch
from io import BytesIO, StringIO
from pathlib import Path
from typing import IO, Any, Dict, Generator, List, Optional, Union

import oead
import psutil
from dolreader.dol import DolFile
from dolreader.section import TextSection
from pyisotools.bnrparser import BNR
from pyisotools.iso import GamecubeISO
from prmparser import PrmFile

from compiler import Compiler, Define

TMPDIR = Path("tmp-compiler")


@atexit.register
def clean_resources():
    if TMPDIR.is_dir():
        shutil.rmtree(TMPDIR)


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


class Region:
    US = "US"
    EU = "EU"
    JP = "JP"
    KR = "KR"
    ANY = "ANY"


class Patcher:
    KAMEK = "KAMEK"
    KURIBO = "KURIBO"


class AllocationMap(object):
    class RelocationType:
        HI = 0
        LO = 1
        NONE = -1

    class InstructionInfo(object):
        def __init__(self, value: int, relocationType: AllocationMap.RelocationType, relmask: int = 0xFFFF):
            self.value = value
            self.reltype = relocationType
            self._relmask = relmask

        def __str__(self) -> str:
            return f"0x{self.value:08X} (Mask ({self.reltype}): 0x{self._relmask:08X})"

        def is_rel_hi(self) -> bool:
            return self.reltype == AllocationMap.RelocationType.HI

        def is_rel_lo(self) -> bool:
            return self.reltype == AllocationMap.RelocationType.LO

        def is_rel_none(self) -> bool:
            return self.reltype == AllocationMap.RelocationType.NONE

        def as_translated(self, offset) -> int:
            if self.is_rel_none():
                return self.value

            offset <<= _get_bit_alignment(self._relmask)
            if self.is_rel_hi():
                offset = (offset >> 16) & 0xFFFF
            elif self.is_rel_lo():
                offset &= 0xFFFF
            return self.value + (offset & self._relmask)

        @staticmethod
        def translate(value: int, offset: int, relocationType: AllocationMap.RelocationType, relmask: int = 0xFFFF):
            instr = AllocationMap.InstructionInfo(
                value, relocationType, relmask)
            return instr.as_translated(offset)

    class AllocationPacket(object):
        def __init__(self, address: int, baseInstrs: List[AllocationMap.InstructionInfo]):
            self._address = address
            self._baseInstrs = baseInstrs

        @property
        def instructions(self) -> Generator[AllocationMap.InstructionInfo]:
            for instr in self._baseInstrs:
                yield instr

        @property
        def address(self) -> int:
            return self._address

    def __init__(self, template: Optional[Dict[Region, List[AllocationMap.AllocationPacket]]] = None):
        if template:
            self._template = template
        else:
            self._template = {
                Region.US: [],
                Region.EU: [],
                Region.JP: [],
                Region.KR: []
            }

    def group(self, region: Region) -> List[AllocationMap.AllocationPacket]:
        return self._template[region]


_ALLOC_LO_INFO = AllocationMap({
    Region.US: (AllocationMap.AllocationPacket(0x80341E74, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]),
                AllocationMap.AllocationPacket(0x80341EAC, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)])),
    Region.EU: (AllocationMap.AllocationPacket(0x80341E74, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]),
                AllocationMap.AllocationPacket(0x80341EAC, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)])),
    Region.JP: (AllocationMap.AllocationPacket(0x80341E74, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]),
                AllocationMap.AllocationPacket(0x80341EAC, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)])),
    Region.KR: (AllocationMap.AllocationPacket(0x80341E74, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]),
                AllocationMap.AllocationPacket(0x80341EAC, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)])),
})

_ALLOC_HI_INFO = AllocationMap({
    Region.US: (AllocationMap.AllocationPacket(0x80341EC8, [AllocationMap.InstructionInfo(0x3C000000, AllocationMap.RelocationType.HI),
                                                            AllocationMap.InstructionInfo(
                                                                0x60000000, AllocationMap.RelocationType.LO),
                                                            AllocationMap.InstructionInfo(
                                                                0x7C601850, AllocationMap.RelocationType.NONE),
                                                            AllocationMap.InstructionInfo(
                                                                0x3C808000, AllocationMap.RelocationType.NONE),
                                                            AllocationMap.InstructionInfo(0x90640034, AllocationMap.RelocationType.NONE)]),),
    Region.EU: (AllocationMap.AllocationPacket(0x80341ED0, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]),),
    Region.JP: (AllocationMap.AllocationPacket(0x80341ED0, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]),),
    Region.KR: (AllocationMap.AllocationPacket(0x80341ED0, [AllocationMap.InstructionInfo(
        0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]),)
})

class FileStream(object):
    def __init__(self, path: str, stream: IO):
        self.path = path
        self.stream = stream

    def __del__(self) -> None:
        if not self.stream.closed:
            self.stream.close()

class FilePatcher(Compiler):
    class State:
        DEBUG = "DEBUG"
        RELEASE = "RELEASE"

    class BootType:
        DOL = "DOL"
        ISO = "ISO"
        NONE = "NONE"

    MainAllocationMap = AllocationMap()

    def __init__(self, build: State, gameDir: str, projectDir: str = Path.cwd(),
                 region: Region = Region.US, compiler: Compiler.Compilers = Compiler.Compilers.CODEWARRIOR,
                 patcher: Patcher = Patcher.KAMEK, bootfrom: BootType = BootType.DOL,
                 startAddr: int = 0x80000000, shines: int = 120, fout: Optional[FileStream] = None):

        if isinstance(startAddr, str):
            startAddr = int(startAddr, 16)

        self.projectDir = Path(projectDir)
        self.gameDir = Path(gameDir)
        self.state = build
        self.maxShines = shines
        self.bootType = bootfrom
        self.region = region
        self._fout = fout
        self._fileTables = {}

        self._init_tables()

        print(self._get_matching_filepath(
            self.solutionRegionDir / "main.dol"))

        super().__init__(Path("compiler"),
                         compiler=compiler,
                         dest=self._get_matching_filepath(
                             self.solutionRegionDir / "main.dol"),
                         linker=Path(f"linker/{self.region}.map"),
                         patcher=patcher,
                         dump=False,
                         startaddr=startAddr)

        _defines = []
        if self.is_release():
            _defines.extend(
                (Define("SME_MAX_SHINES", f"{self.maxShines}"), Define("SME_RELEASE")))
        elif self.is_debug():
            _defines.extend(
                (Define("SME_MAX_SHINES", f"{self.maxShines}"), Define("SME_DEBUG")))
        else:
            raise ValueError(f"Unknown patcher state {self.state}")

        if self.is_kuribo():
            _defines.append(Define("SME_BUILD_KURIBO"))
        elif self.is_kamek():
            _defines.append(Define("SME_BUILD_KAMEK_INLINE"))

        self.defines = _defines

    @property
    def solutionRegionDir(self) -> Path:
        if self.is_release():
            return self.projectDir / "bin" / "release" / self.region.lower()
        elif self.is_debug():
            return self.projectDir / "bin" / "debug" / self.region.lower()

    @property
    def solutionAnyDir(self) -> Path:
        if self.is_release():
            return self.projectDir / "bin" / "release" / "any"
        elif self.is_debug():
            return self.projectDir / "bin" / "debug" / "any"

    def is_release(self) -> bool:
        return self.state == FilePatcher.State.RELEASE

    def is_debug(self) -> bool:
        return self.state == FilePatcher.State.DEBUG

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

        self._rename_files_from_config(self.solutionAnyDir)
        self._rename_files_from_config(self.solutionRegionDir)
        self._delete_files_from_config(self.solutionAnyDir)
        self._delete_files_from_config(self.solutionRegionDir)
        self._replace_files_from_config(self.solutionAnyDir)
        self._replace_files_from_config(self.solutionRegionDir)

        if self._patch_dol() and self.is_booting():
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
                           "-C Dolphin.Core.Fastmem=False",
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
                           "-C Dolphin.Core.Fastmem=False",
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

    def _build_iso(self, config: dict) -> Path:
        print("")
        print("====== ISO BUILDING ======".center(128))
        print("-"*128)

        isoPath = Path(config["buildpath"])

        print(f"{self.dest.parent.parent} -> {isoPath}")

        iso = GamecubeISO.from_root(self.dest.parent.parent, True)
        iso.build(isoPath, preCalc=False)

        print("-"*128)
        return isoPath

    @wrap_printer("DOL PATCHING")
    def _patch_dol(self) -> bool:
        dolPath = self.solutionRegionDir / "system/main.dol"
        kernelPath = self.solutionRegionDir / "kuribo/KuriboKernel.bin"
        modulesDest = self.gameDir / \
            ("files/Kuribo!/Mods/" if self.is_kuribo() else "files/")

        if dolPath.exists():
            if self.is_release():
                print(f"Generating {self.maxShines} shines RELEASE build")
            elif self.is_debug():
                print(f"Generating {self.maxShines} shines DEBUG build")

            modules = self.run(Path("src"), dolPath, fout=self._fout.stream)
            _doldata = DolFile(BytesIO(self.dest.read_bytes()))

            if isinstance(modules, list):
                size = 0
                for m in modules:
                    m.rename(modulesDest / m.name)
                    size += m.stat().st_size
                self._alloc_from_heap(
                    _doldata, (kernelPath.stat().st_size + 31) & -32)
            elif isinstance(modules, Path):
                renamed = (modulesDest /
                           modules.name).with_name("SME").with_suffix(".kxe" if self.is_kuribo() else ".kmk")
                renamed.parent.mkdir(parents=True, exist_ok=True)
                renamed.unlink(missing_ok=True)
                modules.rename(renamed)
                self._alloc_from_heap(
                    _doldata, (kernelPath.stat().st_size + 31) & -32)

            if self.is_kuribo():
                tmpbin = Path("bin", f"kuriboloader-{self.region.lower()}.bin")

                data = BytesIO(tmpbin.read_bytes())
                rawData = data.getvalue()
                injectaddr = (int.from_bytes(rawData[
                    :4], "big", signed=False) & 0x1FFFFFC) | 0x80000000
                codelen = int.from_bytes(
                    rawData[4:8], "big", signed=False) * 8

                blockstart = 0x80003C00
                _doldata.seek(blockstart)
                _doldata.write(rawData[8:])
                _doldata.insert_branch(blockstart, injectaddr)
                _doldata.insert_branch(
                    injectaddr + 4, blockstart + (codelen - 4))

            _doldata.write_uint32(self.by_region(0x802A73F0, 0, 0, 0), 0x60000000)
            _doldata.write_uint32(self.by_region(0x802A7404, 0, 0, 0), 0x60000000)

            with self.dest.open("wb") as dest:
                _doldata.save(dest)

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

        for path in config["rename"]:
            rename = config["rename"][path]

            absPath = self.gameDir / path

            if absPath.exists() and absPath not in _renamed:
                print(f"{absPath} -> {self.gameDir / absPath.parent / rename}")
                absPath.rename(absPath.parent / rename)
                _renamed.append(absPath)

    @wrap_printer("DELETING")
    def _delete_files_from_config(self, solutionPath: Path):
        with Path(solutionPath / ".config.json").open("r") as f:
            config = json.load(f)

        for path in config["delete"]:
            absPath = self.gameDir / path
            if absPath.exists():
                print(f"{absPath} -> DELETED")
                absPath.unlink()

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
        bnr.save_bnr(self.gameDir / "opening.bnr")

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


def _get_bit_alignment(num: int) -> int:
    for i in range(1000):
        if (num >> i) & 1 == 1:
            return i
    return -1


def main():
    parser = argparse.ArgumentParser(
        "SMS-Patcher", description="C++ Patcher for SMS NTSC-U, using Kamek by Treeki")

    parser.add_argument("gamefolder", help="root folder of GCR extracted ISO")
    parser.add_argument("-p", "--projectfolder",
                        help="project folder used to patch game", metavar="PATH")
    parser.add_argument("-s", "--startaddr", help="Starting address for the linker and code",
                        default="0x80000000", metavar="ADDR")
    parser.add_argument("-b", "--build", help="Build type; R=Release, D=Debug",
                        choices=["R", "D"], default="D")
    parser.add_argument("-c", "--compiler", choices=[
                        Compiler.Compilers.CODEWARRIOR, Compiler.Compilers.CLANG, Compiler.Compilers.GCC],
                        default=Compiler.Compilers.CODEWARRIOR, help="Compiler to build with")
    parser.add_argument("-r", "--region", help="Game region",
                        choices=[Region.US, Region.EU, Region.JP, Region.KR], default=Region.US)
    parser.add_argument("-P", "--patcher", help="Game patcher",
                        choices=[Patcher.KAMEK, Patcher.KURIBO], default=Patcher.KAMEK)
    parser.add_argument("--boot", help="What to boot from",
                        choices=["DOL", "ISO", "NONE"], default="NONE")
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
        build = FilePatcher.State.DEBUG
    else:
        build = FilePatcher.State.RELEASE

    if args.out:
        out = FileStream(args.out, open(args.out, "w"))
    else:
        out = None

    patcher = FilePatcher(build, args.gamefolder, args.projectfolder, args.region, args.compiler, args.patcher,
                          args.boot, args.startaddr, args.shines, out)

    if patcher.is_codewarrior():
        patcher.cxxOptions = ["-Cpp_exceptions off", "-gccinc", "-gccext on", "-enum int", "-RTTI off"
                              "-fp fmadd", "-use_lmw_stmw on", "-O4,p", "-c", "-rostr", "-sdata 0", "-sdata2 0"]
    elif patcher.is_clang():
        patcher.cxxOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-std=gnu++17", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math",
                             "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fuse-ld=lld", "-fpermissive", "-Wall", "-O3", "-r", "-v"]
        patcher.cOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math",
                           "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fuse-ld=lld", "-fpermissive", "-Wall", "-O3", "-r", "-v"]
        patcher.sOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables",
                           "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fuse-ld=lld", "-Wall", "-r", "-v"]
        patcher.linkOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-std=gnu++17", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math",
                              "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fuse-ld=lld", "-fpermissive", "-Wall", "-O3", "-r", "-v"]
    elif patcher.is_gcc():
        patcher.cxxOptions = ["-nodefaultlibs", "-nostdlib", "-std=gnu++20",
                              "-fno-exceptions", "-fno-rtti", "-ffast-math", "-fpermissive", "-Wall", "-O3", "-r"]
        patcher.cOptions = ["-nodefaultlibs", "-nostdlib", "-fno-exceptions",
                            "-fno-rtti", "-ffast-math", "-fpermissive", "-Wall", "-O3", "-r"]
        patcher.sOptions = ["-nodefaultlibs", "-nostdlib",
                            "-fno-exceptions", "-fno-rtti", "-Wall", "-O3", "-r"]
        patcher.linkOptions = ["-nodefaultlibs", "-nostdlib", "-std=gnu++20",
                               "-fno-exceptions", "-fno-rtti", "-ffast-math", "-fpermissive", "-Wall", "-O3", "-r"]

    sys.stdout = out.stream
    sys.stderr = out.stream

    patcher.patch_game()

    sys.stdout = sys.__stdout__
    sys.stderr = sys.__stderr__

if __name__ == "__main__":
    main()
