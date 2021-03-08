#!/usr/bin/env python3

#  PyiiASMH 3 (pyiiasmh_cli.py)
#  Copyright (c) 2011, 2012, Sean Power
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#      * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#      * Neither the names of the authors nor the
#        names of its contributors may be used to endorse or promote products
#        derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL SEAN POWER BE LIABLE FOR ANY
#  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import binascii
import re
import shutil
import sys
import tempfile
from argparse import ArgumentParser, Namespace
from pathlib import Path
from typing import Any, List, Optional, Tuple, Union

from pyiiasmh.errors import UnsupportedOSError
from pyiiasmh.ppctools import PpcFormatter


class PyiiAsmhApp(PpcFormatter):

    def __init__(self):
        super().__init__()

        self.opcodes = None
        self.geckocodes = None
        self.bapo = None
        self.xor = None
        self.chksum = None
        self.codetype = None

    def assemble(self, inputasm: str, outputfile: Optional[Union[str, Path]] = None, filetype: str = "text") -> str:
        tmpdir = Path(tempfile.mkdtemp(prefix="pyiiasmh-"))

        if filetype is None:
            try:
                Path(tmpdir, "code.txt").write_text(inputasm)
                inputasm = None
            except IOError:
                self.log.exception("Failed to open input file.")
                shutil.rmtree(tmpdir)
                return None

        try:
            toReturn = ""
            machine_code = self.asm_opcodes(tmpdir, inputasm)
        except UnsupportedOSError as e:
            self.log.exception(e)
            toReturn = (
                f"Your OS \"{sys.platform}\" is not supported. See \"error.log\" for details and also read the README.")
        except IOError as e:
            self.log.exception(e)
            toReturn = f"Error: {str(e)}"
        except RuntimeError as e:
            self.log.exception(e)
            toReturn = str(e)
        else:
            if self.bapo is not None:
                if self.bapo[0] not in ("8", "0") or self.bapo[1] not in ("0", "1"):
                    return f"Invalid ba/po: {self.bapo}"
                elif int(self.bapo[2], 16) > 7 and self.bapo[1] == "1":
                    return f"Invalid ba/po: {self.bapo}"

            toReturn = self.construct_code(
                machine_code, self.bapo, self.xor, self.chksum, self.codetype)
            if outputfile is not None:
                if isinstance(outputfile, str):
                    outputfile = Path(outputfile)

                if filetype == "text":
                    outputfile.write_text(toReturn)
                elif filetype == "bin":
                    outputfile.write_bytes(bytes.fromhex(
                        toReturn.replace("\n", "").replace(" ", "")))
                else:
                    outputfile.write_text(toReturn)

        shutil.rmtree(tmpdir, ignore_errors=True)
        return toReturn

    def disassemble(self,
                    inputfile,
                    outputfile: Optional[Union[str, Path]] = None,
                    filetype: str = "text",
                    cFooter: bool = True,
                    formalNames: bool = False) -> Tuple[str, Tuple[Any, Any, Any, Any]]:
        tmpdir = tempfile.mkdtemp(prefix="pyiiasmh-")
        codes = None

        if filetype == "bin":
            access = "rb"
        else:
            access = "r"

        if filetype is None:
            codes = inputfile
        else:
            try:
                with open(inputfile, access) as f:
                    codes = "".join(f.readlines())
                    if filetype == "bin":
                        codes = binascii.b2a_hex(codes).upper()
            except IOError as e:
                self.log.exception("Failed to open input file.")
                shutil.rmtree(tmpdir)
                return [f"Error: {str(e)}", (None, None, None, None)]

        rawcodes = self.deconstruct_code(codes, cFooter)

        try:
            with Path(tmpdir, "code.bin").open("wb") as f:
                rawhex = "".join("".join(rawcodes[0].split("\n")).split(" "))
                try:
                    f.write(binascii.a2b_hex(rawhex))
                except binascii.Error:
                    f.write(b"")
        except IOError:
            self.log.exception("Failed to open temp file.")

        try:
            toReturn = ["", (None, None, None, None)]
            opcodes = self.dsm_geckocodes(tmpdir, outputfile)
        except UnsupportedOSError:
            self.log.exception("")
            toReturn = [(f"Your OS \"{sys.platform}\" is not supported. " +
                         "See \"error.log\" for details and also read the README."),
                        (None, None, None, None)]
        except IOError as e:
            self.log.exception(e)
            toReturn = ["Error: " + str(e), (None, None, None, None)]
        except RuntimeError as e:
            self.log.exception(e)
            toReturn = [str(e), (None, None, None, None)]
        else:
            toReturn = [opcodes + "\n", rawcodes[1:]]

        if formalNames:
            opcodes = []
            for line in toReturn[0].split("\n"):
                values = re.sub(r"(?<!c)r1(?![0-9])", "sp", line)
                values = re.sub(r"(?<!c)r2(?![0-9])", "rtoc", values)
                opcodes.append(values)
            toReturn[0] = "\n".join(opcodes)

        shutil.rmtree(tmpdir)

        return toReturn

    def run(self, parser: ArgumentParser, args: Namespace, filetype: str = "text"):
        # Check for incorrect usage
        if args.codetype.upper() == "RAW":
            self.codetype = None
        else:
            self.codetype = args.codetype.upper()

        self.bapo = args.bapo
        self.xor = args.xor
        self.chksum = args.samples

        if args.job == "a":
            if args.dest:
                self.assemble(args.source, args.dest, filetype=filetype)
            else:
                print("\n-----------------\n" + self.assemble(args.source,
                                                              None, filetype=filetype).strip() + "\n-----------------\n")
        elif args.job == "d":
            if args.dest:
                self.disassemble(args.source, args.dest, filetype=filetype,
                                 cFooter=args.rmfooterasm, formalNames=args.formalnames)
            else:
                print("\n-----------------\n" + self.disassemble(args.source, None, filetype=filetype,
                                                                 cFooter=args.rmfooterasm, formalNames=args.formalnames)[0].strip() + "\n-----------------\n")
        else:
            parser.print_help()


