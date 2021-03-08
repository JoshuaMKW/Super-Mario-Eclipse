import argparse
import atexit
import json
import shutil
import subprocess

from io import BytesIO
from pathlib import Path
from typing import List, Optional, Union

import pykamek
from dolreader.dol import DolFile, write_uint32

TMPDIR = Path("tmp-compiler")

@atexit.register
def clean_resources():
    if TMPDIR.is_dir():
        pass#shutil.rmtree(TMPDIR)

class Define(object):
    def __init__(self, name: str, attr: Optional[str] = None):
        self.name = name
        self.attr = "1" if attr is None else attr

    def is_defined(self) -> bool:
        return self.attr is not None

    def has_attr(self) -> bool:
        return self.is_defined() and self.attr != "1"

    def __format__(self, fmt: str) -> str:
        return f"{self.name}={self.attr}"

class Compiler(object):

    class Compilers:
        UNKNOWN = -1
        CLANG = 0
        GCC = 1
        CODEWARRIOR = 2

    def __init__(self, compilerPath: Path,
                 dest: Optional[Union[str, Path]] = None,
                 linker: Optional[Union[str, Path]] = None,
                 dump: bool = False,
                 startaddr: int = 0x80000000):
        if isinstance(compilerPath, str):
            compilerPath = Path(compilerPath)
        
        self.dest = Path(dest) if isinstance(dest, str) else dest
        self.linker = Path(linker) if isinstance(linker, str) else linker
        self.dump = dump
        self.startaddr = startaddr

        self._compilers = {}
        self._includes = ""
        self._defines = ""
        self._options = ""

        self._init_compilers(compilerPath)
        self._init_includes()

    @property
    def compiler(self) -> Compilers:
        if "mwcceppc" in self._compilers and "mwasmeppc" in self._compilers:
            return Compiler.Compilers.CODEWARRIOR
        elif "clang" in self._compilers or "clang++" in self._compilers:
            return Compiler.Compilers.CLANG
        elif "gcc" in self._compilers or "g++" in self._compilers:
            return Compiler.Compilers.GCC
        else:
            return Compiler.Compilers.UNKNOWN

    @property
    def options(self) -> List[str]:
        return self._options

    @options.setter
    def options(self, opts: Union[str, list]):
        if isinstance(opts, str):
            self._options = [opts]
        else:
            self._options = opts

    @property
    def includes(self) -> List[str]:
        if self.is_clang():
            return self._includes.split("-I")
        elif self.is_gcc():
            return self._includes.split("-I")
        elif self.is_codewarrior():
            return self._includes.split("-i ")

    @includes.setter
    def includes(self, incs: List[str]):
        if self.is_clang():
            self._includes = [f'-I{inc}' for inc in incs]
        elif self.is_gcc():
            self._includes = [f'-I{inc}' for inc in incs]
        elif self.is_codewarrior():
            self._includes = [f'-i {inc}' for inc in incs]

    @property
    def defines(self) -> List[str]:
        if self.is_clang():
            return self._defines.split("-D ")
        elif self.is_gcc():
            return self._defines.split("-D")
        elif self.is_codewarrior():
            return self._defines.split("-D")

    @defines.setter
    def defines(self, defs: List[Define]):
        if self.is_clang():
            self._defines = [f'-D{d}' for d in defs]
        elif self.is_gcc():
            self._defines = [f'-D{d}' for d in defs]
        elif self.is_codewarrior():
            self._defines = [f'-D{d}' for d in defs]

    def is_clang(self) -> bool:
        return self.compiler == Compiler.Compilers.CLANG

    def is_codewarrior(self) -> bool:
        return self.compiler == Compiler.Compilers.CODEWARRIOR

    def is_gcc(self) -> bool:
        return self.compiler == Compiler.Compilers.GCC

    def run(self, src: Union[str, Path], dol: Optional[Union[str, Path]] = None) -> Optional[Union[List[Path], Path]]:
        if self.is_codewarrior():
            _codewarriorCpp = self._compilers["mwcceppc"]
            _codewarriorAsm = self._compilers["mwasmeppc"]

            buildDir = Path("KAMEK-BUILD")
            if not buildDir.exists():
                buildDir.mkdir()

            if not TMPDIR.exists():
                TMPDIR.mkdir()

            if isinstance(src, str):
                src = Path(src)

            if isinstance(dol, str):
                dol = Path(dol)

            if self.dest is None:
                self.dest = buildDir / dol.name

            if self.linker is not None:
                self.linker = Path(self.linker)

            dumpCode = buildDir / "source.code"
            tmpDumpCode = TMPDIR / "source.tmp"
            tmpDumpDol = TMPDIR / self.dest.name

            preCompilesDir = Path("src/objects")
            linkableObjects = [str((preCompilesDir / f).resolve()) for f in preCompilesDir.iterdir() if f.is_file() and f.suffix == ".o"]

            if dol is not None:
                if not self.dest.parent.exists():
                    self.dest.mkdir(parents=True, exist_ok=True)

                if self.dest.is_file():
                    self.dest.unlink()

                if self.linker:
                    cmdtype = ["--extern", f"{self.linker.resolve()}", "--input-dol", f"{dol.resolve()}", "--output-dol", f"{self.dest}"]
                else:
                    cmdtype = ["--input-dol", f"{dol.resolve()}", "--output-dol", f"{tmpDumpDol}"]

                if self.dump:
                    cmdtype.extend(["--output-code", f"{dumpCode}", "--output-code", f"{tmpDumpCode}"])
                else:
                    cmdtype.extend(["--output-code", f"{tmpDumpCode}"])
            else:
                if self.dest is None:
                    self.dest = dumpCode

                if self.dest.is_file():
                    self.dest.unlink()

                if self.linker:
                    cmdtype = ["--extern", f"{self.linker.resolve()}", "--output-code", f"{tmpDumpCode}"]
                else:
                    cmdtype = ["--output-code", f"{tmpDumpCode}"]

            print("Compiling source code...\n")

            _errors = []

            if src.is_file() and src.suffix in (".c", ".cpp", ".cxx", ".c++", ".s"):
                tmpfile = TMPDIR / "tmp.o"
                if src.suffix == ".s":
                    output = subprocess.run(f"\"{_codewarriorAsm.resolve()}\" \"{src.resolve()}\" {self.includes} {self._defines} -o {tmpfile}", capture_output=True, text=True)
                else:
                    output = subprocess.run(f"\"{_codewarriorCpp.resolve()}\" \"{src.resolve()}\" {self.includes} {self._defines} {self.options} -o {tmpfile}", capture_output=True, text=True)

                if output.stdout.strip() != "":
                    _errors.append(output.stdout.strip())
                    print(f"✘   {src}")
                else:
                    objects = str(tmpfile) + " " + " ".join(linkableObjects)
                    print(f"✔   {src}")

            elif src.is_dir():
                objects = []
                for i, f in enumerate(src.rglob("*")):
                    tmpfile = TMPDIR / f"tmp{i}.o"
                    if f.is_file() and f.suffix in (".c", ".cpp", ".cxx", ".c++", ".s"):
                        if f.suffix == ".s":
                            output = subprocess.run(f"\"{_codewarriorAsm.resolve()}\" \"{f.resolve()}\" {self.includes} {self._defines} -o {tmpfile}", capture_output=True, text=True)
                        else:
                            output = subprocess.run(f"\"{_codewarriorCpp.resolve()}\" \"{f.resolve()}\" {self.includes} {self._defines} {self.options} -o {tmpfile}", capture_output=True, text=True)

                        if output.stdout.strip() != "":
                            _errors.append(output.stdout.strip())
                            print(f"✘   {f}")
                        else:
                            print(f"✔   {f}")
                            objects.append(str(tmpfile))

                objects = " ".join(objects)

            if len(_errors) > 0:
                print("\n--WARNING--")
                print("\n".join(_errors))

            print("")

            args = [*objects.split(" "), "--static", f"0x{self.startaddr:08X}", *cmdtype]
            pykamek.main(args)

            if dol is not None:
                _doldata = DolFile(BytesIO(self.dest.read_bytes()))
                
                self.alloc_from_heap(_doldata, tmpDumpCode.stat().st_size + 0x8000)
                with self.dest.open("wb") as dest:
                    _doldata.save(dest)

            return None
        elif self.is_clang():
            _clangCpp: Path = self._compilers["clang"]

            if not TMPDIR.exists():
                TMPDIR.mkdir()

            if isinstance(src, str):
                src = Path(src)

            if self.linker is not None:
                self.linker = Path(self.linker)

            print("Compiling source code...\n")

            tmpfile = TMPDIR / f"tmp.o"

            if src.is_file() and src.suffix in (".c", ".cpp", ".cxx", ".c++", ".s"):
                subprocess.run(f'"{_clangCpp.resolve()}" "{src.resolve()}" {self._includes} {self._defines} {self._options} -o "{tmpfile}"',
                               text=True)
            elif src.is_dir():
                objects = [f"{f.resolve()}" for f in src.rglob("*") if f.suffix in (".c", ".cpp", ".cxx", ".c++", ".s")]
                print(" ".join([str(_clangCpp.resolve()), *objects, *self._includes, *self._defines, *self.options, "-o", str(tmpfile)]))
                subprocess.run([str(_clangCpp.resolve()), *objects, *self._includes, *self._defines, *self.options, "-o", str(tmpfile)],
                               text=True)

            kxefile = tmpfile.with_suffix(".kxe")
            subprocess.run(["tools/KuriboConverter/KuriboConverter.exe", str(tmpfile), str(kxefile), str(self.linker.resolve())],
                           text=True)

            return kxefile
        elif self.is_gcc():
            _gccCpp: Path = self._compilers["gcc"]

            if not TMPDIR.exists():
                TMPDIR.mkdir()

            if isinstance(src, str):
                src = Path(src)

            if self.linker is not None:
                self.linker = Path(self.linker)

            print("Compiling source code...\n")

            tmpfile = TMPDIR / f"tmp.o"

            if src.is_file() and src.suffix in (".c", ".cpp", ".cxx", ".c++", ".s"):
                subprocess.run(f'"{_gccCpp.resolve()}" "{src.resolve()}" {self._includes} {self._defines} {self._options} -o "{tmpfile}"',
                               text=True)
            elif src.is_dir():
                objects = " ".join([f"{f.resolve()}" for f in src.rglob("*") if f.suffix in (".c", ".cpp", ".cxx", ".c++", ".s")])
                subprocess.run(f'"{_gccCpp.resolve()}" "{objects}" {self._includes} {self._defines} {self._options} -o "{tmpfile}"',
                               text=True)

            kxefile = tmpfile.with_suffix(".kxe")
            subprocess.run(f'tools/KuriboConverter/KuriboConverter.exe "{tmpfile}" "{kxefile}" "{self.linker.resolve()}"',
                           text=True)

            return kxefile

    def _alloc_from_heap(self, dol: DolFile, size: int):
        size = (size + 31) & -32
        dol.seek(0x80341E74)
        write_uint32(dol, 0x3C600000 | (((self.startaddr + size) >> 16) & 0xFFFF))
        write_uint32(dol, 0x60630000 | ((self.startaddr + size) & 0xFFFF))

        dol.seek(0x80341EAC)
        write_uint32(dol, 0x3C600000 | (((self.startaddr + size) >> 16) & 0xFFFF))
        write_uint32(dol, 0x60630000 | ((self.startaddr + size) & 0xFFFF))

    def _init_compilers(self, path: Path):
        for f in path.iterdir():
            if f.is_file() and f.suffix == ".exe":
                self._compilers[f.stem] = f

    def _init_includes(self):
        cppConfig = Path(".vscode/c_cpp_properties.json")

        if cppConfig.exists():
            with cppConfig.open("r") as config:
                jsonData = json.load(config)

            self.includes = [Path(i.replace(r"${workspaceFolder}", str(Path.cwd())))
                             for i in jsonData["configurations"][0]["includePath"]]


