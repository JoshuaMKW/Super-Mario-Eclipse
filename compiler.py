import argparse
import atexit
import json
import shutil
import subprocess

from io import BytesIO
from pathlib import Path, PosixPath

from dolreader import DolFile, write_uint32

TMPDIR = Path("tmp-Kamek")

@atexit.register
def clean_resources():
    if TMPDIR.is_dir():
        pass#shutil.rmtree(TMPDIR)

class Compiler(object):

    def __init__(self, compilerPath: Path, options: str, dest: Path = None, linker: Path = None, dump: bool = False, startaddr: int = 0x80000000):
        if options.strip().endswith(".o"):
            options = " ".join(options.split(" ")[:-1])

        if isinstance(compilerPath, str):
            compilerPath = Path(compilerPath)

        self.compilers = {}
        self._init_compilers(compilerPath)
        
        self.options = options
        self.dest = Path(dest) if isinstance(dest, str) else dest
        self.linker = Path(linker) if isinstance(linker, str) else linker
        self.dump = dump
        self.startaddr = startaddr
        self.includes = ""

        self._init_includes()

        #print(self.includes)
        #raise SystemExit

    def alloc_from_heap(self, dol: DolFile, size: int):
        size = (size + 31) & -32
        dol.seek(0x80341E74)
        write_uint32(dol, 0x3C600000 | (((self.startaddr + size) >> 16) & 0xFFFF))
        write_uint32(dol, 0x60630000 | ((self.startaddr + size) & 0xFFFF))

        dol.seek(0x80341EAC)
        write_uint32(dol, 0x3C600000 | (((self.startaddr + size) >> 16) & 0xFFFF))
        write_uint32(dol, 0x60630000 | ((self.startaddr + size) & 0xFFFF))

    def run(self, src: Path, dol: Path = None):
        _kamek = self.compilers["Kamek"]
        _codewarriorCpp = self.compilers["mwcceppc"]
        _codewarriorAsm = self.compilers["mwasmeppc"]

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
                cmdtype = f"-externals={self.linker.resolve()} -input-dol={dol.resolve()} -output-dol={tmpDumpDol}"
            else:
                cmdtype = f"-input-dol={dol.resolve()} -output-dol={tmpDumpDol}"

            if self.dump:
                cmdtype += f" -output-code={dumpCode} -output-code={tmpDumpCode}"
            else:
                cmdtype += f" -output-code={tmpDumpCode}"
        else:
            if self.dest is None:
                self.dest = dumpCode

            if self.dest.is_file():
                self.dest.unlink()

            if self.linker:
                cmdtype = f"-externals={self.linker.resolve()} -output-code={tmpDumpCode}"
            else:
                cmdtype = f"-output-code={tmpDumpCode}"

        print("Compiling source code...\n")

        _errors = []

        print(f"\"{_codewarriorCpp.resolve()}\" {self.includes} {self.options}")

        if src.is_file() and f.suffix not in (".h", ".hpp", ".hxx"):
            tmpfile = TMPDIR / "tmp.o"
            if f.suffix == ".s":
                output = subprocess.run(f"\"{_codewarriorAsm.resolve()}\" \"{src.resolve()}\" {self.includes} -o {tmpfile}", capture_output=True, text=True)
            else:
                output = subprocess.run(f"\"{_codewarriorCpp.resolve()}\" \"{src.resolve()}\" {self.includes} {self.options} -o {tmpfile}", capture_output=True, text=True)

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
                if f.is_file() and f.suffix not in (".h", ".hpp", ".hxx"):
                    if f.suffix == ".s":
                        output = subprocess.run(f"\"{_codewarriorAsm.resolve()}\" \"{f.resolve()}\" {self.includes} -o {tmpfile}", capture_output=True, text=True)
                    else:
                        output = subprocess.run(f"\"{_codewarriorCpp.resolve()}\" \"{f.resolve()}\" {self.includes} {self.options} -o {tmpfile}", capture_output=True, text=True)

                    if output.stdout.strip() != "":
                        _errors.append(output.stdout.strip())
                        #print(f"✘   {f}")
                    else:
                        #print(f"✔   {f}")
                        objects.append(str(tmpfile))

            objects = " ".join(objects)

        if len(_errors) > 0:
            print("\n--WARNING--")
            print("\n".join(_errors))

        print("\nLinking objects...")
        output = subprocess.run(f"\"{_kamek.resolve()}\" {objects} -static=0x{self.startaddr:08X} {cmdtype}",
                                universal_newlines=True,
                                capture_output=True)

        if output.stderr:
            err = output.stderr.splitlines()[1].split(": ")[-1]
            msg = f"\nFailed!\nReason: {err}"
            print(msg)
        else:
            print("\nSuccess!")

        if dol is not None:
            _doldata = DolFile(BytesIO(tmpDumpDol.read_bytes()))
            
            self.alloc_from_heap(_doldata, tmpDumpCode.stat().st_size + 0x5000)
            with self.dest.open("wb") as dest:
                _doldata.save(dest)

        return

    def _init_compilers(self, path: Path):
        for f in path.iterdir():
            if f.is_file() and f.suffix == ".exe":
                self.compilers[f.stem] = f

    def _init_includes(self):
        cppConfig = Path(".vscode/c_cpp_properties.json")

        if cppConfig.exists():
            with cppConfig.open("r") as config:
                jsonData = json.load(config)

            for include in jsonData["configurations"][0]["includePath"]:
                if include.endswith("**"):
                    continue


                include = Path(include.replace(r"${workspaceFolder}", str(Path.cwd())))
                self.includes += f"-i {include} "

            if len(self.includes) > 0:
                self.includes = f"-I- {self.includes}"

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
                      f"-Cpp_exceptions off -gccinc -gccext on -enum int -fp hard -use_lmw_stmw on -O4,p -c -rostr -sdata 0 -sdata2 0 {defines}",
                      dest=args.dest,
                      linker=args.map,
                      dump=args.dump,
                      startaddr=int(args.startaddr, 16))

    worker.run(args.src, args.dolfile)

    print("\nCompiled Successfully\n")

if __name__ == "__main__":
    main()