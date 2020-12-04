import argparse
import atexit
import os
import shutil
import subprocess
import sys

from dolreader import DolFile, write_uint32

TMPDIR = "tmp-Kamek"

@atexit.register
def clean_resources():
    if os.path.isdir(TMPDIR):
        shutil.rmtree(TMPDIR)

class Compiler(object):

    def __init__(self, compilerPath: str, options: str, dest: str = None, linker: str = None, dump: bool = False, startaddr: int = 0x80000000):
        if options.strip().endswith(".o"):
            options = " ".join(options.split(" ")[:-1])

        self.compilers = {}
        self._init_compilers(compilerPath)
        
        self.options = options
        self.dest = dest
        self.linker = linker
        self.dump = dump
        self.startaddr = startaddr

    def alloc_from_heap(self, dol: DolFile, size: int):
        size = (size + 31) & -32
        dol.seek(0x80341E74)
        write_uint32(dol, 0x3C600000 | (((self.startaddr + size) >> 16) & 0xFFFF))
        write_uint32(dol, 0x60630000 | ((self.startaddr + size) & 0xFFFF))

        dol.seek(0x80341EAC)
        write_uint32(dol, 0x3C600000 | (((self.startaddr + size) >> 16) & 0xFFFF))
        write_uint32(dol, 0x60630000 | ((self.startaddr + size) & 0xFFFF))

    def run(self, src: str, dol: str = None):
        _kamek = self.compilers["Kamek"]
        _codewarriorCpp = self.compilers["mwcceppc"]
        _codewarriorAsm = self.compilers["mwasmeppc"]

        if not os.path.isdir("KAMEK-BUILD"):
            os.mkdir("KAMEK-BUILD")

        if not os.path.isdir(TMPDIR):
            os.mkdir(TMPDIR)

        dumpCode = os.path.join("KAMEK-BUILD", "source.code")
        tmpDumpCode = os.path.join(TMPDIR, "source.tmp")
        linkableObjects = [os.path.join("objects", f) for f in os.listdir("objects") if os.path.isfile(os.path.join("objects", f)) and os.path.splitext(f)[1] == ".o"]

        if dol is not None:
            if self.dest is None:
                self.dest = os.path.join("KAMEK-BUILD", os.path.basename(dol))

            if not os.path.isdir(os.path.dirname(self.dest)):
                os.makedirs(self.dest, exist_ok=True)

            if os.path.isfile(self.dest):
                os.remove(self.dest)

            if self.linker:
                cmdtype = f"-externals={self.linker} -input-dol={dol} -output-dol={self.dest}"
            else:
                cmdtype = f"-input-dol={dol} -output-dol={self.dest}"

            if self.dump:
                cmdtype += f" -output-code={dumpCode} -output-code={tmpDumpCode}"
            else:
                cmdtype += f" -output-code={tmpDumpCode}"
        else:
            if self.dest is None:
                self.dest = dumpCode

            if os.path.isfile(self.dest):
                os.remove(self.dest)

            if self.linker:
                cmdtype = f"-externals={self.linker} -output-code={self.dest}"
            else:
                cmdtype = f"-output-code={self.dest}"

        if isinstance(src, str):
            tmpfile = os.path.join(TMPDIR, f"tmp.o")
            subprocess.run(f"\"{_codewarriorCpp}\" \"{src}\" {self.options} {tmpfile}")

            objects = tmpfile + " " + " ".join(linkableObjects)
            output = subprocess.run(f"\"{_kamek}\" {objects} -static=0x{self.startaddr:08X} {cmdtype}",
                                    universal_newlines=True,
                                    capture_output=True)

            if output.stderr:
                raise RuntimeError(output.stderr)
        else:
            object_files = []
            for i in range(len(src)):
                tmpfile = os.path.join(TMPDIR, f"tmp{i}.o")
                if os.path.splitext(src[i])[1] == ".s":
                    subprocess.run(f"\"{_codewarriorAsm}\" \"{src[i]}\" -o {tmpfile}")
                else:
                    subprocess.run(f"\"{_codewarriorCpp}\" \"{src[i]}\" {self.options} {tmpfile}")

                object_files.append(tmpfile)

            objects = " ".join(object_files) + " " + " ".join(linkableObjects)
            output = subprocess.run(f"\"{_kamek}\" {objects} -static=0x{self.startaddr:08X} {cmdtype}",
                                    universal_newlines=True,
                                    capture_output=True)
            
            if output.stderr:
                raise RuntimeError(output.stderr)

        if dol is not None:
            with open(self.dest, "rb") as dolfile:
                _doldata = DolFile(dolfile)
            
            self.alloc_from_heap(_doldata, os.stat(tmpDumpCode).st_size + 0x500)
            with open(self.dest, "wb") as dest:
                _doldata.save(dest)

        return

    def _init_compilers(self, path: str):
        for file in os.listdir(path):
            if os.path.isfile(os.path.join(path, file)) and os.path.splitext(file)[1] == ".exe":
                self.compilers[os.path.splitext(os.path.basename(file))[0]] = os.path.join(path, file)

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
        defines = " " + " ".join([f"-D{d}" for d in defines])
    else:
        defines = " "

    worker = Compiler("compiler",
                      f"-Cpp_exceptions off -gccinc -gccext on -enum int -fp hard -use_lmw_stmw on -O4,p -c -rostr -sdata 0 -sdata2 0{defines} -o",
                      dest=args.dest,
                      linker=args.map,
                      dump=args.dump,
                      startaddr=int(args.startaddr, 16))

    worker.run(args.src, args.dolfile)

    print("\nCompiled Successfully\n")

if __name__ == "__main__":
    main()