from __future__ import annotations

from abc import ABC, abstractmethod
import argparse
import atexit
from enum import Enum, IntEnum
import sys
import time
from turtle import end_fill
from pathos.multiprocessing import ProcessPool, ThreadPool
import json
import os
import shutil
import subprocess
import tempfile
import logging


from io import BytesIO, StringIO
from pathlib import Path
from typing import Dict, Iterable, List, Optional, TextIO, Tuple, Union
from subprocess import Popen

from dolreader.dol import DolFile
from dolreader.section import TextSection

import pykamek

TMPDIR = Path(tempfile.gettempdir())
COMPILER_PATH = Path("assets/compiler")


@atexit.register
def clean_resources():
    if TMPDIR.is_dir():
        ...# shutil.rmtree(TMPDIR)


def _get_bit_alignment(num: int, limit: int = 64) -> int:
    for i in range(limit):
        if (num >> i) & 1 == 1:
            return i
    return -1


def _get_clang_errors(stream: TextIO) -> List[str]:
    _old = stream.tell()
    stream.seek(0, 2)
    end = stream.tell()
    stream.seek(_old, 0)

    errors = []
    while stream.tell() < end:
        currentLine = stream.readline()
        if "error:" not in currentLine:
            continue
        error = [currentLine]
        print(currentLine)
        while stream.tell() < end:
            futureLine = stream.readline()
            error.append(futureLine)
            if "error generated." in futureLine or "errors generated." in futureLine:
                print(error)
                errors.append("".join(error))
                break
    stream.seek(_old, 0)
    return errors



class Region(str, Enum):
    US = "US"
    EU = "EU"
    JP = "JP"
    KR = "KR"
    ANY = "ANY"


class CompilerKind(str, Enum):
    CLANG = "CLANG"
    GCC = "GCC"
    CODEWARRIOR = "CODEWARRIOR"


class KernelKind(str, Enum):
    KAMEK = "KAMEK"
    KURIBO = "KURIBO"


class BuildKind(IntEnum):
    RELEASE = 0
    RELEASE_DEBUG = 1
    DEBUG = 2


class AllocationMap(object):
    class RelocationKind(IntEnum):
        HI = 0
        LO = 1
        NONE = -1

    class InstructionInfo(object):
        def __init__(self, value: int, relocationType: AllocationMap.RelocationKind, relmask: int = 0xFFFF):
            self.value = value
            self.reltype = relocationType
            self._relmask = relmask

        def __str__(self) -> str:
            return f"0x{self.value:08X} (Mask ({self.reltype}): 0x{self._relmask:08X})"

        def is_rel_hi(self) -> bool:
            return self.reltype == AllocationMap.RelocationKind.HI

        def is_rel_lo(self) -> bool:
            return self.reltype == AllocationMap.RelocationKind.LO

        def is_rel_none(self) -> bool:
            return self.reltype == AllocationMap.RelocationKind.NONE

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
        def translate(value: int, offset: int, relocationType: AllocationMap.RelocationKind, relmask: int = 0xFFFF):
            instr = AllocationMap.InstructionInfo(
                value, relocationType, relmask
            )
            return instr.as_translated(offset)

    class AllocationPacket(object):
        def __init__(self, address: int, baseInstrs: List[AllocationMap.InstructionInfo]):
            self.__address = address
            self.__baseInstrs = baseInstrs

        @property
        def instructions(self) -> Iterable[AllocationMap.InstructionInfo]:
            for instr in self.__baseInstrs:
                yield instr

        @property
        def address(self) -> int:
            return self.__address

    def __init__(self, template: Optional[Dict[Region, List[AllocationMap.AllocationPacket]]] = None):
        if template:
            self.__template = template
        else:
            self.__template = {
                Region.US: [],
                Region.EU: [],
                Region.JP: [],
                Region.KR: []
            }

    def group(self, region: Region) -> List[AllocationMap.AllocationPacket]:
        return self.__template[region]


