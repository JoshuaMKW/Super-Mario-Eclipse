import argparse
import os
import sys

from pathlib import Path
from compiler import Compiler

class FilePatcher(Compiler):
    class STATE:
        DEBUG = 0
        RELEASE = 1

    def __init__(self, gameDir: str, projectDir: str = os.getcwd(), startAddr: int = 0x80000000):
        if isinstance(startAddr, str):
            startAddr = int(startAddr, 16)

        self.projectDir = Path(projectDir)
        self.gameDir = Path(gameDir)
        self.state = FilePatcher.STATE.RELEASE
        self._fileTables = {}

        self._init_tables()

        super().__init__(Path("src/compiler"),
                         "-Cpp_exceptions off -gccinc -gccext on -enum int -fp hard -use_lmw_stmw on -O4,p -c -rostr -sdata 0 -sdata2 0 {} -o",
                         self._get_matching_filepath(Path("Start.dol")),
                         Path("src/linker/address.map"),
                         False,
                         startAddr)

    def patch_game(self):
        releaseDir = self.projectDir / "release"

        for f in releaseDir.rglob("*"):
            destPath = self._get_matching_filepath(f)
            destPath.write_bytes(f.read_bytes())

        if self.state == FilePatcher.STATE.DEBUG:
            debugDir = self.projectDir / "debug"

            for f in debugDir.rglob("*"):
                destPath = self._get_matching_filepath(f)
                destPath.write_bytes(f.read_bytes())
                
            self.options.format("--defines MAX_SHINES=300,SME_DEBUG")
            if (debugDir / "Start.dol").exists():
                try:
                    self.run(Path("src/src-code"), debugDir / "Start.dol")
                except SystemExit:
                    raise RuntimeError("Failed to patch Start.dol")
        elif self.state == FilePatcher.STATE.RELEASE:
            self.options.format("--defines MAX_SHINES=300,SME_RELEASE")
            if (releaseDir / "Start.dol").exists():
                try:
                    self.run(Path("src/src-code"), releaseDir / "Start.dol")
                except SystemExit:
                    raise RuntimeError("Failed to patch Start.dol")
        else:
            raise RuntimeError(f"Unknown patcher state {self.state}")
        
        
                

    def _init_tables(self):
        for f in self.gameDir.rglob("*"):
            if f.is_file:
                self._fileTables.setdefault(f.suffix, []).append(f)

    def _get_matching_filepath(self, path: Path) -> Path:
        try:
            for f in self._fileTables[path.suffix]:
                print(f.name, path.name)
                if f.name == path.name:
                    return f
        except KeyError:
            return None
        else:
            return None
        


def main():
    parser = argparse.ArgumentParser("SMS-Patcher", description="C++ Patcher for SMS NTSC-U, using Kamek by Treeki")

    parser.add_argument("gamefolder", help="root folder of GCR extracted ISO")
    parser.add_argument("-p", "--projectfolder", help="project folder used to patch game")
    parser.add_argument("--startaddr", help="Starting address for the linker and code", default="0x80000000")

    args = parser.parse_args()

    if not args.projectfolder:
        args.projectfolder = Path.cwd()
    
    if not args.startaddr:
        args.startaddr = 0x80000000

    patcher = FilePatcher(args.gamefolder, args.projectfolder, args.startaddr)
    print(patcher._get_matching_filepath(Path("dolpic+.sme")))
    #patcher.patch_game()

if __name__ == "__main__":
    main()