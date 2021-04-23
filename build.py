from __future__ import annotations

import argparse
import atexit
import json
import subprocess
import time
from fnmatch import fnmatch
from io import BytesIO
from pathlib import Path
from typing import Any, Dict, Generator, List, Optional, Union

import oead
import psutil
from dolreader.dolfile import DolFile, TextSection, write_uint32
from pyisotools.bnrparser import BNR
from pyisotools.iso import GamecubeISO

from compiler import Compiler, Define

TMPDIR = Path("tmp-compiler")


@atexit.register
def clean_resources():
    if TMPDIR.is_dir():
        pass  # shutil.rmtree(TMPDIR)


class Region:
    US = "US"
    EU = "EU"
    JP = "JP"
    KR = "KR"


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

        def is_rel_hi(self) -> bool:
            return self.reltype == AllocationMap.RelocationType.HI

        def is_rel_lo(self) -> bool:
            return self.reltype == AllocationMap.RelocationType.LO

        def is_rel_none(self) -> bool:
            return self.reltype == AllocationMap.RelocationType.NONE

        def as_translated(self, offset) -> int:
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

    def group(self, region: Region.US) -> List[AllocationMap.AllocationPacket]:
        return self._template[region]


_ALLOC_INFO = AllocationMap({
    Region.US: (AllocationMap.AllocationPacket(0x80341E74, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]),
                (AllocationMap.AllocationPacket(0x80341EAC, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]))),
    Region.EU: (AllocationMap.AllocationPacket(0x80341E74, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]),
                (AllocationMap.AllocationPacket(0x80341EAC, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]))),
    Region.JP: (AllocationMap.AllocationPacket(0x80341E74, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]),
                (AllocationMap.AllocationPacket(0x80341EAC, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]))),
    Region.KR: (AllocationMap.AllocationPacket(0x80341E74, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]),
                (AllocationMap.AllocationPacket(0x80341EAC, [AllocationMap.InstructionInfo(0x3C600000, AllocationMap.RelocationType.HI), AllocationMap.InstructionInfo(0x60630000, AllocationMap.RelocationType.LO)]))),
})


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
                 region: Region = Region.US, bootfrom: BootType = BootType.DOL,
                 startAddr: int = 0x80000000, shines: int = 120):

        if isinstance(startAddr, str):
            startAddr = int(startAddr, 16)

        self.projectDir = Path(projectDir)
        self.gameDir = Path(gameDir)
        self.state = build
        self.maxShines = shines
        self.bootType = bootfrom
        self.region = region
        self._fileTables = {}

        self._init_tables()

        super().__init__(Path("compiler"),
                         self._get_matching_filepath(
                             self.solutionDir / "main.dol"),
                         Path(f"linker/{self.region}.map"),
                         False,
                         startAddr)

        if self.is_release():
            self.defines = [
                Define("SME_MAX_SHINES", f"{self.maxShines}"), Define("SME_RELEASE")]
        elif self.is_debug():
            self.defines = [
                Define("SME_MAX_SHINES", f"{self.maxShines}"), Define("SME_DEBUG")]
        else:
            raise ValueError(f"Unknown patcher state {self.state}")

    @property
    def solutionDir(self) -> Path:
        if self.is_release():
            return self.projectDir / "bin" / "release" / self.region.lower()
        elif self.is_debug():
            return self.projectDir / "bin" / "debug" / self.region.lower()

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

    def patch_game(self):
        with (self.solutionDir / ".config.json").open("r") as f:
            config = json.load(f)

        self._rename_files_from_config()
        self._replace_files(config)

        PATCHED = self._patch_dol()
        print(PATCHED, self.is_booting())
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

    def _patch_dol(self) -> bool:
        dolPath = self.solutionDir / "system/main.dol"
        kernelPath = self.solutionDir / "kuribo/KuriboKernel.bin"
        modulesDest = self.gameDir / "files/Kuribo!/Mods/"

        if dolPath.exists():
            if self.is_release():
                print("")
                print("====== DOL PATCHING ======".center(128))
                print("-"*128)
                print(f"Generating {self.maxShines} shines RELEASE build")

            elif self.is_debug():
                print("")
                print("====== DOL PATCHING ======".center(128))
                print("-"*128)
                print(f"Generating {self.maxShines} shines DEBUG build")

            modules = self.run(Path("src/src-code"), dolPath)
            _doldata = DolFile(BytesIO(self.dest.read_bytes()))

            if isinstance(modules, list):
                size = 0
                for m in modules:
                    m.rename(modulesDest / m.name)
                    size += m.stat().st_size
                self._alloc_from_heap(
                    _doldata, (kernelPath.stat().st_size + size + 31) & -32)
            elif isinstance(modules, Path):
                renamed = (modulesDest / modules.name).with_name("SME").with_suffix(".kxe")
                renamed.parent.mkdir(parents=True, exist_ok=True)
                renamed.unlink(missing_ok=True)
                modules.rename(renamed)
                self._alloc_from_heap(
                    _doldata, (kernelPath.stat().st_size + renamed.stat().st_size + 31) & -32)

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
            _doldata.insert_branch(injectaddr + 4, blockstart + (codelen - 4))

            with self.dest.open("wb") as dest:
                _doldata.save(dest)

            print("-"*128)
            return True
        return False

    def _replace_files(self, config: dict):
        print("")
        print("====== REPLACING | COPYING ======".center(128))
        print("-"*128)

        bnrPath = (self.solutionDir / config["gamebanner"]).resolve()

        self._compile_bnr_to_game(bnrPath)
        destPath = self.gameDir / "opening.bnr"

        for f in self.solutionDir.rglob("*"):
            relativePath = f.relative_to(Path.cwd())

            if f.name.lower() == ".config.json":
                continue

            destPath = self._get_matching_filepath(f)

            if destPath is None:
                if f.is_file() and str(f.relative_to(Path.cwd(), "bin", "debug")) not in config["ignore"]:
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
            else:
                destPath.write_bytes(f.read_bytes())

            print(f"{relativePath} -> {destPath}")

        print("-"*128)

    def _get_translated_filepath(self, relativePath: Union[str, Path]) -> Path:
        return self.gameDir / relativePath

    def _init_tables(self):
        for f in self.gameDir.rglob("*"):
            if f.is_file:
                self._fileTables.setdefault(f.suffix, []).append(f)

    def _get_matching_filepath(self, path: Path) -> Path:
        try:
            for f in self._fileTables[path.suffix]:
                if f.name == path.name:
                    return self.gameDir / f
        except KeyError:
            return self._get_path_from_config(path)
        else:
            return self._get_path_from_config(path)

    def _get_path_from_config(self, path: Path) -> Path:
        if self.is_release():
            parentGlob = "*/release/"
        elif self.is_debug():
            parentGlob = "*/debug/"
        else:
            raise ValueError("Invalid State!")

        with Path(self.solutionDir / ".config.json").open("r") as f:
            config = json.load(f)
            for _set in config["userfiles"]:
                glob = list(_set.keys())[0]
                if fnmatch(str(path).lower(), parentGlob + glob.strip().lower()):
                    if _set[glob]["rename"].strip() == "":
                        return self.gameDir / _set[glob]["destination"].strip() / path.name
                    else:
                        return self.gameDir / _set[glob]["destination"].strip() / _set[glob]["rename"].strip()

        return None

    def _rename_files_from_config(self):
        print("")
        print("====== RENAMING ======".center(128))
        print("-"*128)

        _renamed = []

        with Path(self.solutionDir / ".config.json").open("r") as f:
            config = json.load(f)
            for path in config["rename"]:
                rename = config["rename"][path]

                absPath = self.gameDir / path

                if absPath.exists() and absPath not in _renamed:
                    print(f"{absPath} -> {self.gameDir / absPath.parent / rename}")
                    absPath.rename(self.gameDir / absPath.parent / rename)
                    _renamed.append(absPath)

        print("-"*128)

    def _delete_files_from_config(self):
        with Path(self.solutionDir / ".config.json").open("r") as f:
            config = json.load(f)
            for path in config["delete"]:
                absPath = self.gameDir / path
                if absPath.exists():
                    print(f"{absPath} -> DELETED")
                    absPath.unlink()

    def _compile_bnr_to_game(self, path):
        bnr = BNR(path, BNR.Regions.AMERICA)
        bnr.save_bnr(self.gameDir / "opening.bnr")

    def _alloc_from_heap(self, dol: DolFile, size: int):
        group = _ALLOC_INFO.group(self.region)
        size = (size + 31) & -32

        for packet in group:
            dol.seek(packet.address)
            for instr in packet.instructions:
                write_uint32(dol, instr.as_translated(self.startaddr + size))


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
    parser.add_argument("-r", "--region", help="Game region",
                        choices=["US", "EU", "JP", "KR"], default="US")
    parser.add_argument("--boot", help="What to boot from",
                        choices=["DOL", "ISO", "NONE"], default="NONE")
    parser.add_argument(
        "--shines", help="Max shines allowed", type=int, default=120)

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

    patcher = FilePatcher(build, args.gamefolder, args.projectfolder, args.region,
                          args.boot, args.startaddr, args.shines)

    if patcher.is_codewarrior():
        patcher.cxxOptions = ["-Cpp_exceptions off", "-gccinc", "-gccext on", "-enum int",
                              "-fp hard", "-use_lmw_stmw on", "-O4,p", "-c", "-rostr", "-sdata 0", "-sdata2 0"]
    elif patcher.is_clang():
        patcher.cxxOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-std=gnu++17", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math",
                              "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fuse-ld=lld", "-fpermissive", "-Wall", "-O3", "-r", "-v"]
        patcher.cOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math", "-fdeclspec",
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

    patcher.patch_game()


if __name__ == "__main__":
    main()