__Map = AllocationMap
_ALLOC_LO_INFO = __Map({
    Region.US: (
        __Map.AllocationPacket(
            0x80341E74,
            [
                __Map.InstructionInfo(0x3C600000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60630000, __Map.RelocationKind.LO)
            ]
        ),
        __Map.AllocationPacket(
            0x80341EAC,
            [
                __Map.InstructionInfo(0x3C600000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60630000, __Map.RelocationKind.LO)
            ]
        )
    ),
    Region.EU: (
        __Map.AllocationPacket(
            0x80341E74,
            [
                __Map.InstructionInfo(0x3C600000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60630000, __Map.RelocationKind.LO)
            ]
        ),
        __Map.AllocationPacket(
            0x80341EAC,
            [
                __Map.InstructionInfo(0x3C600000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60630000, __Map.RelocationKind.LO)
            ]
        )
    ),
    Region.JP: (
        __Map.AllocationPacket(
            0x80341E74,
            [
                __Map.InstructionInfo(0x3C600000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60630000, __Map.RelocationKind.LO)
            ]
        ),
        __Map.AllocationPacket(
            0x80341EAC,
            [
                __Map.InstructionInfo(0x3C600000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60630000, __Map.RelocationKind.LO)
            ]
        )
    ),
    Region.KR: (
        __Map.AllocationPacket(
            0x80341E74,
            [
                __Map.InstructionInfo(0x3C600000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60630000, __Map.RelocationKind.LO)
            ]
        ),
        __Map.AllocationPacket(
            0x80341EAC,
            [
                __Map.InstructionInfo(0x3C600000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60630000, __Map.RelocationKind.LO)
            ]
        )
    ),
})

_ALLOC_HI_INFO = __Map({
    Region.US: (
        __Map.AllocationPacket(
            0x80341EC8,
            [
                __Map.InstructionInfo(0x3C000000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60000000, __Map.RelocationKind.LO),
                __Map.InstructionInfo(0x7C601850, __Map.RelocationKind.NONE),
                __Map.InstructionInfo(0x3C808000, __Map.RelocationKind.NONE),
                __Map.InstructionInfo(0x90640034, __Map.RelocationKind.NONE)
            ]
        ),
    ),
    Region.EU: (
        __Map.AllocationPacket(
            0x80341ED0,
            [
                __Map.InstructionInfo(0x3C600000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60630000, __Map.RelocationKind.LO)
            ]),
    ),
    Region.JP: (
        __Map.AllocationPacket(
            0x80341ED0,
            [
                __Map.InstructionInfo(0x3C600000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60630000, __Map.RelocationKind.LO)
            ]),
    ),
    Region.KR: (
        __Map.AllocationPacket(
            0x80341ED0,
            [
                __Map.InstructionInfo(0x3C600000, __Map.RelocationKind.HI),
                __Map.InstructionInfo(0x60630000, __Map.RelocationKind.LO)
            ]
        ),
    )
})


class Define:
    def __init__(self, name: str, attr: Optional[str] = None):
        self.name = name
        self.attr = "1" if attr is None else attr

    def is_defined(self) -> bool:
        return self.attr is not None

    def has_attr(self) -> bool:
        return self.is_defined() and self.attr != "1"

    def to_cli_command(self) -> str:
        return f"-D{self}"

    def __format__(self, fmt: str) -> str:
        return self.__str__()

    def __str__(self) -> str:
        return f"{self.name}={self.attr}"


