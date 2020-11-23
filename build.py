import argparse
import os
import subprocess
import sys

from dolreader import DolFile, write_uint32

ARENASTART = 0x80427800

class Compiler(object):

    def __init__(self, kamek: str, compiler: str, options: str, dest: str = "src.kmk", linker: str = None):
        if options.strip().endswith(".o"):
            options = " ".join(options.split(" ")[:-1])
        
        self.kamek = kamek
        self.prog = compiler
        self.options = options
        self.dest = dest
        self.linker = linker

    @staticmethod
    def alloc_from_heap(dol: DolFile, size: int):
        size = (size + 31) & -32
        dol.seek(0x80341E74)
        write_uint32(dol, 0x3C600000 | (((ARENASTART + size) >> 16) & 0xFFFF))
        write_uint32(dol, 0x60630000 | ((ARENASTART + size) & 0xFFFF))

        dol.seek(0x80341EAC)
        write_uint32(dol, 0x3C600000 | (((ARENASTART + size) >> 16) & 0xFFFF))
        write_uint32(dol, 0x60630000 | ((ARENASTART + size) & 0xFFFF))

    def run(self, src: str, dol: str = None):
        if dol is not None:
            if self.dest != "src.kmk":
                _pth = self.dest
            else:
                _pth = os.path.join("KAMEK-BUILD", os.path.basename(dol))

            if os.path.isfile(_pth):
                os.remove(_pth)

            if self.linker:
                cmdtype = f"-externals={self.linker} -input-dol={dol} -output-dol={_pth} -output-code=code.tmp"
            else:
                cmdtype = f"-input-dol={dol} -output-dol={_pth} -output-code=code.tmp"
        else:
            if os.path.isfile(self.dest):
                os.remove(self.dest)

            if self.linker:
                cmdtype = f"-externals={self.linker} -output-code={self.dest}"
            else:
                cmdtype = f"-output-code={self.dest}"

        print(cmdtype)

        if isinstance(src, str):
            subprocess.run(f"\"{self.prog}\" \"{src}\" {self.options} tmp.o")
            output = subprocess.run(f"\"{self.kamek}\" tmp.o  gpr_saving.o -static=0x{ARENASTART:08X} {cmdtype}",
                                    universal_newlines=True,
                                    capture_output=True)

            if output.stderr:
                os.remove("tmp.o")
                raise RuntimeError(output.stderr)
        else:
            object_files = []
            for i in range(len(src)):
                subprocess.run(f"\"{self.prog}\" \"{src[i]}\" {self.options} tmp{i}.o")
                object_files.append(f"tmp{i}.o")

            objects = " ".join(object_files)
            output = subprocess.run(f"\"{self.kamek}\" {objects} gpr_saving.o -static=0x{ARENASTART:08X} {cmdtype}",
                                    universal_newlines=True,
                                    capture_output=True)
            
            if output.stderr:
                for i in range(len(src)):
                    os.remove(f"tmp{i}.o")
                raise RuntimeError(output.stderr)

        if dol is not None:
            with open(_pth, "rb") as dolfile:
                _doldata = DolFile(dolfile)
            
            self.alloc_from_heap(_doldata, os.stat("code.tmp").st_size + 0x500)
            with open(self.dest, "wb") as dest:
                _doldata.save(dest)

        return

if __name__ == "__main__":
    parser = argparse.ArgumentParser("SMS-Patcher", description="C++ Patcher for SMS NTSC-U, using Kamek by Treeki")

    parser.add_argument("src", help="Source(s) to compile", nargs="+")
    parser.add_argument("--dolfile", help="SMS NTSC-U DOL")
    parser.add_argument("--map", help="Linker map")
    parser.add_argument("--dest", help="Destination file")

    args = parser.parse_args()

    worker = Compiler(os.path.join("compiler", "Kamek.exe"), os.path.join("compiler", "mwcceppc.exe"), "-gccinc -gccext on -enum int -fp hard -use_lmw_stmw on -O3,p -c -rostr -sdata 0 -sdata2 0 -o", args.dest, args.map)
    worker.run(args.src, args.dolfile)

    print("\nCompiled Successfully\n")