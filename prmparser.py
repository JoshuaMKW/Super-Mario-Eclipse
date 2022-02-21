import argparse
import struct
from pathlib import Path
from typing import Iterable, List, Optional, Union


def calc_key_code(key: str) -> int:
    context = 0
    for char in key:
        context = ord(char) + (context * 3)
        if context > 0xFFFFFFFF:
            context -= 0x100000000
    return context & 0xFFFF


class PrmEntry(object):
    _key: str
    _keyCode: int
    _keyLen: int
    _valueLen: int
    _value: bytes

    def __init__(self, key: str, value: Union[int, bool, str, float, bytes]):
        self.key = key
        self.value = value

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({self.__dict__})"

    def __str__(self) -> str:
        return f"[PRM] {self._key} = 0x{self._value.hex()}"

    @classmethod
    def from_bytes(cls, data: bytes) -> "PrmEntry":
        keyLen = int.from_bytes(data[2:4], "big", signed=False)
        key = data[4:4+keyLen].decode()
        valueLen = int.from_bytes(data[4+keyLen:8+keyLen], "big", signed=False)
        value = data[8+keyLen:8+keyLen+valueLen]
        entry = cls(key, value)
        return entry

    @property
    def key(self) -> str:
        return self._key

    @key.setter
    def key(self, k: str):
        self._keyCode = calc_key_code(k)
        self._keyLen = len(k)
        self._key = k

    @property
    def keyCode(self) -> int:
        return self._keyCode

    @property
    def keyLen(self) -> int:
        return self._keyLen

    @property
    def value(self) -> bytes:
        return self._value

    @value.setter
    def value(self, v: Union[int, bool, str, float, bytes]):
        if isinstance(v, int):
            v = v.to_bytes(4, "big", signed=(True if v < 0 else False))
        elif isinstance(v, bool):
            v = b"\x00" if v is False else b"\x01"
        elif isinstance(v, str):
            v = v.encode("ascii")
        elif isinstance(v, float):
            v = struct.pack(">f", v)

        self._valueLen = len(v)
        self._value = v

    @property
    def valueLen(self) -> int:
        return self._valueLen


class PrmFile(object):
    def __init__(self, entries: Optional[Union[PrmEntry, List[PrmEntry]]] = None):
        if entries:
            if isinstance(entries, list):
                self._entries = entries
            else:
                self._entries = list(entries)
        else:
            self._entries = list()

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({self.__dict__})"

    def __len__(self) -> int:
        return len(self._entries)

    @classmethod
    def from_bytes(cls, data: bytes) -> "PrmFile":
        offset = 0
        entries = list()

        entryNum = int.from_bytes(data[:4], "big", signed=False)
        print(entryNum)
        for _ in range(entryNum):
            _entry = PrmEntry.from_bytes(data[4+offset:])
            entries.append(PrmEntry.from_bytes(data[4+offset:]))
            offset += 4 + _entry._keyLen + 4 + _entry._valueLen

        prm = cls(entries)
        return prm

    @classmethod
    def from_text(cls, text: str) -> "PrmFile":
        def encode_value(value: str) -> bytes:
            value = value.strip()
            if value.startswith("f32("):
                value = struct.pack(">f", float(value[4:-1].strip().rstrip("f")))
            elif value.startswith("f64("):
                value = struct.pack(">d", float(value[4:-1]))
            elif value.startswith("str("):
                value = value[4:-1].encode("ascii")
            elif value.startswith("\""):
                value = value[1:-1].encode("ascii")
            elif value.startswith("s8("):
                value = int(value[3:-1], 16 if value[3:5] in {"0x", "-0x"} else 10).to_bytes(1, "big", signed=True)
            elif value.startswith("s16("):
                value = int(value[4:-1], 16 if value[4:6] in {"0x", "-0x"} else 10).to_bytes(2, "big", signed=True)
            elif value.startswith("s32("):
                value = int(value[4:-1], 16 if value[4:6] in {"0x", "-0x"} else 10).to_bytes(4, "big", signed=True)
            elif value.startswith("s64("):
                value = int(value[4:-1], 16 if value[4:6] in {"0x", "-0x"} else 10).to_bytes(8, "big", signed=True)
            elif value.startswith("u8("):
                value = int(value[3:-1], 16 if value[3:5] in {"0x", "-0x"} else 10).to_bytes(1, "big", signed=False)
            elif value.startswith("u16("):
                value = int(value[4:-1], 16 if value[4:6] in {"0x", "-0x"} else 10).to_bytes(2, "big", signed=False)
            elif value.startswith("u32("):
                value = int(value[4:-1], 16 if value[4:6] in {"0x", "-0x"} else 10).to_bytes(4, "big", signed=False)
            elif value.startswith("u64("):
                value = int(value[4:-1], 16 if value[4:6] in {"0x", "-0x"} else 10).to_bytes(8, "big", signed=False)
            elif value.startswith("bool("):
                value = 1 if value[5:-1].lower() == "true" else 0
                value = value.to_bytes(1, "big", signed=False)
            elif value.startswith("bytes("):
                value = int(value[6:-1], 16).to_bytes(len(value[8:-1]) >> 1, "big", signed=False)
            else:
                raise ValueError(f"Invalid value type found while parsing: {value.split('(')[0]}")
            return value

        entries = list()
        for line in text.splitlines():
            line = line.strip().split("#")[0]
            if line == "":
                continue

            key, _, kvalue = line.split(maxsplit=2)
            if kvalue.startswith("list("):
                value = b""
                for item in kvalue[5:-1].split(","):
                    value += encode_value(item)
            else:
                value = encode_value(kvalue)

            entries.append(PrmEntry(key, value))

        return cls(entries)

    def to_bytes(self) -> bytes:
        data = len(self).to_bytes(4, "big", signed=False)

        for entry in self.iter_entries():
            data += entry.keyCode.to_bytes(2, "big", signed=False)
            data += entry.keyLen.to_bytes(2, "big", signed=False)
            data += entry.key.encode("ascii")
            data += entry.valueLen.to_bytes(4, "big", signed=False)
            data += entry.value

        return data

    def to_text(self) -> str:
        text = ""

        for entry in self.iter_entries():
            if entry.valueLen == 1:
                text += f"{entry.key}\t\t=  u8(0x{int.from_bytes(entry.value, 'big', signed=False):02X})\n"
            elif entry.valueLen == 2:
                text += f"{entry.key}\t\t=  u16(0x{int.from_bytes(entry.value, 'big', signed=False):04X})\n"
            elif entry.valueLen == 4:
                text += f"{entry.key}\t\t=  u32(0x{int.from_bytes(entry.value, 'big', signed=False):08X})\n"
            elif entry.valueLen == 8:
                text += f"{entry.key}\t\t=  u64(0x{int.from_bytes(entry.value, 'big', signed=False):016X})\n"
            else:
                text += f"{entry.key}\t\t=  str({entry.value.decode()})\n"

        return text.strip()

    def add_entry(self, entry: PrmEntry):
        self._entries.append(entry)

    def remove_entry(self, entry: PrmEntry):
        self._entries.remove(entry)

    def iter_entries(self) -> Iterable[PrmEntry]:
        for entry in self._entries:
            yield entry


