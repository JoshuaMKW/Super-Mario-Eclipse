#!/usr/bin/env python3

#  PyiiASMH 3 (pyiiasmh.py)
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

import pickle as cPickle
import re
import signal
import sys
from pathlib import Path
from typing import Optional

from PyQt5 import QtCore, QtGui, QtWidgets

from pyiiasmh import children_ui
from pyiiasmh import mainwindow_ui
from pyiiasmh.ppctools import get_program_folder
from pyiiasmh.pyiiasmh_cli import PyiiAsmhApp, _ppc_exec


class PyiiAsmhGui(PyiiAsmhApp):

    class Actions:
        ASSEMBLE = "asm"
        DISASSEMBLE = "dsm"

    def __init__(self):
        super().__init__()

        self.app: QtWidgets.QApplication = None
        self.ui: mainwindow_ui.MainWindowUi = None
        self.uiprefs: children_ui.PrefsUi = None
        self.uibuiltins: children_ui.BuiltinsDocUI = None
        self.filepath: Path = None
        self.prefs = {"confirm": True, "loadlast": False,
                      "autodecorate": True, "formalnaming": False,
                      "codetype": "RAW", "qtstyle": "Default",
                      "darktheme": False}
        
        self.default_qtstyle: str = None
        self.style_log = []

    def convert(self, action: Actions):
        self.get_uivars(action)
        _stbar = self.ui.statusBar()

        if action == PyiiAsmhGui.Actions.ASSEMBLE:
            self.geckocodes = self.assemble(self.opcodes, None, None)
            if self.uiprefs.autodecorate.isChecked() == False:
                if self.ui.codetypeSelect.currentText() == "C0":
                    if self.geckocodes.endswith("\n4E800020 00000000"):
                        self.geckocodes = self.geckocodes[:-18]
                        self.geckocodes = re.sub(r"(?<=C0000000 )[0-9a-fA-F]{8}", "{:08X}".format(len(self.geckocodes.split("\n")) - 1), self.geckocodes)
                    elif self.geckocodes.endswith("\n4E800020 4E800020"):
                        self.geckocodes = self.geckocodes[:-8] + "00000000"
            self.ui.geckocodesPTextEdit.setPlainText(self.geckocodes)
            try:
                int(self.geckocodes.replace("\n", "").replace(" ", ""), 16)
                _stbar.showMessage("Assembled opcodes into gecko codes.", 3000)
            except (ValueError, AttributeError, TypeError):
                _stbar.showMessage("Failed to assemble opcodes into gecko codes.", 3000)
        elif action == PyiiAsmhGui.Actions.DISASSEMBLE:
            dsm_out = self.disassemble(self.geckocodes, None, None, self.uiprefs.autodecorate.isChecked(), self.uiprefs.formalnaming.isChecked())
            self.opcodes = dsm_out[0]
            self.bapo = dsm_out[1][0]
            self.xor = dsm_out[1][1]
            self.chksum = dsm_out[1][2]
            self.codetype = dsm_out[1][3]

            self.ui.opcodesPTextEdit.setPlainText(self.opcodes)
            if self.bapo is not None:
                self.ui.bapoLineEdit.setText(self.bapo)
            if self.xor is not None:
                self.ui.xorLineEdit.setText(self.xor)
            if self.chksum is not None:
                self.ui.checksumLineEdit.setText(self.chksum)
            if self.codetype == "0414":
                self.ui.codetypeSelect.setCurrentIndex(1)
            elif self.codetype == "0616":
                self.ui.codetypeSelect.setCurrentIndex(2)
            elif self.codetype == "C0":
                self.ui.codetypeSelect.setCurrentIndex(0)
            elif self.codetype == "C2D2":
                self.ui.codetypeSelect.setCurrentIndex(3)
            elif self.codetype == "F2F4":
                self.ui.codetypeSelect.setCurrentIndex(4)
            else:
                self.ui.codetypeSelect.setCurrentIndex(5)
            _stbar.showMessage("Disassembled gecko codes into opcodes.", 3000)
        else:
            raise NotImplementedError(f"Action \"{action}\" is unsupported")

    def get_uivars(self, action: Actions):
        if action == PyiiAsmhGui.Actions.DISASSEMBLE:
            self.geckocodes = str(self.ui.geckocodesPTextEdit.toPlainText())
        elif action == PyiiAsmhGui.Actions.ASSEMBLE:
            self.bapo = str(self.ui.bapoLineEdit.text())
            self.xor = str(self.ui.xorLineEdit.text())
            self.chksum = str(self.ui.checksumLineEdit.text())
            self.opcodes = str(self.ui.opcodesPTextEdit.toPlainText())+"\n"

            if self.ui.codetypeSelect.currentText() == "04/14":
                self.codetype = "0414"
            elif self.ui.codetypeSelect.currentText() == "06/16":
                self.codetype = "0616"
            elif self.ui.codetypeSelect.currentText() == "C0":
                self.codetype = "C0"
            elif self.ui.codetypeSelect.currentText() == "C2/D2":
                self.codetype = "C2D2"
            elif self.ui.codetypeSelect.currentText() == "F2/F4":
                self.codetype = "F2F4"
            else:
                self.codetype = None

            if self.bapo == "":
                if self.codetype not in ("C0", None):
                    self.bapo = "80000000"
                else:
                    self.bapo = None
            if self.xor == "":
                if self.codetype == "F2F4":
                    self.xor = "0000"
                else:
                    self.xor = None
            if self.chksum == "":
                if self.codetype == "F2F4":
                    self.chksum = "00"
                else:
                    self.chksum = None
        else:
            raise NotImplementedError(f"Action \"{action}\" is unsupported")

    def confirm_prompt(self, title: str, text: str, inform_text: str, detailed_text: Optional[str] = None) -> bool:
        cp = QtWidgets.QMessageBox(self.app.activeWindow())
        cp.setWindowTitle(title)
        cp.setText(text)
        cp.setInformativeText(inform_text)

        if detailed_text is not None:
            cp.setDetailedText(detailed_text)

        cp.setIcon(QtWidgets.QMessageBox.Warning)
        cp.setStandardButtons(QtWidgets.QMessageBox.Yes | QtWidgets.QMessageBox.No)
        cp.setDefaultButton(QtWidgets.QMessageBox.No)

        return cp.exec_() == QtWidgets.QMessageBox.No

    def confirm_helper(self, action, reload_file: bool = False):
        if self.prefs.get("confirm"):
            inform_text = "Unsaved data will be lost."

            if action.__name__ == "open_session":
                if reload_file:
                    title = "Reload"
                    text = "reload this session?"
                else:
                    title = "Open"
                    text = "open an existing session?"
                title += " Session"
                text = "Are you sure you want to " + text
            elif action.__name__ == "new_session":
                title = "New Session"
                text = "Are you sure you want to start a new session?"
            else:
                title = "Quit PyiiASMH 3"
                text = "Are you sure you want to quit?"

            if self.confirm_prompt(title, text, inform_text):
                return

        if action.__name__ == "open_session":
            action(reload_file)
        else:
            action()

    def show_dialog(self, dialog_type: str = None):
        if dialog_type == "aboutqt":
            QtWidgets.QMessageBox.aboutQt(self.app.activeWindow())
        elif dialog_type == "aboutpyiiasmh":
            ext = ""
            if sys.platform == "win32":
                ext += ".txt"

            desc = "".join([ "PyiiASMH 3 is a cross-platform WiiRd helper tool that ",
                             "is designed to help users with making WiiRd ready ASM ",
                             "codes. This application supports assembling powerpc ",
                             "opcodes into WiiRd codes using any of the WiiRd ASM ",
                             "codetypes (04/14, 06/16, C0, C2/D2, and F2/F4), or you can ",
                             "assemble them into raw hex. Disassembling of WiiRd codes ",
                             "or raw hex into PPC assembly opcodes is also supported. ",
                             "\n\n",
                             "Please see the readme (README"+ext+") for more details.",
                             "\n\n",
                             "Copyright (c) 2009, 2010, 2011, 2012, 2020\n\n",
                             "Sean Power <hawkeye2777[at]gmail[dot]com> \n\n",
                             "All rights reserved." ])

            QtWidgets.QMessageBox.about(self.app.activeWindow(), "About PyiiASMH 3", desc)
        elif dialog_type == "BuiltinsHelp":
            self.uibuiltins.init_docs()
            self.uibuiltins.funcSelect.clear()
            for item in self.uibuiltins.docs:
                self.uibuiltins.funcSelect.addItem(item[0])
            self.uibuiltins.show()
        else:  # dialog_type == "preferences":
            self.uiprefs.show()

    def new_session(self):
        self.ui.bapoLineEdit.setText("")
        self.ui.xorLineEdit.setText("")
        self.ui.checksumLineEdit.setText("")
        self.ui.opcodesPTextEdit.setPlainText("")
        self.ui.geckocodesPTextEdit.setPlainText("")
        self.ui.actionSave.setEnabled(False)
        self.ui.actionReload.setEnabled(False)
        self.filepath = None
        self.save_prefs()

        if self.prefs.get("codetype") == "04/14":
            self.ui.codetypeSelect.setCurrentIndex(1)
        elif self.prefs.get("codetype") == "06/16":
            self.ui.codetypeSelect.setCurrentIndex(2)
        elif self.prefs.get("codetype") == "C0":
            self.ui.codetypeSelect.setCurrentIndex(0)
        elif self.prefs.get("codetype") == "C2/D2":
            self.ui.codetypeSelect.setCurrentIndex(3)
        elif self.prefs.get("codetype") == "F2/F4":
            self.ui.codetypeSelect.setCurrentIndex(4)
        else:
            self.ui.codetypeSelect.setCurrentIndex(5)

        self.ui.setWindowTitle("PyiiASMH 3 - untitled")
        self.ui.statusBar().showMessage("New session started.", 3000)

    def open_session(self, reload_file: bool = False):
        if not reload_file:
            if self.filepath is None:  # Just start in the home directory
                fname = str(QtWidgets.QFileDialog.getOpenFileName(self.ui, "Open Session", str(Path.home()),
                                                                  "PyiiASMH 3 session files (*.psav);;All files (*)")[0])
            else:  # Start in the last directory used by the user
                fname = str(QtWidgets.QFileDialog.getOpenFileName(self.ui, "Open Session", str(self.filepath.parent),
                                                                  "PyiiASMH 3 session files (*.psav);;All files (*)")[0])

            if fname == "":  # Make sure we have something to open
                return
            else:
                self.filepath = Path(fname)

        try:
            with self.filepath.open("rb") as f:
                try:
                    data = cPickle.load(f)
                except cPickle.UnpicklingError as e:
                    self.log.exception(e)
                else:
                    self.ui.bapoLineEdit.setText(data.get("bapo"))
                    self.ui.xorLineEdit.setText(data.get("xor"))
                    self.ui.checksumLineEdit.setText(data.get("chksum"))
                    self.ui.opcodesPTextEdit.setPlainText(data.get("opcodes"))
                    self.ui.geckocodesPTextEdit.setPlainText(
                        data.get("geckocodes"))
                    self.ui.actionSave.setEnabled(True)
                    self.ui.actionReload.setEnabled(True)

                    if data.get("codetype") == "04/14":
                        self.ui.codetypeSelect.setCurrentIndex(1)
                    elif data.get("codetype") == "06/16":
                        self.ui.codetypeSelect.setCurrentIndex(2)
                    elif data.get("codetype") == "C0":
                        self.ui.codetypeSelect.setCurrentIndex(0)
                    elif data.get("codetype") == "C2/D2":
                        self.ui.codetypeSelect.setCurrentIndex(3)
                    elif data.get("codetype") == "F2/F4":
                        self.ui.codetypeSelect.setCurrentIndex(4)
                    else:
                        self.ui.codetypeSelect.setCurrentIndex(5)

                    self.save_prefs()
                    self.ui.setWindowTitle(f"PyiiASMH 3 - {self.filepath.name}")
                    self.ui.statusBar().showMessage(f"Loaded session '{self.filepath.name}'.", 3000)
        except IOError as e:
            self.log.exception(e)
            

    def save_session(self, save_as: bool = True):
        if save_as:
            if self.filepath is None:  # Just start in the home directory
                fname = str(QtWidgets.QFileDialog.getSaveFileName(self.ui, "Save Session", str(Path.home()),
                                                                  "PyiiASMH 3 session files (*.psav);;All files (*)")[0])
            else:  # Start in the last directory used by the user
                fname = str(QtWidgets.QFileDialog.getSaveFileName(self.ui, "Save Session", str(self.filepath.parent),
                                                                  "PyiiASMH 3 session files (*.psav);;All files (*)")[0])

            if fname == "" or fname is None:  # Make sure we have something to open
                return
            else:
                self.filepath = Path(fname)

        try:
            with self.filepath.open("wb") as f:
                data = {}
                data["bapo"] = str(self.ui.bapoLineEdit.text())
                data["xor"] = str(self.ui.xorLineEdit.text())
                data["chksum"] = str(self.ui.checksumLineEdit.text())
                data["opcodes"] = str(self.ui.opcodesPTextEdit.toPlainText())+"\n"
                data["geckocodes"] = str(self.ui.geckocodesPTextEdit.toPlainText())

                if self.uiprefs.codetypeSelect.currentText() == "04/14":
                    data["codetype"] = "04/14"
                elif self.uiprefs.codetypeSelect.currentText() == "06/16":
                    data["codetype"] = "06/16"
                elif self.uiprefs.codetypeSelect.currentText() == "C0":
                    data["codetype"] = "C0"
                elif self.uiprefs.codetypeSelect.currentText() == "C2/D2":
                    data["codetype"] = "C2/D2"
                elif self.uiprefs.codetypeSelect.currentText() == "F2/F4":
                    data["codetype"] = "F2/F4"
                else:
                    data["codetype"] = "RAW"

                cPickle.dump(data, f)
                self.ui.actionSave.setEnabled(True)
                self.ui.actionReload.setEnabled(True)

                self.save_prefs()
                self.ui.setWindowTitle(f"PyiiASMH 3 - {self.filepath.name}")
                self.ui.statusBar().showMessage(f"Saved session '{self.filepath.name}'.", 3000)
        except IOError as e:
            self.log.exception(e)

    def load_prefs(self):
        datapath = get_program_folder("PyiiASMH-3")

        try:
            with Path(datapath, ".last.psav").open("rb") as f:
                try:
                    filename = cPickle.load(f)
                except cPickle.UnpicklingError as e:
                    self.log.exception(e)
                else:
                    if filename is not None:
                        self.filepath = Path(filename)
        except IOError:
            self.log.warning("No last session found.")

        try:
            with Path(datapath, ".PyiiASMH.conf").open("rb") as f:
                try:
                    p = cPickle.load(f)
                except cPickle.UnpicklingError as e:
                    self.log.exception(e)  # Use defaults for prefs
                else:
                    # Input validation
                    if p.get("confirm") in (True, False):
                        self.prefs["confirm"] = p.get("confirm")

                    if p.get("loadlast") in (True, False):
                        self.prefs["loadlast"] = p.get("loadlast")

                    if p.get("codetype") in ("04/14", "06/16", "C0", "C2/D2", "F2/F4", "RAW"):
                        self.prefs["codetype"] = p.get("codetype")

                        if p.get("codetype") == "04/14":
                            self.uiprefs.codetypeSelect.setCurrentIndex(1)
                        elif p.get("codetype") == "06/16":
                            self.uiprefs.codetypeSelect.setCurrentIndex(2)
                        elif p.get("codetype") == "C0":
                            self.uiprefs.codetypeSelect.setCurrentIndex(0)
                        elif p.get("codetype") == "C2/D2":
                            self.uiprefs.codetypeSelect.setCurrentIndex(3)
                        elif p.get("codetype") == "F2/F4":
                            self.uiprefs.codetypeSelect.setCurrentIndex(4)

                    if p.get("formalnaming") in (True, False):
                        self.prefs["formalnaming"] = p.get("formalnaming")
                    
                    if p.get("autodecorate") in (True, False):
                        self.prefs["autodecorate"] = p.get("autodecorate")

                    if p.get("darktheme") in (True, False):
                        self.prefs["darktheme"] = p.get("darktheme")

                    if (p.get("qtstyle") in list(QtWidgets.QStyleFactory.keys()) or
                            p.get("qtstyle") == "Default"):
                        self.prefs["qtstyle"] = p.get("qtstyle")

                    setCIndex = self.uiprefs.qtstyleSelect.setCurrentIndex

                    if self.prefs.get("qtstyle") in (None, "Default"):
                        setCIndex(0)
                    else:
                        setCIndex(self.uiprefs.qtstyleSelect.findText(
                            self.prefs.get("qtstyle"),
                            flags=QtCore.Qt.MatchFixedString))

                    setCIndex = self.uiprefs.codetypeSelect.setCurrentIndex
                    setCIndex(self.uiprefs.codetypeSelect.findText(
                        self.prefs.get("codetype"),
                        flags=QtCore.Qt.MatchFixedString))

                    self.ui.set_close_event(self.prefs.get("confirm"))
                    self.uiprefs.confirmation.setChecked(self.prefs.get("confirm"))
                    self.uiprefs.loadLast.setChecked(self.prefs.get("loadlast"))
                    self.uiprefs.formalnaming.setChecked(self.prefs.get("formalnaming"))
                    self.uiprefs.autodecorate.setChecked(self.prefs.get("autodecorate"))
                    self.uiprefs.qtdarkButton.setChecked(self.prefs.get("darktheme"))
                    self.update_theme()
        except FileNotFoundError:
            self.log.warning("No preferences file found; using defaults.")

    def save_prefs(self):
        datapath = get_program_folder("PyiiASMH-3")

        self.prefs["confirm"] = self.uiprefs.confirmation.isChecked()
        self.prefs["loadlast"] = self.uiprefs.loadLast.isChecked()
        self.prefs["codetype"] = str(self.uiprefs.codetypeSelect.currentText())
        self.prefs["formalnaming"] = self.uiprefs.formalnaming.isChecked()
        self.prefs["autodecorate"] = self.uiprefs.autodecorate.isChecked()
        self.prefs["qtstyle"] = str(self.uiprefs.qtstyleSelect.currentText())
        self.prefs["darktheme"] = self.uiprefs.qtdarkButton.isChecked()
        self.ui.set_close_event(self.prefs.get("confirm"))

        try:
            with Path(datapath, ".PyiiASMH.conf").open("wb") as f:
                cPickle.dump(self.prefs, f)
        except IOError as e:
            self.log.exception(e)
            
        try:
            with Path(datapath, ".last.psav").open("wb") as f:
                cPickle.dump(str(self.filepath), f)
        except IOError as e:
            self.log.exception(e)

    def load_qtstyle(self, style, first_style_load=False):
        self.style_log.append( [self.app.style, self.uiprefs.qtstyleSelect.currentText()] )

        if len(self.style_log) > 2:
            self.style_log.pop(0)

        if style != "Default":
            self.app.setStyle(style)
        else:
            self.app.setStyle(self.default_qtstyle)

        if first_style_load:
            setCIndex = self.uiprefs.qtstyleSelect.setCurrentIndex
            setCIndex(self.uiprefs.qtstyleSelect.findText(style,
                                                          flags=QtCore.Qt.MatchFixedString))

    def update_theme(self):
        if self.uiprefs.qtdarkButton.isChecked():
            self.app.setPalette(self.ui.DarkTheme)
            self.load_qtstyle("Fusion", True)
            self.uiprefs.qtstyleSelect.setDisabled(True)
        else:
            self.app.setPalette(self.ui.LightTheme)
            self.load_qtstyle(self.style_log[0][1], True)
            self.uiprefs.qtstyleSelect.setEnabled(True)

    def connect_signals(self):
        self.ui.asmButton.clicked.connect(lambda: self.convert(PyiiAsmhGui.Actions.ASSEMBLE))
        self.ui.dsmButton.clicked.connect(lambda: self.convert(PyiiAsmhGui.Actions.DISASSEMBLE))

        self.ui.actionQuit.triggered.connect(self.ui.close)
        self.ui.actionPreferences.triggered.connect(lambda: self.show_dialog("Preferences"))
        self.ui.actionAbout_Qt.triggered.connect(lambda: self.show_dialog("aboutqt"))
        self.ui.actionAbout_PyiiASMH.triggered.connect(lambda: self.show_dialog("aboutpyiiasmh"))
        self.ui.actionBuiltins_Help.triggered.connect(lambda: self.show_dialog("BuiltinsHelp"))

        self.ui.actionNew.triggered.connect(lambda: self.confirm_helper(self.new_session))
        self.ui.actionOpen.triggered.connect(lambda: self.confirm_helper(self.open_session))
        self.ui.actionSave_As.triggered.connect(lambda: self.save_session(True))
        self.ui.actionReload.triggered.connect(lambda: self.confirm_helper(self.open_session, True))
        self.ui.actionSave.triggered.connect(lambda: self.save_session(False))

        self.uiprefs.buttonBox.accepted.connect(self.save_prefs)
        self.uiprefs.qtstyleSelect.currentIndexChanged.connect(lambda: self.load_qtstyle(self.uiprefs.qtstyleSelect.currentText()))
        self.uiprefs.qtdarkButton.clicked.connect(lambda: self.update_theme())

        self.uibuiltins.funcSelect.currentRowChanged.connect(lambda: self.uibuiltins.update_info())

    def run(self):
        datapath = get_program_folder("PyiiASMH-3")

        if not datapath.is_dir():
            datapath.mkdir(parents=True)

        self.app = QtWidgets.QApplication(sys.argv)
        self.default_qtstyle = self.app.style().objectName()
        self.ui = mainwindow_ui.MainWindowUi()
        self.uiprefs = children_ui.PrefsUi()
        self.uibuiltins = children_ui.BuiltinsDocUI()

        self.uiprefs.qtstyleSelect.addItem("Default")

        for i in range(0, len(list(QtWidgets.QStyleFactory.keys()))):
            self.uiprefs.qtstyleSelect.addItem(list(QtWidgets.QStyleFactory.keys())[i])

        self.load_qtstyle(self.prefs.get("qtstyle"), True)
        self.load_prefs()
        self.ui.opcodesPTextEdit.setFocus()
        self.ui.codetypeSelect.setCurrentIndex(self.uiprefs.codetypeSelect.currentIndex())

        regex = QtCore.QRegExp("[0-9A-Fa-f]*")
        validator = QtGui.QRegExpValidator(regex)
        self.ui.bapoLineEdit.setValidator(validator)
        self.ui.xorLineEdit.setValidator(validator)
        self.ui.checksumLineEdit.setValidator(validator)

        if self.filepath and self.prefs.get("loadlast"):
            self.open_session(self.filepath)

        self.connect_signals()
        self.ui.show()
        sys.exit(self.app.exec_())


if __name__ == "__main__":
    if len(sys.argv) == 1:
        app = PyiiAsmhGui()
        signal.signal(signal.SIGINT, signal.SIG_DFL)
        app.run()
    else:
        _ppc_exec()