def _ppc_exec(_args: List[str] = sys.argv):
    parser = ArgumentParser(prog="PyiiASMH 3",
                            description="Gecko code compiler for PPC assembly",
                            allow_abbrev=False)

    parser.add_argument("source", help="Source file")
    parser.add_argument("job",
                        help="Job to execute. Valid jobs are `a' and `d'",
                        choices=["a", "d"])
    parser.add_argument("--dest",
                        help="Destination file",
                        metavar="FILE")
    parser.add_argument("--bapo",
                        help="Address for the codehook",
                        default="80000000",
                        metavar="ADDR")
    parser.add_argument("--codetype",
                        help="The codetype being assembled",
                        choices=["0414", "0616", "C0", "C2D2", "F2F4", "RAW"],
                        default="RAW",
                        metavar="TYPE")
    parser.add_argument("--xor",
                        help="The XOR checksum in hex used for the F2/F4 codetype",
                        default="0000",
                        metavar="HEXVALUE")
    parser.add_argument("--samples",
                        help="""The number of samples in hex to be XORed for the F2/F4 codetype.
                        If negative, it searches backwards, else forwards""",
                        default="00",
                        metavar="HEXCOUNT")
    parser.add_argument("--formalnames",
                        help="Names r1 and r2 to be sp and rtoc respectively",
                        action="store_true")
    parser.add_argument("--rmfooterasm",
                        help="Removes the footer from a C0 block if possible, only useful if your code already contains an exit point",
                        action="store_true")

    args = parser.parse_args(_args)
    args.source = Path(args.source).resolve()

    if args.dest:
        args.dest = Path(args.dest).resolve()

    dumptype = "text"

    if args.bapo:
        try:
            addr = int(args.bapo, 16)
            if (addr < 0x80000000 or addr >= 0x81800000) and (addr < 0 or addr >= 0x01800000):
                parser.error(
                    f"The given ba/po address value {args.bapo} is invalid")
        except ValueError:
            parser.error(
                f"The given ba/po address value {args.bapo} is not a hex value")

    if args.xor:
        try:
            int(args.xor, 16)
        except ValueError:
            parser.error(f"The given XOR value {args.xor} is not a hex value")
        if len(args.xor) > 4:
            parser.error(f"The given XOR value {args.xor} is too large")

    if args.samples:
        try:
            int(args.samples, 16)
        except ValueError:
            parser.error(
                f"The given samples value {args.samples} is not a hex value")
        if len(args.samples) > 2:
            parser.error(
                f"The given samples value {args.samples} is too large")

    if args.dest and args.job == "a":
        if args.dest.suffix.lower() == ".txt":
            dumptype = "text"
        elif args.dest.suffix.lower() in (".bin", ".gct"):
            dumptype = "bin"
        else:
            parser.error(f"Destination file {args.dest} is invalid type")
    elif args.dest and args.job == "d":
        if args.dest.suffix.lower() != ".txt":
            parser.error(f"Destination file {args.dest} is invalid type")

    app = PyiiAsmhApp()
    app.run(parser, args, dumptype)


if __name__ == "__main__":
    _ppc_exec()