class Module:
    """
    Represents an executable module to be compiled
    """

    def __init__(
        self,
        name: str,
        objects: Optional[List[Path]] = None,
        defines: Optional[List[str]] = None,
        includes: Optional[List[str]] = None,
        cppOptions: Optional[List[str]] = None,
        cOptions: Optional[List[str]] = None,
        sOptions: Optional[List[str]] = None,
        linkOptions: Optional[List[str]] = None,
        compiler: CompilerKind = CompilerKind.CODEWARRIOR
    ):
        self.name = name
        self.objects = objects if objects is not None else []
        self.defines = defines if defines is not None else []
        self.includes = includes if includes is not None else []
        self.cppOptions = cppOptions if cppOptions is not None else []
        self.cOptions = cOptions if cOptions is not None else []
        self.sOptions = sOptions if sOptions is not None else []
        self.linkOptions = linkOptions if linkOptions is not None else []
        self.compilerKind = compiler

    def populate_from_folder(self, path: Path):
        self.objects = [obj for obj in path.rglob(
            "*") if obj.is_file() and obj.suffix in {".s", ".c", ".cpp", ".cxx", ".c++"}]

    def compile(self, logger: logging.Logger) -> List[Path]:
        """
        Compiles the module into object files

        If a linker is set, it will link them together into a single object
        as the first item in the returned list
        """
        logger.info("[%s] Compiling into object...", self.name)

        tmpStream = open("tmpstream_.log", "w+")
        tmpObject = "tmp.o"

        cppObjects = []
        cObjects = []
        sObjects = []
        linkObjects = [
            lib for lib in Path("lib/").iterdir() if lib.suffix == ".a"
        ]
        
        cppOptions = [str(opt) for opt in self.cppOptions]
        cOptions = [str(opt) for opt in self.cOptions]
        sOptions = [str(opt) for opt in self.sOptions]
        linkOptions = [str(opt) for opt in self.linkOptions]

        if self.compilerKind == CompilerKind.CODEWARRIOR:
            cppCompilerPath = COMPILER_PATH / "codewarrior/mwcceppc.exe"
            cCompilerPath = COMPILER_PATH / "codewarrior/mwcceppc.exe"
            sCompilerPath = COMPILER_PATH / "codewarrior/mwasmeppc.exe"
            linkerPath = COMPILER_PATH / "codewarrior/mwldeppc.exe"
        elif self.compilerKind == CompilerKind.CLANG:
            cppCompilerPath = COMPILER_PATH / "clang/clang.exe"
            cCompilerPath = COMPILER_PATH / "clang/clang.exe"
            sCompilerPath = COMPILER_PATH / "clang/clang.exe"
            linkerPath = COMPILER_PATH / "clang/clang.exe"
        elif self.compilerKind == CompilerKind.GCC:
            cppCompilerPath = COMPILER_PATH / "gcc/gcc.exe"
            cCompilerPath = COMPILER_PATH / "gcc/gcc.exe"
            sCompilerPath = COMPILER_PATH / "gcc/gcc.exe"
            linkerPath = COMPILER_PATH / "gcc/gcc.exe"
        else:
            raise ValueError(f"Unknown compiler kind \"{self.compilerKind}\"")

        runningTasks: List[Popen] = []
        for obj in self.objects:
            obj = obj.resolve()
            if obj.is_file():
                if obj.suffix in (".cpp", ".cxx", ".c++"):
                    cppObjects.append(str(obj))
                elif obj.suffix == ".c":
                    cObjects.append(str(obj))
                elif obj.suffix == ".s":
                    sObjects.append(str(obj))
            else:
                logger.info("[%s] %s is not a file", self.name, obj)

            # elif obj.is_dir():
            #     for f in obj.rglob("*"):
            #         f = f.resolve()
            #         if f.suffix in (".cpp", ".cxx", ".c++"):
            #             cppObjects.append(str(f))
            #         elif f.suffix == ".c":
            #             cObjects.append(str(f))
            #         elif f.suffix == ".s":
            #             sObjects.append(str(f))

        TMP_CPP = TMPDIR / f"cpp_obj.o"
        TMP_C = TMPDIR / f"c_obj.o"
        TMP_ASM = TMPDIR / f"s_obj.o"
        if len(cppObjects) > 0:
            # print(" ".join([str(cppCompilerPath), *cppObjects, *self.includes,
            #                 *self.defines, *cppOptions, "-o", str(TMP_CPP)]))
            runningTasks.append(
                self.__compile_objects(
                    str(TMP_CPP),
                    str(cppCompilerPath),
                    cppObjects,
                    self.includes,
                    self.defines,
                    cppOptions,
                    stdout=tmpStream,
                    stderr=tmpStream
                )
            )
            linkObjects.append(TMP_CPP)
        if len(cObjects) > 0:
            runningTasks.append(
                self.__compile_objects(
                    str(TMP_C),
                    str(cCompilerPath),
                    cObjects,
                    self.includes,
                    self.defines,
                    cOptions,
                    stdout=tmpStream,
                    stderr=tmpStream
                )
            )
            linkObjects.append(TMP_C)
        if len(sObjects) > 0:
            runningTasks.append(
                self.__compile_objects(
                    str(TMP_ASM),
                    str(sCompilerPath),
                    sObjects,
                    self.includes,
                    self.defines,
                    sOptions,
                    stdout=tmpStream,
                    stderr=tmpStream
                )
            )
            linkObjects.append(TMP_ASM)

        while any([task.poll() is None for task in runningTasks]):
            time.sleep(0.1)

        for obj in linkObjects:
            if not obj.is_file():
                logger.warning(
                    "[%s] Failed to compile into %s", self.name, obj.name
                )

        if len(linkObjects) != 0 and linkerPath.is_file():
            subprocess.run([str(linkerPath), *linkObjects, *linkOptions, "-o", tmpObject], stdout=tmpStream, stderr=tmpStream,
                           text=True)
            linkObjects = [tmpObject]

        tmpStream.seek(0)
        self.__report_errors(_get_clang_errors(tmpStream), logger)
        tmpStream.close()
        # os.unlink("tmpstream_.log")
        return linkObjects

    def __compile_objects(
        self,
        dst: str,
        compiler: str,
        objects: List[str],
        includes: List[str],
        defines: List[str],
        options: List[str],
        stdout: Optional[TextIO] = None,
        stderr: Optional[TextIO] = None
    ) -> Popen:
        if stdout is None:
            stdout = sys.stdout
        if stderr is None:
            stderr = sys.stderr
        return Popen(
            [
                compiler,
                *objects,
                *includes,
                *defines,
                *options,
                "-o", dst
            ],
            stdout=stdout,
            stderr=stderr
        )

    def __report_errors(self, errors: List[str], logger: logging.Logger):
        if len(errors) > 0:
            logger.error("\n".join(errors).rstrip())


