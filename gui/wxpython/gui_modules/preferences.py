"""
@package preferences

@brief User preferences dialog

Sets default display font, etc.

Classes:
 * PreferencesDialog
 * SetDefaultFont

(C) 2007-2008 by the GRASS Development Team
This program is free software under the GNU General Public
License (>=v2). Read the file COPYING that comes with GRASS
for details.

@author Michael Barton (Arizona State University)
Martin Landa <landa.martin gmail.com>
"""

import os
import sys
import copy

import wx
import wx.lib.filebrowsebutton as filebrowse
from wx.lib.wordwrap import wordwrap

gmpath = os.path.join( os.getenv("GISBASE"), "etc", "wx", "gui_modules")
sys.path.append(gmpath)

import gcmd

class Settings:
    """Generic class where to store settings"""
    def __init__(self):
        self.defaultSettings = {
            # general
            'displayFont' : '',
            # advanced
            'settingsFile' : 'home', # home, location, mapset
            'digitInterface' : 'vedit', # vedit, vdigit
            }
        
        self.userSettings = copy.deepcopy(self.defaultSettings)

    def Get(self, key):
        """Get value by key

        @return value
        @return None if key not found
        """
        if self.userSettings.has_key(key):
            return self.userSettings[key]
        else:
            None
    
    def Set(self, key, value):
        """Set value by key

        Raise KeyError if key is not found
        """
        if self.userSettings.has_key(key):
            self.userSettings[key] = value
        else:
            raise KeyError

globalSettings = Settings()

