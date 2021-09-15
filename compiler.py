import argparse
import atexit
import json
import os
import shutil
import subprocess

from io import BytesIO, StringIO
from pathlib import Path
from typing import List, Optional, Union

import pykamek

TMPDIR = Path("tmp-compiler")


@atexit.register
def clean_resources():
    if TMPDIR.is_dir():
        shutil.rmtree(TMPDIR)


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
        CLANG = "CLANG"
        GCC = "GCC"
        CODEWARRIOR = "CODEWARRIOR"

    class Patchers:
        KAMEK = "KAMEK"
        KURIBO = "KURIBO"

    def __init__(self, compilerPath: Path,
                 compiler: Compilers = Compilers.CODEWARRIOR,
                 dest: Optional[Union[str, Path]] = None,
                 linker: Optional[Union[str, Path]] = None,
                 patcher: Patchers = Patchers.KAMEK,
                 dump: bool = False,
                 startaddr: int = 0x80000000):
        if isinstance(compilerPath, str):
            compilerPath = Path(compilerPath)

        self.dest = Path(dest) if isinstance(dest, str) else dest
        self.linker = Path(linker) if isinstance(linker, str) else linker
        self.compiler = compiler
        self.patcher = patcher

        if self.linker and not self.linker.exists():
            raise FileNotFoundError(
                f"Linker map could not be found :: {self.linker}")

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
            self._includes = [f'-I{inc}' for inc in incs]

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

    def is_kamek(self) -> bool:
        return self.patcher == Compiler.Patchers.KAMEK

    def is_kuribo(self) -> bool:
        return self.patcher == Compiler.Patchers.KURIBO

    def run(self, src: Union[str, Path], dol: Optional[Union[str, Path]] = None, fout: Optional[StringIO] = None) -> Optional[Union[List[Path], Path]]:
        def build_objects(cppCompiler: Path, cCompiler: Path, sCompiler: Path, linker: Optional[Path] = None) -> List[str]:
            print("Compiling source code...\n")

            tmpStream = open("tmpstream_.log", "w+")

            tmpObject = "tmp.o"
            cppObjects = []
            cObjects = []
            sObjects = []
            linkObjects = [lib for lib in Path(
                "lib/").iterdir() if lib.suffix == ".a"]

            if src.is_file():
                if src.suffix in (".cpp", ".cxx", ".c++"):
                    subprocess.run([str(cppCompiler.resolve()), str(src.resolve()), *self._includes, *self._defines, *self.cxxOptions, "-o", str(tmpObject)], stdout=tmpStream, stderr=tmpStream,
                                   text=True)
                elif src.suffix == ".c":
                    subprocess.run([str(cCompiler.resolve()), str(src.resolve()), *self._includes, *self._defines, *self.cOptions, "-o", str(tmpObject)], stdout=tmpStream, stderr=tmpStream,
                                   text=True)
                elif src.suffix == ".s":
                    subprocess.run([str(sCompiler.resolve()), str(src.resolve()), *self._includes, *self._defines, *self.sOptions, "-o", str(tmpObject)], stdout=tmpStream, stderr=tmpStream,
                                   text=True)
            elif src.is_dir():
                for f in src.rglob("*"):
                    if f.suffix in (".cpp", ".cxx", ".c++"):
                        cppObjects.append(str(f.resolve()))
                    elif f.suffix == ".c":
                        cObjects.append(str(f.resolve()))
                    elif f.suffix == ".s":
                        sObjects.append(str(f.resolve()))

                if len(cppObjects) > 0:
                    print(" ".join([str(cppCompiler.resolve()), *cppObjects, *self._includes,
                                    *self._defines, *self.cxxOptions, "-o", str(TMPDIR / f"cpp_obj.o")]))
                    subprocess.run([str(cppCompiler.resolve()), *cppObjects, *self._includes, *self._defines, *self.cxxOptions, "-o", str(TMPDIR / f"cpp_obj.o")], stdout=tmpStream, stderr=tmpStream,
                                   text=True)
                    linkObjects.append(str(TMPDIR / f"cpp_obj.o"))
                if len(cObjects) > 0:
                    subprocess.run([str(cCompiler.resolve()), *cObjects, *self._includes, *self._defines, *self.cOptions, "-o", str(TMPDIR / f"c_obj.o")], stdout=tmpStream, stderr=tmpStream,
                                   text=True)
                    linkObjects.append(str(TMPDIR / f"c_obj.o"))
                if len(sObjects) > 0:
                    subprocess.run([str(sCompiler.resolve()), *sObjects, *self._includes, *self._defines, *self.sOptions, "-o", str(TMPDIR / f"s_obj.o")], stdout=tmpStream, stderr=tmpStream,
                                   text=True)
                    linkObjects.append(str(TMPDIR / f"s_obj.o"))

                if len(linkObjects) == 0:
                    tmpStream.seek(0)
                    fout.write(tmpStream.read())
                    tmpStream.close()
                    os.unlink("tmpstream_.log")
                    return None

                if linker:
                    print([str(linker.resolve()), *linkObjects,
                           *self.linkOptions, "-o", tmpObject])
                    subprocess.run([str(linker.resolve()), *linkObjects, *self.linkOptions, "-o", tmpObject], stdout=tmpStream, stderr=tmpStream,
                                   text=True)
                    tmpStream.seek(0)
                    fout.write(tmpStream.read())
                    tmpStream.close()
                    os.unlink("tmpstream_.log")

                    return [tmpObject]
                tmpStream.seek(0)
                fout.write(tmpStream.read())
                tmpStream.close()
                os.unlink("tmpstream_.log")
                return linkObjects

            
            tmpStream.seek(0)
            fout.write(tmpStream.read())
            tmpStream.close()
            os.unlink("tmpstream_.log")

        buildDir = Path("PATCHER-BUILD")
        if not buildDir.exists():
            buildDir.mkdir()

        if not TMPDIR.exists():
            TMPDIR.mkdir()

        if isinstance(src, str):
            src = Path(src)

        if isinstance(dol, str):
            dol = Path(dol)

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

        if self.is_codewarrior():
            _codewarriorCpp = self._compilers["mwcceppc"]
            _codewarriorAsm = self._compilers["mwasmeppc"]
            _codewarriorLd = self._compilers["mwldeppc"]

            if self.is_kuribo():
                obj, *_ = build_objects(_codewarriorCpp,
                                        _codewarriorCpp, _codewarriorAsm, _codewarriorLd)
                kxefile = Path(obj).with_suffix(".kxe")
                subprocess.run(["tools/KuriboConverter/KuriboConverter.exe", str(obj), str(kxefile), str(self.linker.resolve())],
                               text=True)
                return kxefile
            else:
                if self.dest.is_file():
                    self.dest.unlink()

                linkObjects = build_objects(
                    _codewarriorCpp, _codewarriorCpp, _codewarriorAsm)
                args = [*linkObjects, "--static",
                        f"0x{self.startaddr:08X}", *kCmdtype]
                pykamek.main(args)
                return tmpDumpCode

        elif self.is_clang():
            _clangCpp: Path = self._compilers["clang"]

            if self.linker is not None:
                self.linker = Path(self.linker)

            if self.is_kuribo():
                obj, *_ = build_objects(_clangCpp,
                                        _clangCpp, _clangCpp, _clangCpp)
                kxefile = Path(obj).with_suffix(".kxe")
                subprocess.run(["tools/KuriboConverter/KuriboConverter.exe", str(obj), str(kxefile), str(self.linker.resolve())],
                               text=True)
                return kxefile
            else:
                if self.dest.is_file():
                    self.dest.unlink()

                linkObjects = build_objects(_clangCpp, _clangCpp, _clangCpp)
                args = [*linkObjects, "--static",
                        f"0x{self.startaddr:08X}", *kCmdtype]
                pykamek.main(args)
                return tmpDumpCode

        elif self.is_gcc():
            _gccCpp: Path = self._compilers["gcc"]

            if self.linker is not None:
                self.linker = Path(self.linker)

            if self.is_kuribo():
                obj, *_ = build_objects(_gccCpp, _gccCpp, _gccCpp, _gccCpp)
                kxefile = Path(obj).with_suffix(".kxe")
                subprocess.run(["tools/KuriboConverter/KuriboConverter.exe", str(obj), str(kxefile), str(self.linker.resolve())],
                               text=True)
                return kxefile
            else:
                if self.dest.is_file():
                    self.dest.unlink()

                linkObjects = build_objects(_gccCpp, _gccCpp, _gccCpp)
                args = [*linkObjects, "--static",
                        f"0x{self.startaddr:08X}", *kCmdtype]
                pykamek.main(args)
                return tmpDumpCode

    def _init_compilers(self, path: Path):
        clangFolder = path / "clang"
        cwFolder = path / "codewarrior"
        gccFolder = path / "gcc"
        for f in clangFolder.iterdir():
            if f.is_file() and f.suffix == ".exe":
                self._compilers[f.stem] = f
        for f in cwFolder.iterdir():
            if f.is_file() and f.suffix == ".exe":
                self._compilers[f.stem] = f
        for f in gccFolder.iterdir():
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
    parser = argparse.ArgumentParser(
        "SMS-worker", description="C++ worker for SMS NTSC-U, using Kamek by Treeki")

    parser.add_argument("src", help="Source(s) to compile", nargs="+")
    parser.add_argument("--compiler", choices=[
                        Compiler.Compilers.CODEWARRIOR, Compiler.Compilers.CLANG, Compiler.Compilers.GCC],
                        default=Compiler.Compilers.CODEWARRIOR, help="Compiler to build with")
    parser.add_argument("--dolfile", help="SMS NTSC-U DOL")
    parser.add_argument("--map", help="Linker map")
    parser.add_argument("--dest", help="Destination file")
    parser.add_argument(
        "--dump", help="Dump raw code to bin file during dol patching", action="store_true")
    parser.add_argument(
        "--defines", help="Definitions before compile; Input definitions as a comma separated list", default=" ")
    parser.add_argument(
        "--startaddr", help="Starting address for the linker and code", default="0x80000000")

    args = parser.parse_args()

    if args.defines.strip() != "":
        defines = args.defines.split(",")
        defines = " ".join([f"-D{d}" for d in defines])
    else:
        defines = ""

    worker = Compiler(Path("compiler"),
                      compiler=args.compiler,
                      dest=args.dest,
                      linker=args.map,
                      dump=args.dump,
                      startaddr=int(args.startaddr, 16))

    worker.defines = [Define(*d.strip().split("="))
                      for d in args.defines.split(",")]

    if worker.is_codewarrior():
        worker.cxxOptions = ["-Cpp_exceptions off", "-gccinc", "-gccext on", "-enum int", "-RTTI off"
                              "-fp fmadd", "-use_lmw_stmw on", "-O4,p", "-c", "-rostr", "-sdata 0", "-sdata2 0"]
    elif worker.is_clang():
        worker.cxxOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-fdeclspec", "-std=gnu++17", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math",
                              "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fno-use-cxa-atexit", "-fno-c++-static-destructors", "-fno-function-sections", "-fno-data-sections", "-fuse-ld=lld", "-fpermissive", "-Werror", "-O3", "-r", "-v"]
        worker.cOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-fdeclspec", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math", "-fdeclspec",
                            "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fno-use-cxa-atexit", "-fno-c++-static-destructors", "-fno-function-sections", "-fno-data-sections", "-fuse-ld=lld", "-fpermissive", "-Werror", "-O3", "-r", "-v"]
        worker.sOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-fdeclspec", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables",
                            "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fno-use-cxa-atexit", "-fno-c++-static-destructors", "-fno-function-sections", "-fno-data-sections", "-fuse-ld=lld", "-Werror", "-r", "-v"]
        worker.linkOptions = ["--target=powerpc-gekko-ibm-kuribo-eabi", "-fdeclspec", "-std=gnu++17", "-fno-exceptions", "-fno-rtti", "-fno-unwind-tables", "-ffast-math",
                               "-flto", "-nodefaultlibs", "-nostdlib", "-fno-use-init-array", "-fno-use-cxa-atexit", "-fno-c++-static-destructors", "-fno-function-sections", "-fno-data-sections", "-fuse-ld=lld", "-fpermissive", "-Werror", "-O3", "-r", "-v"]
    elif worker.is_gcc():
        worker.cxxOptions = ["-nodefaultlibs", "-nostdlib", "-std=gnu++20",
                              "-fno-exceptions", "-fno-rtti", "-ffast-math", "-fpermissive", "-Wall", "-O3", "-r"]
        worker.cOptions = ["-nodefaultlibs", "-nostdlib", "-fno-exceptions",
                            "-fno-rtti", "-ffast-math", "-fpermissive", "-Wall", "-O3", "-r"]
        worker.sOptions = ["-nodefaultlibs", "-nostdlib",
                            "-fno-exceptions", "-fno-rtti", "-Wall", "-O3", "-r"]
        worker.linkOptions = ["-nodefaultlibs", "-nostdlib", "-std=gnu++20",
                               "-fno-exceptions", "-fno-rtti", "-ffast-math", "-fpermissive", "-Wall", "-O3", "-r"]

    worker.run(args.src, args.dolfile)

    print("\nCompiled Successfully\n")


if __name__ == "__main__":
    main()