class _Compiler(ABC):
    def __init__(
        self,
        compilerPath: Path,
        startaddr: int = 0x80000000,
        buildKind: BuildKind = BuildKind.DEBUG,
        region: Region = Region.US,
        dump: bool = False,
        logger: logging.Logger = None
    ):
        if logger is None:
            logger = logging.getLogger(f"Base Compiler")
        self.logger = logger

        if isinstance(compilerPath, str):
            compilerPath = Path(compilerPath)

        self.linker = Path("assets/linker") / f"{region.lower()}.map"
        if self.linker and not self.linker.exists():
            raise FileNotFoundError(
                f"Linker map could not be found :: {self.linker}")

        self.startaddr = startaddr
        self.buildKind = buildKind
        self.region = region
        self.dump = dump

        self.__compilers = {}
        self.__includes = []
        self.__defines = []
        self.__cxxOptions = []
        self.__cOptions = []
        self.__sOptions = []
        self.__linkOptions = []

        self._init_compilers(compilerPath)
        self._init_includes()

    @property
    def cxxOptions(self) -> List[str]:
        return self.__cxxOptions

    @cxxOptions.setter
    def cxxOptions(self, opts: Union[str, list]):
        if isinstance(opts, str):
            self.__cxxOptions = [opts]
        else:
            self.__cxxOptions = opts

    @property
    def cOptions(self) -> List[str]:
        return self.__cOptions

    @cOptions.setter
    def cOptions(self, opts: Union[str, list]):
        if isinstance(opts, str):
            self.__cOptions = [opts]
        else:
            self.__cOptions = opts

    @property
    def sOptions(self) -> List[str]:
        return self.__sOptions

    @sOptions.setter
    def sOptions(self, opts: Union[str, list]):
        if isinstance(opts, str):
            self.__sOptions = [opts]
        else:
            self.__sOptions = opts

    @property
    def linkOptions(self) -> List[str]:
        return self.__linkOptions

    @linkOptions.setter
    def linkOptions(self, opts: Union[str, list]):
        if isinstance(opts, str):
            self.__linkOptions = [opts]
        else:
            self.__linkOptions = opts

    @property
    def includes(self) -> List[str]:
        return self.__includes

    @includes.setter
    def includes(self, incs: List[str]):
        if self.is_clang():
            self.__includes = [f'-I{inc}' for inc in incs]
        elif self.is_gcc():
            self.__includes = [f'-I{inc}' for inc in incs]
        elif self.is_codewarrior():
            self.__includes = [f'-I{inc}' for inc in incs]

    @property
    def defines(self) -> List[str]:
        return self.__defines

    @defines.setter
    def defines(self, defs: List[Define]):
        self.__defines = [d.to_cli_command() for d in defs]

    def get_assets_path(self):
        buildFolder = "release" if self.is_release() else "debug"
        return Path.cwd() / "assets/bin" / buildFolder / self.region.lower()

    def is_release(self) -> bool:
        return self.buildKind in {BuildKind.RELEASE, BuildKind.RELEASE_DEBUG}

    def is_debug(self) -> bool:
        return self.buildKind == BuildKind.DEBUG

    @abstractmethod
    def is_clang(self) -> bool: ...

    @abstractmethod
    def is_codewarrior(self) -> bool: ...

    @abstractmethod
    def is_gcc(self) -> bool: ...

    @abstractmethod
    def is_kamek(self) -> bool: ...

    @abstractmethod
    def is_kuribo(self) -> bool: ...

    @abstractmethod
    def run(self, src: Union[str, Path], dol: DolFile) -> List[Path]:
        """
        Compile the given path and patch the DOL
        """
        ...

    def _init_compilers(self, path: Path):
        clangFolder = path / "clang"
        cwFolder = path / "codewarrior"
        gccFolder = path / "gcc"
        for f in clangFolder.iterdir():
            if f.is_file() and f.suffix == ".exe":
                self.__compilers[f.stem] = f
        for f in cwFolder.iterdir():
            if f.is_file() and f.suffix == ".exe":
                self.__compilers[f.stem] = f
        for f in gccFolder.iterdir():
            if f.is_file() and f.suffix == ".exe":
                self.__compilers[f.stem] = f

    def _init_includes(self):
        cppConfig = Path(".vscode/c_cpp_properties.json")

        if cppConfig.exists():
            with cppConfig.open("r") as config:
                jsonData = json.load(config)

            self.includes = [Path(i.replace(r"${workspaceFolder}", str(Path.cwd())))
                             for i in jsonData["configurations"][0]["includePath"]]

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


