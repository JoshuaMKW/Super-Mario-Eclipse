import argparse
import atexit
import json
from os import link
import subprocess

from io import BytesIO
from pathlib import Path
from typing import List, Optional, Union

#import pykamek

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

        if self.linker and not self.linker.exists():
            raise FileNotFoundError(f"Linker map could not be found :: {self.linker}")
            
        self.dump = dump
        self.startaddr = startaddr

        self._compilers = {}
        self._includes = []
        self._defines = []
        self._cxxOptions = []
        self._cOptions = []
        self._sOptions = []
        self._linkOptions = []

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
    def cxxOptions(self) -> List[str]:
        return self._cxxOptions

    @cxxOptions.setter
    def cxxOptions(self, opts: Union[str, list]):
        if isinstance(opts, str):
            self._cxxOptions = [opts]
        else:
            self._cxxOptions = opts

    @property
    def cOptions(self) -> List[str]:
        return self._cOptions

    @cOptions.setter
    def cOptions(self, opts: Union[str, list]):
        if isinstance(opts, str):
            self._cOptions = [opts]
        else:
            self._cOptions = opts

    @property
    def sOptions(self) -> List[str]:
        return self._sOptions

    @sOptions.setter
    def sOptions(self, opts: Union[str, list]):
        if isinstance(opts, str):
            self._sOptions = [opts]
        else:
            self._sOptions = opts

    @property
    def linkOptions(self) -> List[str]:
        return self._linkOptions

    @linkOptions.setter
    def linkOptions(self, opts: Union[str, list]):
        if isinstance(opts, str):
            self._linkOptions = [opts]
        else:
            self._linkOptions = opts

    @property
    def includes(self) -> List[str]:
        self._includes

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
        return self._defines

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
            linkableObjects = []

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
                    output = subprocess.run(f"\"{_codewarriorCpp.resolve()}\" \"{src.resolve()}\" {self.includes} {self._defines} {' '.join(self.cxxOptions)} -o {tmpfile}", capture_output=True, text=True)

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
                            output = subprocess.run(f"\"{_codewarriorCpp.resolve()}\" \"{f.resolve()}\" {self.includes} {self._defines} {' '.join(self.cxxOptions)} -o {tmpfile}", capture_output=True, text=True)

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
            # pykamek.main(args)

            return tmpDumpCode
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

            if src.is_file(): 
                if src.suffix in (".cpp", ".cxx", ".c++"):
                    subprocess.run([str(_clangCpp.resolve()), str(src.resolve()), *self._includes, *self._defines, *self.cxxOptions, "-o", str(tmpfile)],
                                    text=True)
                elif src.suffix == ".c":
                    subprocess.run([str(_clangCpp.resolve()), str(src.resolve()), *self._includes, *self._defines, *self.cOptions, "-o", str(tmpfile)],
                                    text=True)
                elif src.suffix == ".s":
                    subprocess.run([str(_clangCpp.resolve()), str(src.resolve()), *self._includes, *self._defines, *self.sOptions, "-o", str(tmpfile)],
                                    text=True)
            elif src.is_dir():
                cppObjects = []
                cObjects = []
                sObjects = []
                linkObjects = []

                for f in src.rglob("*"):
                    if f.suffix in (".cpp", ".cxx", ".c++"):
                        cppObjects.append(str(f.resolve()))
                    elif f.suffix == ".c":
                        cObjects.append(str(f.resolve()))
                    elif f.suffix == ".s":
                        sObjects.append(str(f.resolve()))

                if len(cppObjects) > 0:
                    subprocess.run([str(_clangCpp.resolve()), *cppObjects, *self._includes, *self._defines, *self.cxxOptions, "-o", str(TMPDIR / f"cpp_obj.o")],
                                    text=True)
                    linkObjects.append(str(TMPDIR / f"cpp_obj.o"))
                if len(cObjects) > 0:
                    subprocess.run([str(_clangCpp.resolve()), *cObjects, *self._includes, *self._defines, *self.cOptions, "-o", str(TMPDIR / f"c_obj.o")],
                                    text=True)
                    linkObjects.append(str(TMPDIR / f"c_obj.o"))
                if len(sObjects) > 0:
                    subprocess.run([str(_clangCpp.resolve()), *sObjects, *self._includes, *self._defines, *self.sOptions, "-o", str(TMPDIR / f"s_obj.o")],
                                    text=True)
                    linkObjects.append(str(TMPDIR / f"s_obj.o"))

                if len(linkObjects) == 0:
                    return None

                print([str(_clangCpp.resolve()), *linkObjects, *self._includes, *self._defines, *self.linkOptions, "-o", str(TMPDIR / f"tmp.o")])
                subprocess.run([str(_clangCpp.resolve()), *linkObjects, *self._includes, *self._defines, *self.linkOptions, "-o", str(TMPDIR / f"tmp.o")],
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

            if src.is_file(): 
                if src.suffix in (".cpp", ".cxx", ".c++"):
                    subprocess.run([str(_gccCpp.resolve()), str(src.resolve()), *self._includes, *self._defines, *self.cxxOptions, "-o", str(TMPDIR / f"tmp.o")],
                                    text=True)
                elif src.suffix == ".c":
                    subprocess.run([str(_gccCpp.resolve()), str(src.resolve()), *self._includes, *self._defines, *self.cOptions, "-o", str(TMPDIR / f"tmp.o")],
                                    text=True)
                elif src.suffix == ".s":
                    subprocess.run([str(_gccCpp.resolve()), str(src.resolve()), *self._includes, *self._defines, *self.sOptions, "-o", str(TMPDIR / f"tmp.o")],
                                    text=True)
            elif src.is_dir():
                cppObjects = []
                cObjects = []
                sObjects = []
                linkObjects = []

                for f in src.rglob("*"):
                    if src.suffix in (".cpp", ".cxx", ".c++"):
                        cppObjects.append(str(src.resolve()))
                    elif src.suffix == ".c":
                        cObjects.append(str(src.resolve()))
                    elif src.suffix == ".s":
                        sObjects.append(str(src.resolve()))

                if len(cppObjects) > 0:
                    subprocess.run([str(_gccCpp.resolve()), str(f.resolve()), *self._includes, *self._defines, *self.cxxOptions, "-o", str(TMPDIR / f"cpp_obj.o")],
                                    text=True)
                    linkObjects.append(str(TMPDIR / f"cpp_obj.o"))
                if len(cObjects) > 0:
                    subprocess.run([str(_gccCpp.resolve()), str(f.resolve()), *self._includes, *self._defines, *self.cOptions, "-o", str(TMPDIR / f"c_obj.o")],
                                    text=True)
                    linkObjects.append(str(TMPDIR / f"c_obj.o"))
                if len(sObjects) > 0:
                    subprocess.run([str(_gccCpp.resolve()), str(f.resolve()), *self._includes, *self._defines, *self.sOptions, "-o", str(TMPDIR / f"s_obj.o")],
                                    text=True)
                    linkObjects.append(str(TMPDIR / f"s_obj.o"))

                if len(linkObjects) == 0:
                    return None

                subprocess.run([str(_gccCpp.resolve()), *linkObjects, *self._includes, *self._defines, *self.linkOptions, "-o", str(TMPDIR / f"tmp.o")],
                                text=True)
                

            kxefile = tmpfile.with_suffix(".kxe")
            subprocess.run(f'tools/KuriboConverter/KuriboConverter.exe "{tmpfile}" "{kxefile}" "{self.linker.resolve()}"',
                           text=True)

            return kxefile

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
    parser = argparse.ArgumentParser("SMS-worker", description="C++ worker for SMS NTSC-U, using Kamek by Treeki")

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

    worker = Compiler(Path("compiler"),
                      dest=args.dest,
                      linker=args.map,
                      dump=args.dump,
                      startaddr=int(args.startaddr, 16))

    worker.defines = [Define(*d.strip().split("=")) for d in args.defines.split(",")]

    if worker.is_codewarrior():
        worker.cxxOptions = ["-Cpp_exceptions off", "-gccinc", "-gccext on", "-enum int", "-fp hard", "-use_lmw_stmw on", "-O4,p", "-c", "-rostr", "-sdata 0", "-sdata2 0"]
    elif worker.is_clang():
        worker.cxxOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-std=gnu++17", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math",
                              "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fuse-ld=lld", "-fpermissive", "-Wall", "-O3", "-r", "-v"]
        worker.cOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math",
                            "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fuse-ld=lld", "-fpermissive", "-Wall", "-O3", "-r", "-v"]
        worker.sOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables",
                            "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fuse-ld=lld", "-Wall", "-r", "-v"]
        worker.linkOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-std=gnu++17", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math",
                               "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fuse-ld=lld", "-fpermissive", "-Wall", "-O3", "-r", "-v"]
    elif worker.is_gcc():
        worker.cxxOptions = ["-nodefaultlibs", "-nostdlib", "-std=gnu++20", "-fno-exceptions", "-fno-rtti", "-ffast-math", "-fpermissive", "-Wall", "-O3", "-r"]
        worker.cOptions = ["-nodefaultlibs", "-nostdlib", "-fno-exceptions", "-fno-rtti", "-ffast-math", "-fpermissive", "-Wall", "-O3", "-r"]
        worker.sOptions = ["-nodefaultlibs", "-nostdlib", "-fno-exceptions", "-fno-rtti", "-Wall", "-O3", "-r"]
        worker.linkOptions = ["-nodefaultlibs", "-nostdlib", "-std=gnu++20", "-fno-exceptions", "-fno-rtti", "-ffast-math", "-fpermissive", "-Wall", "-O3", "-r"]

    worker.run(args.src, args.dolfile)

    print("\nCompiled Successfully\n")

if __name__ == "__main__":
    main()