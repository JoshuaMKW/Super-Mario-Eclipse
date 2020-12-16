from __future__ import annotations

import argparse
import json
from io import BytesIO

import oead

from fnmatch import fnmatch
from pathlib import Path

from bnrparser import BNR, Regions
from compiler import Compiler

class FilePatcher(Compiler):
    class STATE:
        DEBUG = "DEBUG"
        RELEASE = "RELEASE"

    def __init__(self, build: STATE, gameDir: str, projectDir: str = Path.cwd(), startAddr: int = 0x80000000, shines: int = 120):
        if isinstance(startAddr, str):
            startAddr = int(startAddr, 16)

        self.projectDir = Path(projectDir)
        self.gameDir = Path(gameDir)
        self.state = build
        self.maxShines = shines
        self._fileTables = {}

        self._init_tables()

        if self.state == FilePatcher.STATE.RELEASE:
            destDol = Path("release/Start.dol")
        elif self.state == FilePatcher.STATE.DEBUG:
            destDol = Path("debug/Start.dol")
        else:
            raise ValueError(f"Unknown patcher state {self.state}")

        super().__init__(Path("src/compiler"),
                         "-Cpp_exceptions off -gccinc -gccext on -enum int -fp hard -use_lmw_stmw on -O4,p -c -rostr -sdata 0 -sdata2 0 {} -o",
                         self._get_matching_filepath(destDol),
                         Path("src/linker/address.map"),
                         False,
                         startAddr)

    def patch_game(self):
        releaseDir = self.projectDir / "release"
        debugDir = self.projectDir / "debug"

        print("")
        print("====== RENAMING ======".center(128))
        print("-"*128)
        self._rename_files_from_config()
        print("-"*128)

        print("")
        print("====== REPLACING | COPYING ======".center(128))

        print("-"*128)

        if self.state == FilePatcher.STATE.RELEASE:
            with Path("release/.config.json").open("r") as f:
                config = json.load(f)

            bnrPath = Path("release/" + config["gamebanner"]).resolve()

            self._compile_bnr_to_game(bnrPath)
            destPath = self.gameDir / "opening.bnr"
            print(f"{bnrPath.relative_to(Path.cwd())} -> {destPath}")

            for f in releaseDir.rglob("*"):
                relativePath = f.relative_to(Path.cwd())

                if f.name.lower() == ".config.json":
                    continue

                destPath = self._get_matching_filepath(f)

                if destPath is None:
                    if f.is_file() and str(f.relative_to(Path.cwd() / "release/")) not in config["ignore"]:
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

            defines = (f"MAX_SHINES={self.maxShines}", "SME_RELEASE")
            
            self.options = self.options.format(" ".join([f"-D{d}" for d in defines]))
            if (releaseDir / "system/Start.dol").exists():
                print("")
                print("====== DOL PATCHING ======".center(128))
                print("-"*128)
                print(f"Generating {self.maxShines} shines RELEASE build")

                self.run(Path("src/src-code"), releaseDir / "system/Start.dol")

                print("-"*128)

        elif self.state == FilePatcher.STATE.DEBUG:
            with Path("debug/.config.json").open("r") as f:
                config = json.load(f)

            bnrPath = Path("debug/" + config["gamebanner"]).resolve()

            self._compile_bnr_to_game(bnrPath)
            destPath = self.gameDir / "opening.bnr"
            print(f"{bnrPath.relative_to(Path.cwd())} -> {destPath}")

            for f in debugDir.rglob("*"):
                relativePath = f.relative_to(Path.cwd())

                if f.name.lower() == ".config.json":
                    continue

                destPath = self._get_matching_filepath(f)

                if destPath is None:
                    if f.is_file() and str(f.relative_to(Path.cwd() / "debug/")) not in config["ignore"]:
                        print(f"{relativePath} -> No destination found")
                    continue

                if not destPath.parent.exists():
                    destPath.parent.mkdir(parents=True, exist_ok=True)

                if f.suffix.lower() in (".szs", ".arc"):
                    destPath.with_suffix(".szs")
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

            defines = (f"MAX_SHINES={self.maxShines}", "SME_DEBUG")
                
            self.options = self.options.format(" ".join([f"-D{d}" for d in defines]))
            if (debugDir / "system/Start.dol").exists():
                print("")
                print("====== DOL PATCHING ======".center(128))
                print("-"*128)
                print(f"Generating {self.maxShines} shines DEBUG build")
                
                self.run(Path("src/src-code"), debugDir / "system/Start.dol")
            
                print("-"*128)
            
        else:
            raise ValueError(f"Unknown patcher state {self.state}")

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
        if self.state == FilePatcher.STATE.RELEASE:
            with Path("release/.config.json").open("r") as f:
                config = json.load(f)
                for _set in config["userfiles"]:
                    glob = list(_set.keys())[0]
                    if fnmatch(str(path).lower(), "*/release/" + glob.strip().lower()):
                        if _set[glob]["rename"].strip() == "":
                            return self.gameDir / _set[glob]["destination"].strip() / path.name
                        else:
                            return self.gameDir / _set[glob]["destination"].strip() / _set[glob]["rename"].strip()
        
        elif self.state == FilePatcher.STATE.DEBUG:
            with Path("debug/.config.json").open("r") as f:
                config = json.load(f)
                for _set in config["userfiles"]:
                    glob = list(_set.keys())[0]
                    if fnmatch(str(path).lower(), "*/debug/" + glob.strip().lower()):
                        if _set[glob]["rename"].strip() == "":
                            return self.gameDir / _set[glob]["destination"].strip() / path.name
                        else:
                            return self.gameDir / _set[glob]["destination"].strip() / _set[glob]["rename"].strip()

        return None
        
    def _rename_files_from_config(self):
        _renamed = []

        if self.state == FilePatcher.STATE.RELEASE:
            with Path("release/.config.json").open("r") as f:
                config = json.load(f)
                for path in config["rename"]:
                    rename = config["rename"][path]

                    absPath = self.gameDir / path

                    if absPath.exists() and absPath not in _renamed:
                        print(f"{absPath} -> {self.gameDir / absPath.parent / rename}")
                        absPath.rename(self.gameDir / absPath.parent / rename)
                        _renamed.append(absPath)
                    
        elif self.state == FilePatcher.STATE.DEBUG:
            with Path("debug/.config.json").open("r") as f:
                config = json.load(f)
                for path in config["rename"]:
                    rename = config["rename"][path]

                    absPath = self.gameDir / path

                    if absPath.exists() and absPath not in _renamed:
                        print(f"{absPath} -> {self.gameDir / absPath.parent / rename}")
                        absPath.rename(self.gameDir / absPath.parent / rename)
                        _renamed.append(absPath)

    def _delete_files_from_config(self):
        if self.state == FilePatcher.STATE.RELEASE:
            with Path("release/.config.json").open("r") as f:
                config = json.load(f)
                for path in config["delete"]:
                    absPath = self.gameDir / path

                    if absPath.exists():
                        print(f"{absPath} -> DELETED")
                        absPath.unlink()
                        
        elif self.state == FilePatcher.STATE.DEBUG:
            with Path("debug/.config.json").open("r") as f:
                config = json.load(f)
                for path in config["delete"]:
                    absPath = self.gameDir / path

                    if absPath.exists():
                        print(f"{absPath} -> DELETED")
                        absPath.unlink()

    def _compile_bnr_to_game(self, path):
        bnr = BNR(path, Regions.AMERICA)
        bnr.save_bnr(self.gameDir / "opening.bnr")


def main():
    parser = argparse.ArgumentParser("SMS-Patcher", description="C++ Patcher for SMS NTSC-U, using Kamek by Treeki")

    parser.add_argument("gamefolder", help="root folder of GCR extracted ISO")
    parser.add_argument("-p", "--projectfolder", help="project folder used to patch game", metavar="PATH")
    parser.add_argument("-s", "--startaddr", help="Starting address for the linker and code", default="0x80000000", metavar="ADDR")
    parser.add_argument("-b", "--build", help="Build type", choices=["R", "D"], default="D")
    parser.add_argument("--shines", help="Max shines allowed", type=int, default=120)

    args = parser.parse_args()

    if args.shines < 0 or args.shines > 999:
        parser.error(f"Shine count is beyond the inclusive range (0 - 999): {args.shines}")

    if not args.projectfolder:
        args.projectfolder = Path.cwd()
    
    if not args.startaddr:
        args.startaddr = 0x80000000

    if args.build == "D":
        build = FilePatcher.STATE.DEBUG
    else:
        build = FilePatcher.STATE.RELEASE

    

    patcher = FilePatcher(build, args.gamefolder, args.projectfolder, args.startaddr, args.shines)
    patcher.patch_game()

if __name__ == "__main__":
    main()