class KamekCompiler(_Compiler):
    def is_clang(self) -> bool:
        return False

    def is_codewarrior(self) -> bool:
        return True

    def is_gcc(self) -> bool:
        return False

    def is_kamek(self) -> bool:
        return True

    def is_kuribo(self) -> bool:
        return False

    def run(self, src: Union[str, Path], dol: DolFile) -> List[Path]:
        raise NotImplementedError("Support for Kamek is unimplemented")

        module = Module(
            "SME-KamekCodeWarrior",
            defines=self.defines,
            includes=self.includes,
            cppOptions=self.cxxOptions,
            cOptions=self.cOptions,
            sOptions=self.sOptions,
            linkOptions=self.linkOptions,
            compiler=CompilerKind.CODEWARRIOR
        )
        dumpCode = buildDir / "source.code"
        tmpDumpCode = TMPDIR / "source.tmp"
        tmpDumpDol = TMPDIR / self.dest.name if self.dest else "main.dol"

        if dol is not None:
            if not self.dest.parent.exists():
                self.dest.mkdir(parents=True, exist_ok=True)

            if self.linker:
                kCmdtype = ["--extern", f"{self.linker.resolve()}", "--input-dol",
                            f"{dol.resolve()}", "--output-dol", f"{self.dest}"]
            else:
                kCmdtype = ["--input-dol",
                            f"{dol.resolve()}", "--output-dol", f"{tmpDumpDol}"]

            if self.dump:
                kCmdtype.extend(
                    ["--output-code", f"{dumpCode}", "--output-code", f"{tmpDumpCode}"])
            else:
                kCmdtype.extend(["--output-code", f"{tmpDumpCode}"])
        else:
            if self.dest is None:
                self.dest = dumpCode

            if self.linker:
                kCmdtype = [
                    "--extern", f"{self.linker.resolve()}", "--output-code", f"{tmpDumpCode}"]
            else:
                kCmdtype = ["--output-code", f"{tmpDumpCode}"]


class _KuriboCompiler(_Compiler):
    def __init__(
        self,
        compilerPath: Path,
        startaddr: int = 0x80000000,
        buildKind: BuildKind = BuildKind.DEBUG,
        region: Region = Region.US,
        dump: bool = False,
        logger: logging.Logger = None
    ):
        super().__init__(compilerPath, startaddr, buildKind, region, dump, logger)
        self._kernelPath = self.get_assets_path() / "kuribo/KuriboKernel.bin"

    def is_kamek(self) -> bool:
        return False

    def is_kuribo(self) -> bool:
        return True


class KuriboClangCompiler(_KuriboCompiler):
    def is_clang(self) -> bool:
        return True

    def is_codewarrior(self) -> bool:
        return False

    def is_gcc(self) -> bool:
        return False

    def run(self, src: Union[str, Path], dol: DolFile) -> List[Path]:
        _defines = self.defines

        if self.buildKind == BuildKind.DEBUG:
            _defines.append(Define("SME_DEBUG").to_cli_command())
        elif self.buildKind == BuildKind.RELEASE_DEBUG:
            _defines.append(Define("SME_RELEASE").to_cli_command())
        elif self.buildKind == BuildKind.RELEASE:
            _defines.append(Define("SME_RELEASE").to_cli_command())

        if self.is_kuribo():
            _defines.append(Define("SME_BUILD_KURIBO").to_cli_command())
        elif self.is_kamek():
            _defines.append(Define("SME_BUILD_KAMEK_INLINE").to_cli_command())

        module = Module(
            name="SME",
            defines=_defines,
            includes=self.includes,
            cppOptions=self.cxxOptions,
            cOptions=self.cOptions,
            sOptions=self.sOptions,
            linkOptions=self.linkOptions,
            compiler=CompilerKind.CLANG
        )
        module.populate_from_folder(Path(src))
        objects = module.compile(logger=self.logger)

        kxeModules: List[Path] = []
        runningTasks: List[Tuple[Popen, Path]] = []
        for obj in objects:
            kxefile = Path(obj).with_name(f"{module.name}.kxe")
            runningTasks.append(
                (
                    Popen(
                        [
                            "tools/KuriboConverter/KuriboConverter.exe",
                            str(obj),
                            str(kxefile),
                            str(self.linker.resolve())
                        ],
                        text=True
                    ),
                    kxefile
                )
            )

        while any([task[0].poll() is None for task in runningTasks]):
            time.sleep(0.1)

        for task, module in runningTasks:
            if task.returncode == 0:
                kxeModules.append(module)

        size = sum([kxe.stat().st_size for kxe in kxeModules])
        self._alloc_from_heap(
            dol, (self._kernelPath.stat().st_size + 31) & -32)

        return kxeModules