def main():
    parser = argparse.ArgumentParser("SMS-Patcher", description="C++ Patcher for SMS NTSC-U, using Kamek by Treeki")

    parser.add_argument("src", help="Source(s) to compile", nargs="+")
    parser.add_argument("--dolfile", help="SMS NTSC-U DOL")
    parser.add_argument("--map", help="Linker map")
    parser.add_argument("--dest", help="Destination file")
    parser.add_argument("--dump", help="Dump raw code to bin file during dol patching", action="store_true")
    parser.add_argument("--defines", help="Definitions before compile; Input definitions as a comma separated list", default=" ")
    parser.add_argument("--startaddr", help="Starting address for the linker and code", default="0x80000000")

    args = parser.parse_args()

    if args.defines.strip() != "":
        defines = args.defines.split(",")
        defines = " ".join([f"-D{d}" for d in defines])
    else:
        defines = ""

    worker = Compiler(Path("src/compiler"),
                      dest=args.dest,
                      linker=args.map,
                      dump=args.dump,
                      startaddr=int(args.startaddr, 16))

    worker.defines = [Define(*d.strip().split("=")) for d in args.defines.split(",")]

    if worker.is_codewarrior():
        worker.options = ["-Cpp_exceptions off", "-gccinc", "-gccext on", "-enum int", "-fp hard", "-use_lmw_stmw on", "-O4,p", "-c", "-rostr", "-sdata 0", "-sdata2 0"]
    elif worker.is_clang():
         worker.options = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-std=gnu++17", "-fno-exceptions", "-fno-rtti", "-ffast-math",
                            "-flto", "-nodefaultlibs", "-nostdlib", "-fuse-ld=lld", "-fpermissive", "-Wall", "-O3", "-r", "-v"]
    elif worker.is_gcc():
        worker.options = ["-nodefaultlibs", "-nostdlib", "-std=gnu++20", "-fno-exceptions", "-fno-rtti", "-ffast-math", "-fpermissive", "-Wall", "-O3", "-r"]

    worker.run(args.src, args.dolfile)

    print("\nCompiled Successfully\n")

if __name__ == "__main__":
    main()