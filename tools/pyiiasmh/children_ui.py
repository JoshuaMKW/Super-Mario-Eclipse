# -*- coding: utf-8 -*-
#
#  PyiiASMH 3 (prefs_ui.py)
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

import sys

from PyQt5 import QtCore, QtWidgets, QtGui

class PrefsUi(QtWidgets.QDialog):
    def __init__(self):
        super().__init__(None, QtCore.Qt.WindowSystemMenuHint | QtCore.Qt.WindowTitleHint | QtCore.Qt.WindowCloseButtonHint)

        self.setupUi()

    def setupUi(self):
        self.setObjectName("Dialog")
        self.resize(0, 0)
        self.setModal(True)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap("PyiiASMH.ico"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.setWindowIcon(icon)
        self.buttonBox = QtWidgets.QDialogButtonBox(self)
        self.buttonBox.setMinimumSize(QtCore.QSize(280, 40))
        self.buttonBox.setMaximumSize(QtCore.QSize(280, 40))
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")

        self.centerWidget = QtWidgets.QWidget(self)
        self.centerWidget.setObjectName("centralwidget")

        self.formLayoutWidget = QtWidgets.QGridLayout(self)
        self.formLayoutWidget.setObjectName("formLayoutWidget")

        self.codeSettingsLayout = QtWidgets.QGridLayout()
        self.codeSettingsLayout.setVerticalSpacing(8)
        self.codeSettingsLayout.setObjectName("codeSettingsLayout")

        #confirmation
        self.confirmation = QtWidgets.QCheckBox()
        self.confirmation.setChecked(True)
        self.confirmation.setObjectName("confirmation")
        self.codeSettingsLayout.addWidget(self.confirmation, 0, 0, 1, 1)

        #loadLast
        self.loadLast = QtWidgets.QCheckBox()
        self.loadLast.setObjectName("loadLast")
        self.codeSettingsLayout.addWidget(self.loadLast, 1, 0, 1, 1)

        #autodecorate
        self.autodecorate = QtWidgets.QCheckBox()
        self.autodecorate.setChecked(True)
        self.autodecorate.setObjectName("autodecorate")
        self.codeSettingsLayout.addWidget(self.autodecorate, 2, 0, 1, 1)

        #formalnaming
        self.formalnaming = QtWidgets.QCheckBox()
        self.formalnaming.setObjectName("formalnaming")
        self.codeSettingsLayout.addWidget(self.formalnaming, 3, 0, 1, 1)

        #codetype label
        self.codetypeLabel = QtWidgets.QLabel()
        self.codetypeLabel.setObjectName("codetypeLabel")
        self.codeSettingsLayout.addWidget(self.codetypeLabel, 4, 0, 1, 1)
        
        #codetype box
        self.codetypeSelect = QtWidgets.QComboBox()
        self.codetypeSelect.setMaximumWidth(110)
        self.codetypeSelect.setObjectName("codetypeSelect")
        self.codetypeSelect.addItems(["", "", "", "", "", ""])
        self.codeSettingsLayout.addWidget(self.codetypeSelect, 4, 1, 1, 1)

        self.comboBoxLayout = QtWidgets.QGridLayout()
        self.comboBoxLayout.setVerticalSpacing(10)
        self.comboBoxLayout.setObjectName("comboBoxLayout")

        #qtstyle box
        self.qtstyleSelect = QtWidgets.QComboBox()
        self.qtstyleSelect.setMaximumWidth(110)
        self.qtstyleSelect.setObjectName("qtstyleSelect")
        self.comboBoxLayout.addWidget(self.qtstyleSelect, 1, 1, 1, 1)

        #qtstyle label
        self.qtstyleLabel = QtWidgets.QLabel()
        self.qtstyleLabel.setObjectName("qtstyleLabel")
        self.comboBoxLayout.addWidget(self.qtstyleLabel, 1, 0, 1, 1)

        #qtdark theme
        self.qtdarkButton = QtWidgets.QCheckBox()
        self.qtdarkButton.setObjectName("formalnaming")
        self.qtdarkButton.setText("Dark Theme")
        self.comboBoxLayout.addWidget(self.qtdarkButton, 2, 0, 1, 1)

        #horizontal separater 1
        self.horiSepA = QtWidgets.QFrame(self.centerWidget)
        self.horiSepA.setMinimumSize(QtCore.QSize(280, 30))
        self.horiSepA.setFrameShape(QtWidgets.QFrame.HLine)
        self.horiSepA.setFrameShadow(QtWidgets.QFrame.Sunken)
        self.horiSepA.setObjectName("horiSepA")

        #horizontal separater 2
        self.horiSepB = QtWidgets.QFrame(self.centerWidget)
        self.horiSepB.setMinimumSize(QtCore.QSize(280, 30))
        self.horiSepB.setFrameShape(QtWidgets.QFrame.HLine)
        self.horiSepB.setFrameShadow(QtWidgets.QFrame.Sunken)
        self.horiSepB.setObjectName("horiSepB")

        self.formLayoutWidget.addLayout(self.codeSettingsLayout, 0, 0, 1, 1)
        self.formLayoutWidget.addWidget(self.horiSepA, 1, 0, 1, 1)
        self.formLayoutWidget.addLayout(self.comboBoxLayout, 2, 0, 1, 1)
        self.formLayoutWidget.addWidget(self.horiSepB, 3, 0, 1, 1)
        self.formLayoutWidget.addWidget(self.buttonBox, 4, 0, 1, 1)

        self.retranslateUi()
        self.codetypeSelect.setCurrentIndex(5)

        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
        QtCore.QMetaObject.connectSlotsByName(self)

    def retranslateUi(self):
        self.setWindowTitle(QtWidgets.QApplication.translate("Dialog", "Preferences", None))
        self.loadLast.setText(QtWidgets.QApplication.translate("Dialog", "Load last session on startup", None))
        self.confirmation.setToolTip(QtWidgets.QApplication.translate("Dialog", "Show confirmation dialogs for starting a new session, saving, reloading, or exiting the application.", None))
        self.confirmation.setText(QtWidgets.QApplication.translate("Dialog", "Confirmation Dialogs", None))
        self.autodecorate.setText(QtWidgets.QApplication.translate("Dialog", "Use C0 end block", None))
        self.formalnaming.setText(QtWidgets.QApplication.translate("Dialog", "Use \"sp\" and \"rtoc\"", None))
        self.codetypeSelect.setItemText(0, QtWidgets.QApplication.translate("Dialog", "C0", None))
        self.codetypeSelect.setItemText(1, QtWidgets.QApplication.translate("Dialog", "04/14", None))
        self.codetypeSelect.setItemText(2, QtWidgets.QApplication.translate("Dialog", "06/16", None))
        self.codetypeSelect.setItemText(3, QtWidgets.QApplication.translate("Dialog", "C2/D2", None))
        self.codetypeSelect.setItemText(4, QtWidgets.QApplication.translate("Dialog", "F2/F4", None))
        self.codetypeSelect.setItemText(5, QtWidgets.QApplication.translate("Dialog", "RAW", None))
        self.codetypeLabel.setText(QtWidgets.QApplication.translate("Dialog", "Default Codetype:", None))
        self.qtstyleLabel.setText(QtWidgets.QApplication.translate("Dialog", "GUI Style:", None))

class BuiltinsDocUI(QtWidgets.QDialog):
    def __init__(self):
        super().__init__(None, QtCore.Qt.WindowSystemMenuHint | QtCore.Qt.WindowTitleHint | QtCore.Qt.WindowCloseButtonHint)
        self.docs = []
        
        self.setupUi()

    def setupUi(self):
        self.setObjectName("Dialog")
        self.resize(700, 400)
        self.setMinimumSize(QtCore.QSize(700, 400))
        self.setBaseSize(QtCore.QSize(700, 400))
        self.setMaximumSize(QtCore.QSize(700, 400))
        self.setModal(True)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap("PyiiASMH.ico"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.setWindowIcon(icon)

        self.objtypebar = QtWidgets.QPlainTextEdit(self)
        font = QtGui.QFont()
        font.setFamily("Consolas")
        font.setPointSize(13)
        font.setWeight(34)
        fontMetrics = QtGui.QFontMetricsF(font)
        spaceWidth = fontMetrics.width(' ')
        self.objtypebar.setFont(font)
        self.objtypebar.setMinimumSize(QtCore.QSize(400, 32))
        self.objtypebar.setMaximumSize(QtCore.QSize(16777215, 32))
        self.objtypebar.setTabStopDistance(spaceWidth * 4)
        self.objtypebar.setReadOnly(True)
        self.objtypebar.setLineWrapMode(QtWidgets.QPlainTextEdit.NoWrap)
        self.objtypebar.setBackgroundVisible(False)
        self.objtypebar.setObjectName("docuTextBox")

        self.objtypebar.setPlainText("")

        self.gridLayoutWidget = QtWidgets.QGridLayout(self)
        self.gridLayoutWidget.setGeometry(QtCore.QRect(10, 60, 400, 70))
        self.gridLayoutWidget.setObjectName("gridLayoutWidget")

        self.funcSelect = QtWidgets.QListWidget(self)
        self.funcSelect.setMinimumSize(QtCore.QSize(200, 200))
        self.funcSelect.setMaximumSize(QtCore.QSize(200, 16777215))
        self.funcSelect.setObjectName("funcSelect")

        for item in self.docs:
            self.funcSelect.addItem(item[0])

        self.docuTextBox = QtWidgets.QPlainTextEdit(self)
        self.docuTextBox.setMinimumSize(QtCore.QSize(400, 300))
        self.docuTextBox.setMaximumSize(QtCore.QSize(16777215, 16777215))
        font = QtGui.QFont()
        font.setFamily("Consolas")
        font.setPointSize(10)
        font.setWeight(34)
        fontMetrics = QtGui.QFontMetricsF(font)
        spaceWidth = fontMetrics.width(' ')
        self.docuTextBox.setFont(font)
        self.docuTextBox.setTabStopDistance(spaceWidth * 4)
        self.docuTextBox.setReadOnly(True)
        self.docuTextBox.setBackgroundVisible(False)
        self.docuTextBox.setObjectName("docuTextBox")

        self.docuTextBox.setPlainText("")

        self.gridLayoutWidget.addWidget(self.funcSelect, 0, 0, 2, 1)
        self.gridLayoutWidget.addWidget(self.objtypebar, 0, 1, 1, 1)
        self.gridLayoutWidget.addWidget(self.docuTextBox, 1, 1, 1, 1)

        self.setWindowTitle(QtWidgets.QApplication.translate("Dialog", "Builtins Documentation", None))

        QtCore.QMetaObject.connectSlotsByName(self)

    def init_docs(self):
        self.docs = [("#inject", "Syntax: #inject(addr)\n\n"
                                 "pragma like shebang to be used at the very beginning of a code to set the injection address\n\n"
                                 "Example:\n"
                                 "  #inject(0x80241898)\n\n"
                                 "  __set r3, INJECTADDR\n\n"
                                 "Result (06 type):\n"
                                 "  06241898 00000008\n"
                                 "  3C608024 60631898", "shebang"),
                     ("INJECTADDR", "Dynamic value representing the injecton address of the compiled code", "const")]
        _names = []
        _docs = []
        _types = []

        try:
            with open("__includes.s", "r") as builtins:
                commentready = False
                _documentation = ""
                for line in builtins.readlines():
                    line.strip()
                    if line == "":
                        continue
                    elif line.startswith("#") and commentready:
                        _documentation += line[1:].strip() + "\n"
                        continue
                    elif commentready:
                        _docs.append(_documentation)
                        commentready = False
                        _documentation = ""
                        continue

                    segments = line.split(" ")
                    if segments[0] == ".set":
                        _types.append("const")
                    elif segments[0] == ".macro":
                        _types.append("macro")
                    else:
                        commentready = False
                        continue
                    
                    commentready = True
                    _names.append(segments[1].strip().rstrip(","))

            for i in range(len(_names)):
                self.docs.append((_names[i], _docs[i], _types[i]))
        except FileNotFoundError:
            pass

    def update_info(self):
        if "linux" in sys.platform:
            self.objtypebar.setPlainText(self.docs[self.funcSelect.currentRow()][2].center(45, " ").upper())
        else:
            self.objtypebar.setPlainText(self.docs[self.funcSelect.currentRow()][2].center(51, " ").upper())
            
        self.docuTextBox.setPlainText(self.docs[self.funcSelect.currentRow()][1])


        