class KuriboCodeWarriorCompiler(_KuriboCompiler):
    def is_clang(self) -> bool:
        return False

    def is_codewarrior(self) -> bool:
        return True

    def is_gcc(self) -> bool:
        return False

    def run(self, src: Union[str, Path], dol: DolFile) -> List[Path]:
        defines = self.defines

        if self.buildKind == BuildKind.DEBUG:
            defines.append(Define("SME_DEBUG"))
        elif self.buildKind == BuildKind.RELEASE_DEBUG:
            defines.append(Define("SME_RELEASE"))
        elif self.buildKind == BuildKind.RELEASE:
            defines.append(Define("SME_RELEASE"))

        if self.is_kuribo():
            defines.append(Define("SME_BUILD_KURIBO"))
        elif self.is_kamek():
            defines.append(Define("SME_BUILD_KAMEK_INLINE"))

        module = Module(
            "SME-KuriboCodeWarrior",
            defines=self.defines,
            includes=self.includes,
            cppOptions=self.cxxOptions,
            cOptions=self.cOptions,
            sOptions=self.sOptions,
            linkOptions=self.linkOptions,
            compiler=CompilerKind.CODEWARRIOR
        )
        module.populate_from_folder(Path(src))
        objects = module.compile(logger=self.logger)

        kxeModules: List[Path] = []
        runningTasks: List[Tuple[Popen, Path]] = []
        for obj in objects:
            kxefile = Path(obj).with_suffix(".kxe")
            runningTasks.append(
                (
                    Popen(
                        [
                            "tools/KuriboConverter/KuriboConverter.exe",
                            str(obj),
                            str(kxefile),
                            str(self.linker.resolve())
                        ],
                        text=True
                    ),
                    kxefile
                )
            )

        while any([task[0].poll() is None for task in runningTasks]):
            time.sleep(0.1)

        for task, module in runningTasks:
            if task.returncode == 0:
                kxeModules.append(module)

        size = sum([kxe.stat().st_size for kxe in kxeModules])
        self._alloc_from_heap(
            dol, (self._kernelPath.stat().st_size + 31) & -32)

        return kxeModules


class KuriboGCCCompiler(_KuriboCompiler):
    def is_clang(self) -> bool:
        return False

    def is_codewarrior(self) -> bool:
        return False

    def is_gcc(self) -> bool:
        return True

    def run(self, src: Union[str, Path], dol: DolFile) -> List[Path]:
        defines = self.defines

        if self.buildKind == BuildKind.DEBUG:
            defines.append(Define("SME_DEBUG"))
        elif self.buildKind == BuildKind.RELEASE_DEBUG:
            defines.append(Define("SME_RELEASE"))
        elif self.buildKind == BuildKind.RELEASE:
            defines.append(Define("SME_RELEASE"))

        if self.is_kuribo():
            defines.append(Define("SME_BUILD_KURIBO"))
        elif self.is_kamek():
            defines.append(Define("SME_BUILD_KAMEK_INLINE"))

        self.defines

        module = Module(
            "SME-KuriboGCC",
            defines=self.defines,
            includes=self.includes,
            cppOptions=self.cxxOptions,
            cOptions=self.cOptions,
            sOptions=self.sOptions,
            linkOptions=self.linkOptions,
            compiler=CompilerKind.GCC
        )
        module.populate_from_folder(Path(src))
        objects = module.compile(logger=self.logger)

        kxeModules: List[Path] = []
        runningTasks: List[Tuple[Popen, Path]] = []
        for obj in objects:
            kxefile = Path(obj).with_suffix(".kxe")
            runningTasks.append(
                (
                    Popen(
                        [
                            "tools/KuriboConverter/KuriboConverter.exe",
                            str(obj),
                            str(kxefile),
                            str(self.linker.resolve())
                        ],
                        text=True
                    ),
                    kxefile
                )
            )

        while any([task[0].poll() is None for task in runningTasks]):
            time.sleep(0.1)

        for task, module in runningTasks:
            if task.returncode == 0:
                kxeModules.append(module)

        size = sum([kxe.stat().st_size for kxe in kxeModules])
        self._alloc_from_heap(
            dol, (self._kernelPath.stat().st_size + 31) & -32)

        return kxeModules