class PreferencesDialog(wx.Dialog):
    """User preferences dialog"""
    def __init__(self, parent, title,
                 settings=globalSettings,
                 style=wx.DEFAULT_DIALOG_STYLE):
        self.parent = parent # GMFrame
        self.title = title
        wx.Dialog.__init__(self, parent=parent, id=wx.ID_ANY, title=title,
                           style=style)

        self.settings = settings
        # notebook
        notebook = wx.Notebook(parent=self, id=wx.ID_ANY, style=wx.BK_DEFAULT)

        # create notebook pages
        self.__CreateGeneralPage(notebook)
        self.__CreateAdvancedPage(notebook)

        # buttons
        # btnSave = wx.Button(self, wx.ID_SAVE)
        btnApply = wx.Button(self, wx.ID_APPLY)
        btnCancel = wx.Button(self, wx.ID_CANCEL)
        btnOk = wx.Button(self, wx.ID_OK)
        btnOk.SetDefault()

        # bindigs
        btnApply.Bind(wx.EVT_BUTTON, self.OnApply)
        btnOk.Bind(wx.EVT_BUTTON, self.OnOK)
        btnCancel.Bind(wx.EVT_BUTTON, self.OnCancel)

        # sizers
        btnSizer = wx.StdDialogButtonSizer()
        btnSizer.AddButton(btnCancel)
        # btnSizer.AddButton(btnSave)
        btnSizer.AddButton(btnApply)
        btnSizer.AddButton(btnOk)
        btnSizer.Realize()
        
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        mainSizer.Add(item=notebook, proportion=1, flag=wx.EXPAND | wx.ALL, border=5)
        mainSizer.Add(item=btnSizer, proportion=0,
                      flag=wx.EXPAND | wx.ALL | wx.ALIGN_CENTER, border=5)

        self.SetSizer(mainSizer)
        mainSizer.Fit(self)

    def __CreateGeneralPage(self, notebook):
        """Create notebook page concerning with symbology settings"""
        panel = wx.Panel(parent=notebook, id=wx.ID_ANY)
        notebook.AddPage(page=panel, text=_("General"))

        border = wx.BoxSizer(wx.VERTICAL)
        box   = wx.StaticBox (parent=panel, id=wx.ID_ANY, label=" %s " % _("General settings"))
        sizer = wx.StaticBoxSizer(box, wx.VERTICAL)

        gridSizer = wx.GridBagSizer (hgap=3, vgap=3)
        gridSizer.AddGrowableCol(0)

        #
        # display font
        #
        gridSizer.Add(item=wx.StaticText(parent=panel, id=wx.ID_ANY,
                                         label=_("Default font for GRASS displays:")),
                       flag=wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL,
                       pos=(0, 0))
        fontButton = wx.Button(parent=panel, id=wx.ID_ANY,
                               label=_("Set font"), size=(100, -1))
        gridSizer.Add(item=fontButton,
                      flag=wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL,
                       pos=(0, 1))
        gridSizer.Add(item=wx.StaticText(parent=panel, id=wx.ID_ANY,
                                         label=_("Currently selected font:")),
                       flag=wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL,
                       pos=(1, 0), span=(1, 2))

        sizer.Add(item=gridSizer, proportion=1, flag=wx.ALL | wx.EXPAND, border=5)
        border.Add(item=sizer, proportion=1, flag=wx.ALL | wx.EXPAND, border=3)

        panel.SetSizer(border)
        
        # bindings
        fontButton.Bind(wx.EVT_BUTTON, self.OnSetFont)

        return panel

    def __CreateAdvancedPage(self, notebook):
        """Create notebook page concerning with symbology settings"""
        panel = wx.Panel(parent=notebook, id=wx.ID_ANY)
        notebook.AddPage(page=panel, text=_("Advanced"))

        border = wx.BoxSizer(wx.VERTICAL)
        box   = wx.StaticBox (parent=panel, id=wx.ID_ANY, label=" %s " % _("Advanced settings"))
        sizer = wx.StaticBoxSizer(box, wx.VERTICAL)

        gridSizer = wx.GridBagSizer (hgap=3, vgap=3)
        gridSizer.AddGrowableCol(0)

        #
        # place where to store settings
        #
        gridSizer.Add(item=wx.StaticText(parent=panel, id=wx.ID_ANY,
                                         label=_("Place where to store settings:")),
                       flag=wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL,
                       pos=(0, 0))
        self.settingsFile = wx.Choice(parent=panel, id=wx.ID_ANY, size=(125, -1),
                                      choices=['home', 'location', 'mapset'])
        self.settingsFile.SetStringSelection(self.settings.Get('settingsFile'))
        gridSizer.Add(item=self.settingsFile,
                      flag=wx.ALIGN_RIGHT |
                      wx.ALIGN_CENTER_VERTICAL,
                      pos=(0, 1))

        #
        # digitization interface
        #
        gridSizer.Add(item=wx.StaticText(parent=panel, id=wx.ID_ANY,
                                         label=_("Digitization interface:")),
                       flag=wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL,
                       pos=(1, 0))
        self.digitInterface = wx.Choice(parent=panel, id=wx.ID_ANY, size=(125, -1),
                                        choices=['vdigit', 'vedit'])
        self.digitInterface.SetStringSelection(self.settings.Get('digitInterface'))
        gridSizer.Add(item=self.digitInterface,
                      flag=wx.ALIGN_RIGHT |
                      wx.ALIGN_CENTER_VERTICAL,
                      pos=(1, 1))

        digitNote = wordwrap(_("Note: User can choose from two interfaces for digitization. "
                               "The simple one uses v.edit command on the background. "
                               "Map topology is rebuild on each operation which can "
                               "significantly slow-down response. The vdigit is a native "
                               "interface which uses v.edit functionality, but doesn't "
                               "call the command itself."),
                             self.GetSize()[0]-50, wx.ClientDC(self))

        gridSizer.Add(item=wx.StaticText(parent=panel, id=wx.ID_ANY,
                                         label=digitNote),
                      flag=wx.ALIGN_LEFT |
                      wx.ALIGN_CENTER_VERTICAL,
                      pos=(2, 0), span=(1, 2))

        sizer.Add(item=gridSizer, proportion=1, flag=wx.ALL | wx.EXPAND, border=5)
        border.Add(item=sizer, proportion=1, flag=wx.ALL | wx.EXPAND, border=3)

        panel.SetSizer(border)
        
        return panel

    def OnSetFont(self, event):
        """'Set font' button pressed"""
        dlg = SetDefaultFont(parent=self, id=wx.ID_ANY,
                             title=_('Select default display font'),
                             pos=wx.DefaultPosition, size=wx.DefaultSize,
                             style=wx.DEFAULT_DIALOG_STYLE,
                             encoding=self.parent.encoding)
        if dlg.ShowModal() == wx.ID_CANCEL:
            dlg.Destroy()
            return

        # set default font type, font, and encoding to whatever selected in dialog
        if dlg.font != None:
            self.font = dlg.font
        if dlg.encoding != None:
            self.encoding = dlg.encoding

        dlg.Destroy()

        # set default font and encoding environmental variables
        os.environ["GRASS_FONT"] = self.font
        if self.encoding != None and self.encoding != "ISO-8859-1":
            os.environ["GRASS_ENCODING"] = self.encoding

        event.Skip()

    def OnOK(self, event):
        """Button 'OK' clicked"""
        self.__UpdateSettings()
        self.Close()

    def OnApply(self, event):
        """Button 'Apply' clicked"""
        self.__UpdateSettings()
        
    def OnCancel(self, event):
        """Button 'Cancel' clicked"""
        self.Close()

    def __UpdateSettings(self):
        """Update user settings"""
        # font
        # TODO

        # location
        self.settings.Set('settingsFile', self.settingsFile.GetStringSelection())
        
        # digitization interface
        self.settings.Set('digitInterface', self.digitInterface.GetStringSelection())