def decode_all(path: Path, dest: Path, suffix: str = ".prm"):
    startPath = path

    def decode(path: Path, dest: Path, suffix: str):
        nonlocal startPath
        if path.is_dir():
            for p in path.iterdir():
                decode(p, dest, suffix)
        elif path.is_file():
            if path.suffix != suffix:
                print(f"[PRM-PARSER] (Invalid Extension) Ignoring {path}")
                return
                
            prm = PrmFile.from_bytes(path.read_bytes())
            if not dest.is_file() and dest.suffix == "":
                dest.mkdir(parents=True, exist_ok=True)
                try:
                    dest = dest / path.relative_to(startPath).with_suffix(suffix)
                except ValueError:
                    dest = dest.with_name(path.name).with_suffix(suffix)
            else:
                dest.parent.mkdir(parents=True, exist_ok=True)

            dest.write_bytes(prm.to_bytes())
        else:
            print(f"[PRM-PARSER] (Path Not Found) Ignoring {path}")

    decode(path, dest, suffix)


def encode_all(path: Path, dest: Path, suffix: str = ".prm"):
    startPath = path

    def encode(path: Path, dest: Path, suffix: str):
        nonlocal startPath
        if path.is_dir():
            for p in path.iterdir():
                encode(p, dest, suffix)
        elif path.is_file():
            if path.suffix != ".txt":
                print(f"[PRM-PARSER] (Invalid Extension) Ignoring {path}")
                return

            prm = PrmFile.from_text(path.read_text())
            if not dest.is_file() and dest.suffix == "":
                dest.mkdir(parents=True, exist_ok=True)
                try:
                    dest = dest / path.relative_to(startPath).with_suffix(suffix)
                except ValueError:
                    dest = (dest / path.name).with_suffix(suffix)
            else:
                dest.parent.mkdir(parents=True, exist_ok=True)

            dest.write_bytes(prm.to_bytes())
        else:
            print(f"[PRM-PARSER] (Path Not Found) Ignoring {path}")

    encode(path, dest, suffix)


def init_template(dest: Path):
    intEntry = PrmEntry("OurIntEntry", 800)
    boolEntry = PrmEntry("OurBoolEntry", False)
    strEntry = PrmEntry("OurStrEntry", "Example string")
    floatEntry = PrmEntry("OurFloatEntry", 3.14159265358979323846)
    bytesEntry = PrmEntry("OurBytesEntry", b"\x00\xD0\xC0\xDE")

    prm = PrmFile({intEntry, boolEntry, strEntry, floatEntry, bytesEntry})
    print(prm.to_text())
    dest.mkdir(parents=True, exist_ok=True)
    dest.write_text(prm.to_text())


if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='.prm parser for SMS modding',
                                     description='Create/Edit/Save/Extract .prm files',
                                     allow_abbrev=False)

    parser.add_argument('path', help='input file/folder')
    parser.add_argument("job",
                        help="Job to execute. Valid jobs are `c' (Compile), `d' (Decompile), and `i' (Init)",
                        choices=["c", "d", "i"])
    parser.add_argument('--dest',
                        help='Where to create/dump contents to',
                        metavar='filepath')

    args = parser.parse_args()

    path = Path(args.path)
    if args.dest:
        dest = Path(args.dest)
        if dest.suffix == "":
            dest.mkdir(parents=True, exist_ok=True)
        else:
            dest.parent.mkdir(parents=True, exist_ok=True)
    elif path.is_file():
        dest = path.with_suffix(".prm")
    else:
        dest = path / "out"

    if args.job == "i":
        init_template(path)
    elif args.job == "d":
        decode_all(path, dest)
    elif args.job == "c":
        encode_all(path, dest)
    else:
        parser.print_help()