class CompilerFactory:
    @staticmethod
    def create(
        kernel: KernelKind,
        compiler: CompilerKind,
        startAddress: int = 0x80000000,
        optimize: str = "-O1",
        debugLevel: BuildKind = BuildKind.DEBUG,
        region: Region = Region.US,
        logger: Optional[logging.Logger] = None
    ) -> _Compiler:
        if kernel == KernelKind.KAMEK:
            assert compiler == CompilerKind.CODEWARRIOR, "Kamek requires CodeWarrior!"
            newCompiler = KamekCompiler(
                COMPILER_PATH,
                startAddress,
                debugLevel,
                region,
                False,
                logger
            )
            newCompiler.cxxOptions = [
                "-Cpp_exceptions off", "-gccinc", "-gccext on", "-enum int",
                "-RTTI off" "-fp fmadd", "-use_lmw_stmw on", optimize,
                "-c", "-rostr", "-sdata 0", "-sdata2 0"
            ]
            newCompiler.cOptions = [
                "-Cpp_exceptions off", "-gccinc", "-gccext on", "-enum int",
                "-RTTI off" "-fp fmadd", "-use_lmw_stmw on", optimize,
                "-c", "-rostr", "-sdata 0", "-sdata2 0"
            ]
            newCompiler.sOptions = [
                "-enum int",
                "-RTTI off", optimize,
                "-c", "-rostr", "-sdata 0", "-sdata2 0"
            ]
            return newCompiler
        elif kernel == KernelKind.KURIBO:
            if compiler == CompilerKind.CODEWARRIOR:
                newCompiler = KuriboCodeWarriorCompiler(
                    COMPILER_PATH,
                    startAddress,
                    debugLevel,
                    region,
                    False,
                    logger
                )
                newCompiler.cxxOptions = [
                    "-Cpp_exceptions off", "-gccinc", "-gccext on", "-enum int",
                    "-RTTI off" "-fp fmadd", "-use_lmw_stmw on", optimize,
                    "-c", "-rostr", "-sdata 0", "-sdata2 0"
                ]
                newCompiler.cOptions = [
                    "-Cpp_exceptions off", "-gccinc", "-gccext on", "-enum int",
                    "-RTTI off" "-fp fmadd", "-use_lmw_stmw on", optimize,
                    "-c", "-rostr", "-sdata 0", "-sdata2 0"
                ]
                newCompiler.sOptions = [
                    "-enum int",
                    "-RTTI off", optimize,
                    "-c", "-rostr", "-sdata 0", "-sdata2 0"
                ]
            elif compiler == CompilerKind.CLANG:
                newCompiler = KuriboClangCompiler(
                    COMPILER_PATH,
                    startAddress,
                    debugLevel,
                    region,
                    False,
                    logger
                )
                newCompiler.cxxOptions = [
                    "--target=powerpc-gekko-ibm-kuribo-eabi", "-fdeclspec", "-std=gnu++17",
                    "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math",
                    "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array",
                    "-fno-use-cxa-atexit", "-fno-c++-static-destructors", "-fno-function-sections",
                    # f"-fmacro-prefix-map={Path.cwd()}=.",
                    "-fno-data-sections", "-fuse-ld=lld", "-fpermissive", "-Werror",
                    optimize, "-r", "-v"
                ]
                newCompiler.cOptions = [
                    "--target=powerpc-gekko-ibm-kuribo-eabi", "-fdeclspec", "-fno-exceptions",
                    "-fno-rtti", "-fno-unwind-tables", "-ffast-math", "-fdeclspec",
                    "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array",
                    "-fno-use-cxa-atexit", "-fno-c++-static-destructors", "-fno-function-sections",
                    "-fno-data-sections", "-fuse-ld=lld", "-fpermissive", "-Werror",
                    optimize, "-r", "-v"
                ]
                newCompiler.sOptions = [
                    "--target=powerpc-gekko-ibm-kuribo-eabi", "-fdeclspec", "-fno-exceptions",
                    "-fno-rtti", "-fno-unwind-tables", "-flto", "-nodefaultlibs", "-nostdlib",
                    "-fno-use-init-array", "-fno-use-cxa-atexit", "-fno-c++-static-destructors",
                    "-fno-function-sections", "-fno-data-sections", "-fuse-ld=lld", "-Werror",
                    "-r", "-v"
                ]
                newCompiler.linkOptions = [
                    "--target=powerpc-gekko-ibm-kuribo-eabi", "-fdeclspec", "-std=gnu++17",
                    "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math",
                    "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array",
                    "-fno-use-cxa-atexit", "-fno-c++-static-destructors", "-fno-function-sections",
                    "-fno-data-sections", "-fuse-ld=lld", "-fpermissive", "-Werror",
                    optimize, "-r", "-v"
                ]
            elif compiler == CompilerKind.GCC:
                newCompiler = KuriboGCCCompiler(
                    COMPILER_PATH,
                    startAddress,
                    debugLevel,
                    region,
                    False,
                    logger
                )
                newCompiler.cxxOptions = [
                    "-nodefaultlibs", "-nostdlib", "-std=gnu++20",
                    "-fno-exceptions", "-fno-rtti", "-ffast-math",
                    "-fpermissive", "-Wall", optimize, "-r"
                ]
                newCompiler.cOptions = [
                    "-nodefaultlibs", "-nostdlib", "-fno-exceptions",
                    "-fno-rtti", "-ffast-math", "-fpermissive",
                    "-Wall", optimize, "-r"
                ]
                newCompiler.sOptions = [
                    "-nodefaultlibs", "-nostdlib", "-fno-exceptions",
                    "-fno-rtti", "-Wall", optimize, "-r"
                ]
                newCompiler.linkOptions = [
                    "-nodefaultlibs", "-nostdlib", "-std=gnu++20",
                    "-fno-exceptions", "-fno-rtti", "-ffast-math",
                    "-fpermissive", "-Wall", optimize, "-r"
                ]
            return newCompiler
        else:
            raise ValueError("Unknown compiler kind provided")