class SetDefaultFont(wx.Dialog):
    """
    Opens a file selection dialog to select default font
    to use in all GRASS displays
    """
    def __init__(self, parent, id, title, pos=wx.DefaultPosition, size=wx.DefaultSize,
            style=wx.DEFAULT_DIALOG_STYLE, encoding='ISO-8859-1'):
        wx.Dialog.__init__(self, parent, id, title, pos, size, style)

        if "GRASS_FONT" in os.environ:
            self.font = os.environ["GRASS_FONT"]
        else:
            self.font = None

        self.fontlist = self.GetFonts()

        self.encoding = encoding

        sizer = wx.BoxSizer(wx.VERTICAL)

        box = wx.BoxSizer(wx.HORIZONTAL)
        label = wx.StaticText(self, -1, "Select Font:", (15, 50))
        box.Add(label, 0, wx.EXPAND|wx.GROW|wx.ALIGN_TOP|wx.RIGHT, 5)
        self.fontlb = wx.ListBox(self, wx.ID_ANY, pos=wx.DefaultPosition,
                                 size=(280,150), choices=self.fontlist,
                                 style=wx.LB_SINGLE|wx.LB_SORT)
        self.Bind(wx.EVT_LISTBOX, self.EvtListBox, self.fontlb)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.EvtListBoxDClick, self.fontlb)
        if self.font:
            self.fontlb.SetStringSelection(self.font, True)
        box.Add(self.fontlb, 0, wx.EXPAND|wx.GROW|wx.ALIGN_RIGHT)
        sizer.Add(box, 0, wx.EXPAND|wx.GROW|wx.ALIGN_RIGHT|wx.ALL, 8)

        box = wx.BoxSizer(wx.HORIZONTAL)
        label = wx.StaticText(self, -1, "Character encoding:")
        box.Add(label, 0, wx.ALIGN_RIGHT|wx.RIGHT, 5)
        self.textentry = wx.TextCtrl(self, -1, "", size=(200,-1))
        self.textentry.SetValue(self.encoding)
        box.Add(self.textentry, 0, wx.ALIGN_LEFT)
        self.textentry.Bind(wx.EVT_TEXT, self.OnEncoding)
        sizer.Add(box, 0, wx.GROW|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 8)

        line = wx.StaticLine(self, -1, size=(20,-1), style=wx.LI_HORIZONTAL)
        sizer.Add(line, 0, wx.GROW|wx.ALIGN_CENTER_VERTICAL|wx.ALL, 10)

        btnsizer = wx.StdDialogButtonSizer()

        btn = wx.Button(self, wx.ID_OK)
        btn.SetDefault()
        btnsizer.AddButton(btn)

        btn = wx.Button(self, wx.ID_CANCEL)
        btnsizer.AddButton(btn)
        btnsizer.Realize()

        sizer.Add(btnsizer, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALL, 5)

        self.SetSizer(sizer)
        sizer.Fit(self)

    def EvtRadioBox(self, event):
        if event.GetInt() == 0:
            self.fonttype = 'grassfont'
        elif event.GetInt() == 1:
            self.fonttype = 'truetype'

        self.fontlist = self.GetFonts(self.fonttype)
        self.fontlb.SetItems(self.fontlist)

    def OnEncoding(self, event):
        self.encoding = event.GetString()

    def EvtListBox(self, event):
        self.font = event.GetString()
        event.Skip()

    def EvtListBoxDClick(self, event):
        self.font = event.GetString()
        event.Skip()

    def GetFonts(self):
        """
        parses fonts directory or fretypecap file to get a list of fonts for the listbox
        """
        fontlist = []

        cmd = ["d.font", "-l"]

        p = gcmd.Command(cmd, stderr=None)

        dfonts = p.ReadStdOutput()
        dfonts.sort(lambda x,y: cmp(x.lower(), y.lower()))
        for item in range(len(dfonts)):
           # ignore duplicate fonts and those starting with #
           if not dfonts[item].startswith('#') and \
                  dfonts[item] != dfonts[item-1]:
              fontlist.append(dfonts[item])

        return fontlist
