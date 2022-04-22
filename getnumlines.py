import sys

from pathlib import Path
from typing import TextIO

def getNumLines(f: TextIO) -> int:
    return f.read().count(b"\n")

def gatherNumLinesFromDir(p: Path) -> int:
    numlines = 0
    for node in p.rglob("*.?*"):
        if node.is_file() and node.suffix.lower() in {".cxx", ".cpp", ".c", ".hxx", ".hpp", ".h", ".s", ".asm"}:
            with node.open("rb") as f:
                numlines += getNumLines(f)
    return numlines

if __name__ == "__main__":
    print(f"Number of lines in project: {gatherNumLinesFromDir(Path(sys.argv[1]))}")