def main():
    parser = argparse.ArgumentParser(
        "SMS-worker", description="C++ worker for SMS NTSC-U, using Kamek by Treeki")

    parser.add_argument("src", help="Source(s) to compile", nargs="+")
    parser.add_argument("--compiler", choices=[
                        CompilerKind.CODEWARRIOR, CompilerKind.CLANG, CompilerKind.GCC],
                        default=CompilerKind.CODEWARRIOR, help="Compiler to build with")
    parser.add_argument("-p", "--patcher", help="Game patcher",
                        choices=[KernelKind.KAMEK, KernelKind.KURIBO], default=KernelKind.KAMEK)
    parser.add_argument("-r", "--region", help="Game region",
                        choices=[Region.US, Region.EU, Region.JP, Region.KR], default=Region.US)
    parser.add_argument("--dolfile", help="SMS NTSC-U DOL")
    parser.add_argument("--map", help="Linker map")
    parser.add_argument("--dest", help="Destination file")
    parser.add_argument("-b", "--build", help="Build type; R=Release, D=Debug",
                        choices=["R", "D", "RD"], default="D")
    parser.add_argument("-o", "--optimize-level", help="Optimization strength",
                        default="2")
    parser.add_argument(
        "--dump", help="Dump raw code to bin file during dol patching", action="store_true")
    parser.add_argument(
        "--defines", help="Definitions before compile; Input definitions as a comma separated list", default="")
    parser.add_argument(
        "--startaddr", help="Starting address for the linker and code", default=0x80000000, type=lambda x: int(x, 0)
    )

    args = parser.parse_args()

    if args.defines.strip() != "":
        defines = args.defines.split(",")
        defines = " ".join([f"-D{d}" for d in defines])
    else:
        defines = ""

    worker = CompilerFactory.create(
        KernelKind(args.patcher),
        CompilerKind(args.compiler),
        args.startaddr,
        f"-O{args.optimize}",
        {
            "R": BuildKind.RELEASE,
            "RD": BuildKind.RELEASE_DEBUG,
            "D": BuildKind.DEBUG
        }[args.build],
        Region(args.region)
    )

    worker.defines = [
        Define(*d.strip().split("="))
        for d in args.defines.split(",")
    ]

    worker.run(args.src, args.dolfile)
    worker.logger.log("Built Successfully")

if __name__ == "__main__":
    main()
