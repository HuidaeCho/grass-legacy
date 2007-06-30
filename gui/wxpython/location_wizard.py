import wx
import wx.wizard as wiz
import  wx.lib.rcsizer  as rcs
from wx.lib.combotreebox import ComboTreeBox

import os
import sys
import string
import re

import gui_modules
gmpath = gui_modules.__path__[0]
sys.path.append(gmpath)

import gui_modules.cmd as cmd

global coordsys
global georeffile
global datum
global transform
global projection
global north
global south
global east
global west
global resolution
global wizerror

coordsys = ''
georeffile = ''
datum = ''
transform = ''
projection = ''
north = ''
south = ''
east = ''
west = ''
resolution = ''

class TitledPage(wiz.WizardPageSimple):
    def __init__(self, parent, title):
        wiz.WizardPageSimple.__init__(self, parent)

        self.title = wx.StaticText(self,-1,title)
        self.title.SetFont(wx.Font(13, wx.SWISS, wx.NORMAL, wx.BOLD))

        self.sizer = rcs.RowColSizer()
        tmpsizer = wx.BoxSizer(wx.VERTICAL)

        tmpsizer.Add(self.title, 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        tmpsizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND|wx.ALL, 0)
        tmpsizer.Add(self.sizer, wx.EXPAND)

        self.SetSizer(tmpsizer)
        self.SetAutoLayout(True)

    def MakeLabel(self, text=""):
        try:
            if text[-1] != " ":
                text += " "
        except:
            pass
        return wx.StaticText(self, -1, text, style=wx.ALIGN_RIGHT)

    def MakeTextCtrl(self,text='', size=(100,-1)):
        return wx.TextCtrl(self,-1, text, size=size)

    def MakeButton(self,text, size=(75,25)):
        return wx.Button(self, -1, text,
                style=wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL,
                size=size)


class DatumPage(TitledPage):
    def __init__(self, wizard, parent):
        TitledPage.__init__(self, wizard, "Specify geodetic datum")

        self.parent = parent

        # text input
        self.tdatum = self.MakeTextCtrl("", size=(200,-1))
        self.ttrans = self.MakeTextCtrl("", size=(200,-1))

        # search box
        self.searchb = wx.SearchCtrl(self, size=(200,-1),
                                     style=wx.TE_PROCESS_ENTER)

        # button
        self.bupdate = self.MakeButton("Update trans. parms.",
                                       size=(-1,-1))

        # table
        self.tablewidth=675

        # create list control for datum/elipsoid list
        self.datumlist = wx.ListCtrl(self, id=wx.ID_ANY,
                                     size=(675,150),
                                     style=wx.LC_REPORT |
                                     wx.LC_VRULES |
                                     wx.LC_HRULES |
                                     wx.EXPAND)
        self.datumlist.InsertColumn(0, 'Short Name')
        self.datumlist.InsertColumn(1, 'Full EPSG-style name')
        self.datumlist.InsertColumn(2, 'Ellipsoid')
        self.datumlist.InsertColumn(3, 'Parameters')
        self.datumlist.SetColumnWidth(0, 100)
        self.datumlist.SetColumnWidth(1, 225)
        self.datumlist.SetColumnWidth(2, 100)
        self.datumlist.SetColumnWidth(3, 250)

        # create list control for datum transformation parameters list
        self.transformlist = wx.ListCtrl(self, id=wx.ID_ANY,
                                     size=(675,125),
                                     style=wx.LC_REPORT |
                                     wx.LC_VRULES |
                                     wx.LC_HRULES |
                                     wx.EXPAND)
        self.transformlist.InsertColumn(0, 'ID')
        self.transformlist.InsertColumn(1, 'Country')
        self.transformlist.InsertColumn(2, 'Description ')
        self.transformlist.InsertColumn(3, 'Parameters')
        self.transformlist.SetColumnWidth(0, 50)
        self.transformlist.SetColumnWidth(1, 125)
        self.transformlist.SetColumnWidth(2, 250)
        self.transformlist.SetColumnWidth(3, 250)

        # layout
        self.sizer.Add(self.MakeLabel("Geodetic datum:"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, col=1, row=1)
        self.sizer.Add(self.tdatum, 0 ,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=1, col=2)
        self.sizer.Add(self.bupdate, 0 ,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=1, col=3)
        self.sizer.Add(self.MakeLabel("Search in description:"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, col=1, row=2)
        self.sizer.Add(self.searchb, 0 ,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=2, col=2)

        self.sizer.Add(self.datumlist, 0 ,
                       wx.EXPAND |
                       wx.ALIGN_LEFT |
                       wx.ALL, 5, row=3, col=1, colspan=5)

        self.sizer.Add(self.MakeLabel("Transformation parameters:"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, col=1, row=5)
        self.sizer.Add(self.ttrans, 0 ,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=5, col=2)

        self.sizer.Add(self.transformlist, 0 ,
                       wx.EXPAND |
                       wx.ALIGN_LEFT |
                       wx.ALL, 5, row=6, col=1, colspan=5)

        # events
        #wx.EVT_BUTTON(self, self.bbrowse.GetId(), self.OnBrowse)
        #wx.EVT_BUTTON(self, self.bbcodes.GetId(), self.OnBrowseCodes)
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnItemSelected, self.datumlist)
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnTransformSelected, self.transformlist)
        self.Bind(wiz.EVT_WIZARD_PAGE_CHANGING, self.onPageChange)
        self.bupdate.Bind(wx.EVT_BUTTON, self._onBrowseParams)
        self.searchb.Bind(wx.EVT_TEXT_ENTER, self.OnDoSearch, self.searchb)
        self.tdatum.Bind(wx.EVT_TEXT_ENTER, self._onBrowseParams, self.tdatum)

        self._onBrowseDatums(None,None)
        self.datumlist.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.datumlist.SetColumnWidth(1, wx.LIST_AUTOSIZE)
        self.datumlist.SetColumnWidth(2, wx.LIST_AUTOSIZE)
        self.datumlist.SetColumnWidth(3, wx.LIST_AUTOSIZE)

    def onPageChange(self,event):
        self.GetNext().SetPrev(self)
        global datum
        datum = self.tdatum.GetValue()
        global transform
        transform = self.ttrans.GetValue()

    def OnDoSearch(self,event):
        str =  self.searchb.GetValue()
        listItem  = self.datumlist.GetColumn(1)

        for i in range(self.datumlist.GetItemCount()):
            listItem = self.datumlist.GetItem(i,1)
            if listItem.GetText().find(str) > -1:
                datumcode = self.datumlist.GetItem(i, 0)
                self.tdatum.SetValue(datumcode.GetText())
                break

        self._onBrowseDatums(None,str)

    def OnTransformSelected(self,event):
        item = event.GetItem()
        self.ttrans.SetValue(str(item.GetText()))

    def OnItemSelected(self,event):
        item = event.GetItem()
        self.tdatum.SetValue(str(item.GetText()))
        self._onBrowseParams()

    def _onBrowseParams(self, event=None):
        params = [["","Use whole region",""]]
        file = os.path.join(os.getenv("GISBASE"), "etc","datumtransform.table")
        search = self.tdatum.GetValue()

        try:
            f = open(file,"r")
            rex=re.compile('^(.+)\s+"(.+)"\s+"(.+)"\s+"(.+)"')
            for line in f.readlines():
                line = line.strip()
                if line[0] == "#": continue
                id,parm,country,descr = rex.findall(line)[0]
                id=id.strip()
                parm=parm.strip()
                country = country.strip()
                descr=descr.strip()
                if id == search:
                    params.append([parm,country,descr])
            f.close()

            self.transformlist.DeleteAllItems()
            for i in range(len(params)):
                # fill datum transformation parameters list control
                index = self.transformlist.InsertStringItem(i,str(i+1))
                dtp1 = self.transformlist.SetStringItem(index,1,params[i][1])
                dtp2 = self.transformlist.SetStringItem(index,2,params[i][2])
                dtp3 = self.transformlist.SetStringItem(index,3,params[i][0])
                # format list control columns
                if index != '':
                    self.transformlist.SetColumnWidth(0, wx.LIST_AUTOSIZE)
                else:
                    self.transformlist.SetColumnWidth(0, 30)
                if dtp1 != '':
                    self.transformlist.SetColumnWidth(1, wx.LIST_AUTOSIZE)
                else:
                    self.transformlist.SetColumnWidth(1, 150)
                if dtp2 != '':
                    self.transformlist.SetColumnWidth(2, wx.LIST_AUTOSIZE)
                else:
                    self.transformlist.SetColumnWidth(2, 250)
                if dtp3 != '':
                    self.transformlist.SetColumnWidth(3, wx.LIST_AUTOSIZE)
                else:
                    self.transformlist.SetColumnWidth(3, 320)

        except IOError, e:
            self.transformlist.DeleteAllItems()
            dlg = wx.MessageDialog(self, "Could not read datum params: %s " % e,
                                   "Could not read file",  wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()


    def _onBrowseDatums(self,event,search=None):
        try:
            self.datumlist.DeleteAllItems()
            f = open(os.path.join(os.getenv("GISBASE"), "etc","datum.table"),"r")
            # shortname "Full EPSG-style name" ellipsoid dx= dy= dz=
            rex=re.compile('^(.+)\s+"(.+)"\s+(.+)\s+(dx=.+)')
            j=0
            for line in f.readlines():
                line = line.strip()
                if not line: continue
                if line[0] == "#": continue
                shortname, epsgname, ellps, params = rex.findall(line)[0]
                params = params.strip()
                ellps = ellps.strip()
                if search and (shortname.lower().find(search.lower()) > -1 or\
                              ellps.lower().find(search.lower()) > -1 or\
                              epsgname.lower().find(search.lower()) > -1) or\
                        not search:
                    index = self.datumlist.InsertStringItem(j,shortname)
                    self.datumlist.SetStringItem(index,1,epsgname)
                    self.datumlist.SetStringItem(index,2,ellps)
                    self.datumlist.SetStringItem(index,3,params)
                    j  += 1
            f.close()
            self.datumlist.SendSizeEvent()
        except IOError, e:
            dlg = wx.MessageDialog(self, "Could not read datums: %s " % e,
                                   "Could not read file",  wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()


class SummaryPage(TitledPage):
    def __init__(self, wizard, parent):
        TitledPage.__init__(self, wizard, "Summary")

        self.parent = parent

        self.sizer.Add(self.MakeLabel("GRASS database:"), 1, flag=wx.ALIGN_RIGHT, row=1, col=0)
        self.sizer.Add(self.MakeLabel("Location name:"), 1, flag=wx.ALIGN_RIGHT, row=2, col=0)
        self.sizer.Add(wx.StaticLine(self, -1), 0, wx.ALIGN_RIGHT|wx.EXPAND|wx.ALL, 0, row=3, col=0, colspan=2)
        self.sizer.Add((10,10), 1, flag=wx.ALIGN_CENTER_HORIZONTAL, row=4, col=0)
        self.sizer.Add(self.MakeLabel("Projection:"), 1, flag=wx.ALIGN_RIGHT, row=5, col=0)
        self.sizer.Add(self.MakeLabel("North:"), 1, flag=wx.ALIGN_RIGHT, row=6, col=0)
        self.sizer.Add(self.MakeLabel("South:"), 1, flag=wx.ALIGN_RIGHT, row=7, col=0)
        self.sizer.Add(self.MakeLabel("East:"), 1, flag=wx.ALIGN_RIGHT, row=8, col=0)
        self.sizer.Add(self.MakeLabel("West:"), 1, flag=wx.ALIGN_RIGHT, row=9, col=0)
        self.sizer.Add(self.MakeLabel("Resolution:"), 1, flag=wx.ALIGN_RIGHT, row=10, col=0)
        self.sizer.Add(self.MakeLabel("Rows:"), 1, flag=wx.ALIGN_RIGHT, row=12, col=0)
        self.sizer.Add(self.MakeLabel("Columns:"), 1, flag=wx.ALIGN_RIGHT, row=13, col=0)
        self.sizer.Add(self.MakeLabel("Cells:"), 1, flag=wx.ALIGN_RIGHT, row=14, col=0)

        # labels
        self.ldatabase  =	self.MakeLabel("")
        self.llocation  =	self.MakeLabel("")
        self.lprojection =	self.MakeLabel("")
        self.lnorth =	self.MakeLabel("")
        self.lsouth  =	self.MakeLabel("")
        self.least =	self.MakeLabel("")
        self.lwest =	self.MakeLabel("")
        self.lres =	self.MakeLabel("")
        self.lrows =	self.MakeLabel("")
        self.lcols =	self.MakeLabel("")
        self.lcells =	self.MakeLabel("")

        self.sizer.Add(self.ldatabase, 1, flag=wx.ALIGN_LEFT, row=1, col=1)
        self.sizer.Add(self.llocation, 1, flag=wx.ALIGN_LEFT, row=2, col=1)
        self.sizer.Add(self.lprojection, 1, flag=wx.ALIGN_LEFT, row=5, col=1)
        self.sizer.Add(self.lnorth, 1, flag=wx.ALIGN_LEFT, row=6, col=1)
        self.sizer.Add(self.lsouth, 1, flag=wx.ALIGN_LEFT, row=7, col=1)
        self.sizer.Add(self.least, 1, flag=wx.ALIGN_LEFT, row=8, col=1)
        self.sizer.Add(self.lwest, 1, flag=wx.ALIGN_LEFT, row=9, col=1)
        self.sizer.Add(self.lres, 1, flag=wx.ALIGN_LEFT, row=10, col=1)
        self.sizer.Add(self.lrows, 1, flag=wx.ALIGN_LEFT, row=12, col=1)
        self.sizer.Add(self.lcols, 1, flag=wx.ALIGN_LEFT, row=13, col=1)
        self.sizer.Add(self.lcells, 1, flag=wx.ALIGN_LEFT, row=14, col=1)

    def FillVars(self,event=None):
        database = self.parent.startpage.grassdatabase
        location = self.parent.startpage.location
        global coordsys
        global north
        global south
        global east
        global west
        global resolution

        if not coordsys:
            coordsys = 0
        if not north:
            north  = 0
        if not south:
            south = 0
        if not east:
            east = 0
        if not west:
            west = 0
        if not resolution:
            resolution = 1


        #if projection != "latlong":
        rows = int(round((float(north)-float(south))/float(resolution)))
        cols = int(round((float(east)-float(west))/float(resolution)))
        cells = int(rows*cols)

        self.ldatabase.SetLabel(str(database))
        self.llocation.SetLabel(str(location))
        if coordsys == 'epsg':
            label = 'EPSG code %s: %s' % (self.parent.epsgpage.epsgcode,self.parent.epsgpage.epsgdesc)
            self.lprojection.SetLabel(label)
        elif coordsys == 'file':
            label = 'Matches file: %s' % self.parent.filepage.georeffile
            self.lprojection.SetLabel(label)
        else:
            self.lprojection.SetLabel(str(coordsys))
        self.lnorth.SetLabel(str(north))
        self.lsouth.SetLabel(str(south))
        self.least.SetLabel(str(east))
        self.lwest.SetLabel(str(west))
        self.lres.SetLabel(str(resolution))
        self.lrows.SetLabel(str(rows))
        self.lcols.SetLabel(str(cols))
        self.lcells.SetLabel(str(cells))


class BBoxPage(TitledPage):
    def __init__(self, wizard, parent):
        TitledPage.__init__(self, wizard, "Set default region extents and resolution")

        self.parent = parent
        # inputs
        self.ttop = self.MakeTextCtrl("1", size=(150, -1))
        self.tbottom = self.MakeTextCtrl("0", size=(150, -1))
        self.tleft = self.MakeTextCtrl("0", size=(150, -1))
        self.tright = self.MakeTextCtrl("1", size=(150, -1))
        self.tres = self.MakeTextCtrl("1", size=(150, -1))

        self.tgdal = self.MakeTextCtrl("", size=(250, -1))
        self.tdsn = self.MakeTextCtrl("", size=(250, -1))
        # list of layers
        self.layers = []
        self.llayers = wx.ComboBox(self, -1,
                       choices=self.layers,
                       size=(250,-1),
                       style=wx.CB_DROPDOWN)

        # labels
        self.lmessage = wx.StaticText(self,-1, "", size=(300,50))

        # buttons
        self.bbrowsegdal = self.MakeButton("Browse...", size=(150,-1))
        self.bbrowseogr = self.MakeButton("Browse...", size=(150,-1))
        self.bgetlayers = self.MakeButton("Get Layers", size=(150,-1))
        self.bset = self.MakeButton("Set coordinates", size=(150,-1))

        # list of states
        self.states = []
        self.coords = []
        try:
            f = open(os.path.join(os.getenv("GISBASE"),"etc","wx","states.txt"),"r")
            for line in f.readlines():
                if line[0] == "#":
                    continue
                state,coord = line.split(";")
                coord = coord.replace(","," ")
                self.states.append(state)
                self.coords.append(coord.split())
            f.close()
        except:
            pass
        # NOTE: ComboCtcl should come here, but nobody knows, how to
        # implement it
        # self.stateslist = wx.ListCtrl(self,
        #                    style=wx.LC_LIST|wx.LC_SINGLE_SEL|wx.SIMPLE_BORDER)
        # self.cstate = wx.combo.ComboCtrl(self, -1, pos=(50, 170), size=(150, -1),
        #          style=wx.CB_READONLY)

        self.cstate = wx.ComboBox(self, -1,
                       size=(250,-1),
                       choices=self.states,
                       style=wx.CB_DROPDOWN)

        # layout
        self.sizer.Add(self.MakeLabel("North"), 0,
                       wx.ALIGN_CENTER_HORIZONTAL |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 0, row=1,col=2)
        self.sizer.Add(self.ttop, 0,
                       wx.ALIGN_CENTER_HORIZONTAL |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=2,col=2)

        self.sizer.Add(self.MakeLabel("West"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 0, row=3,col=0)
        self.sizer.Add(self.tleft, 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5,  row=3,col=1)

        self.sizer.Add(self.tright, 0,
                       wx.ALIGN_CENTER_HORIZONTAL |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5,  row=3,col=3)
        self.sizer.Add(self.MakeLabel("East"), 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 0, row=3,col=4)

        self.sizer.Add(self.tbottom, 0,
                       wx.ALIGN_CENTER_HORIZONTAL |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=4,col=2)
        self.sizer.Add(self.MakeLabel("South"), 0,
                       wx.ALIGN_CENTER_HORIZONTAL |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 0, row=5,col=2)

        self.sizer.Add(self.MakeLabel("Initial resolution"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=6,col=1)
        self.sizer.Add(self.tres, 0,
                       wx.ALIGN_CENTER_HORIZONTAL |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=6,col=2)
        self.sizer.Add(self.bset, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=6, col=3 )

        self.sizer.Add(wx.StaticLine(self, -1), 0, wx.EXPAND|wx.ALL, 0, row=7, col=0, colspan=6)

        self.sizer.Add(self.MakeLabel("Match extents of georeferenced raster map or image"), 3,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=8,col=0, colspan=3)

        self.sizer.Add(self.MakeLabel("File:"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=9,col=0, colspan=1)
        self.sizer.Add(self.tgdal, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=9,col=1, colspan=2)
        self.sizer.Add(self.bbrowsegdal, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=9,col=3)

        self.sizer.Add(wx.StaticLine(self, -1), 0,
                       wx.EXPAND|wx.ALL, 0,
                       row=10, col=0, colspan=6)

        self.sizer.Add(self.MakeLabel("Match extents of georeferenced vector map"), 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=11,col=0, colspan=3 )

        self.sizer.Add(self.MakeLabel("Data source/directory:"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=12,col=0, colspan=1)
        self.sizer.Add(self.tdsn, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=12, col=1, colspan=2)
        self.sizer.Add(self.bbrowseogr, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=12, col=3)

        self.sizer.Add(self.MakeLabel("Layer/file:"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=13,col=0, colspan=1)
        self.sizer.Add(self.llayers, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=13,col=1, colspan=2)
        self.sizer.Add(self.bgetlayers, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=13,col=3)

        self.sizer.Add(wx.StaticLine(self, -1), 0,
                       wx.EXPAND|wx.ALL |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5,
                       row=14, col=0, colspan=6)
        self.sizer.Add(self.MakeLabel("Match extents of selected country"), 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=15,col=0, colspan=3)
        self.sizer.Add(self.cstate, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=16,col=1, colspan=2)

        self.sizer.Add(self.lmessage, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5,
                       row=17,col=1, colspan=3)

        self.Bind(wiz.EVT_WIZARD_PAGE_CHANGING, self.onPageChange)
        self.Bind(wx.EVT_COMBOBOX, self.OnItemSelected, self.cstate)
        self.Bind(wx.EVT_TEXT, self.OnStateText, self.cstate)
        self.Bind(wx.EVT_BUTTON, self.OnSetButton, self.bset)
        self.Bind(wx.EVT_BUTTON, self.OnBrowseGdal, self.bbrowsegdal)
        self.Bind(wx.EVT_BUTTON, self.OnBrowseOGR, self.bbrowseogr)
        self.Bind(wx.EVT_BUTTON, self.OnGetOGRLayers, self.bgetlayers)

    def OnBrowseGdal(self, event):
        dlg = wx.FileDialog(self, "Choose a raster file:", os.getcwd(), "", "*.*", wx.OPEN)
        path = ""
        if dlg.ShowModal() == wx.ID_OK:
                    path = dlg.GetPath()
                    self.tgdal.SetValue(path)
        dlg.Destroy()

        self.OnSetButton()

    def OnBrowseOGR(self, event):
        dlg = wx.FileDialog(self, "Choose a data source name:", os.getcwd(), "", "*.*", wx.OPEN)
        path = ""
        if dlg.ShowModal() == wx.ID_OK:
                    path = dlg.GetPath()
                    self.tdsn.SetValue(path)
        dlg.Destroy()
        self.OnGetOGRLayers(None)

    def OnSetButton(self,event=None):
        if self.tgdal.GetValue():
            self.__setGDAL()
        if self.tdsn.GetValue() and self.llayers.GetSelection()>-1:
            self.__setOGR()
        elif self.cstate.GetSelection() > -1:
            sys.stderr.write("##############"+str(self.cstate.GetSelection())+"\n")
            self.OnItemSelected(None)

    def OnGetOGRLayers(self, event):
        path = self.tdsn.GetValue()
        line = ""

        sys.stderr.write(path+"####\n")
        self.layers = []
        self.llayers.Clear()
        cmd = os.popen("ogrinfo -so %s\n" % (path))
        line = cmd.readline()
        # 1: cr (Polygon)
        rex = re.compile("^(\d+):\s+(.+)\s+\(.*\)")
        while 1:
            if not line or line == "":
                break
            try:
                sys.stderr.write("#####"+line+"####\n")
                number, name = rex.findall(line)[0]
                self.layers.append(name)
            except:
                pass
            line = cmd.readline()
        self.llayers.AppendItems(self.layers)
        #sys.stderr.write(str( self.layers)+"\n")
        #self.sizer.Remove(self.llayers)
        #self.llayers = wx.ComboBox(self, -1, choices=self.layers, size=(100,-1),
        #        style=wx.CB_DROPDOWN)
        #self.sizer.Add(self.llayers, 0, wx.ALIGN_CENTER_VERTICAL, row=12,col=3)
        #self.sizer.ShowItems(True)
        self.OnSetButton()
        pass

    def __setOGR(self):
        layer = self.layers[self.llayers.GetSelection()]
        path = self.tdsn.GetValue()
        number="-?\d+\.\d+"
        line = ""

        #test values
        self.ttop.SetValue(500)
        self.tleft.SetValue(500)

        #Extent: (-146.976217, -55.985484) - (72.774632, 80.594358)
        rex = re.compile("\((%s),\s*(%s)\)\s*-\s*\((%s),\s*(%s)\)" %(number, number, number, number))
        cmd = os.popen("ogrinfo -so %s %s" % (path ,layer))
        line = cmd.readline()
        while 1:
            if not line or line == "":
                    break
            sys.stderr.write(line+"\n")
            if line.find("Extent")>-1:
                sys.stderr.write(line[0]+"#####\n")
                x1,y1,x2,y2 = rex.findall(line)[0]
                self.tbottom.SetValue(y1)
                self.tleft.SetValue(x1)
                self.ttop.SetValue(y2)
                self.tright.SetValue(x2)
                break
            line = cmd.readline()
        return

    def __setGDAL(self):
        path = self.tgdal.GetValue()
        line = ""
        number="-?\d+\.\d+"

        #test values
        self.ttop.SetValue(500)
        self.tleft.SetValue(500)

        # Upper Left  (    0.0,    0.0)
        rex=re.compile("\(\s*(%s)\s*,\s*(%s)\)" % (number, number))
        obj = os.popen("gdalinfo %s | grep \"Upper\|Lower\"" % path)

        line = obj.readline()
        while 1:
            sys.stderr.write(line+"\n")
            if not line:
                    break
            if line.find("Upper Left")>-1:
                x,y = rex.findall(line)[0]
                self.ttop.SetValue(y)
                self.tleft.SetValue(x)
            if line.find("Lower Right")>-1:
                x,y = rex.findall(line)[0]
                self.tbottom.SetValue(y)
                self.tright.SetValue(x)
            line = obj.readline()
        return

    def OnStateText(self,event):
        item = self.llayers.FindString(event.GetString())
        self.llayers.SetSelection(item)
        self.llayers.SetValue(self.llayers.GetStringSelection())
        pass
        #.log.WriteText('EvtTextEnter: %s' % event.GetString())
        #sys.stderr.write(event.GetString()+"\n")
        #text=event.GetString().lower()
        #for idx in range(len(self.states)):
        #    if self.states[idx].lower() == text:
        #        self.cstate.Select(idx)
        #        break

    def onPageChange(self, event):
        self.GetNext().FillVars()
        self.GetNext().SetPrev(self)

        global north
        north = self.ttop.GetValue()
        global south
        south = self.tbottom.GetValue()
        global east
        east = self.tright.GetValue()
        global west
        west = self.tleft.GetValue()
        global resolution
        resolution = self.tres.GetValue()

    def OnItemSelected(self, event):
        item = self.cstate.GetSelection()
        w,s,e,n = self.coords[item]
        #  4
        # 1 3
        #  2

        if self.parent.csystemspage.cs == "latlong":
            pass
        if self.parent.csystemspage.cs == "xy":
            pass
        else:
            if self.parent.csystemspage.cs == "epsg":
                to="+init=epsg:%d" % (int(self.parent.epsgpage.tcode.GetValue()))
            elif self.parent.csystemspage.cs == "custom":
                to="+proj=%s" % (self.parent.projpage.tproj.GetValue())
            elif self.parent.csystemspage.cs == "utm":
                to="+proj=utm"
            else:
                sys.stderr.write(self.parent.csystemspage.cs+"\n")

            try:
                sin, sout = os.popen2("cs2cs +proj=latlong +datum=WGS84 +to %s" %  (to))
                sin.write("%s %s\n" % (w,s))
                sin.write("%s %s\n" % (e,n))
                sin.close()
                w,s,t = sout.readline().split()
                e,n,t = sout.readline().split()
                self.lmessage.SetLabel("")
            except:
                n = s = w = e="NULL"
                self.lmessage.SetLabel("Unable to calculate country extends:\n cs2cs +proj=latlong +datum=WGS84 +to %s"% to)

        self.ttop.SetValue( str(n) )
        self.tbottom.SetValue( str(s) )
        self.tright.SetValue( str(e) )
        self.tleft.SetValue( str(w) )

class ProjectionsPage(TitledPage):
    def __init__(self, wizard, parent):
        TitledPage.__init__(self, wizard, "Choose projection name")

        self.parent = parent
        # text input
        self.tproj = self.MakeTextCtrl("", size=(200,-1))

        # search box
        self.searchb = wx.SearchCtrl(self, size=(200,-1),
                                     style=wx.TE_PROCESS_ENTER)

        # table
        self.tablewidth=675
        self.list = wx.ListCtrl(self, id=wx.ID_ANY,
                                     size=(675,275),
                                     style=wx.LC_REPORT |
                                     wx.LC_VRULES |
                                     wx.LC_HRULES |
                                     wx.EXPAND)
        self.list.InsertColumn(0, 'Name')
        self.list.InsertColumn(1, 'Description')
        self.list.SetColumnWidth(0, 100)
        self.list.SetColumnWidth(1, 575)

        # layout
        self.sizer.Add(self.MakeLabel("Projection name:"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=1, col=2)
        self.sizer.Add(self.tproj, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=1, col=3)

        self.sizer.Add(self.MakeLabel("Search in projection description"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=2, col=2)
        self.sizer.Add(self.searchb, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=2, col=3)

        self.sizer.Add(self.list,
                       wx.EXPAND |
                       wx.ALIGN_LEFT |
                       wx.ALL, 5, row=3, col=1, colspan=4)

        # events
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnItemSelected, self.list)
        self.searchb.Bind(wx.EVT_TEXT_ENTER, self.OnDoSearch, self.searchb)
        self.Bind(wiz.EVT_WIZARD_PAGE_CHANGING, self.onPageChange)

        self._onBrowseDatums(None, None)

        self.list.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.list.SetColumnWidth(1, wx.LIST_AUTOSIZE)

    def OnDoSearch(self,event):
        str =  self.searchb.GetValue()
        listItem  = self.list.GetColumn(1)

        for i in range(self.list.GetItemCount()):
            listItem = self.list.GetItem(i,1)
            if listItem.GetText().find(str) > -1:
                datumcode = self.list.GetItem(i, 0)
                self.tproj.SetValue(datumcode.GetText())
                break

        self._onBrowseDatums(None,str)


    def OnItemSelected(self,event):
        item = event.GetItem()
        self.tproj.SetValue(str(item.GetText()))


    def _onBrowseDatums(self,event,search=None):
        try:
            self.list.DeleteAllItems()
            f = open(os.path.join(os.getenv("GISBASE"), "etc","projections"),"r")
            i=1
            j = 0
            descr = None
            proj = None
            for line in f.readlines():
                line = line.strip()
                if not line:
                    continue
                if line[0] == "#":
                    continue
                proj,descr = string.split(line, ":", maxsplit=1)
                if search and (descr.lower().find(search.lower()) > -1 or\
                              proj.lower().find(search.lower()) > -1) or\
                        not search:
                    self.list.InsertStringItem(j,proj)
                    self.list.SetStringItem(j,1,descr)
                    j  += 1
                    # reset
                    descr = None; proj = ""
            f.close()
            self.SendSizeEvent()
        except StandardError, e:
            dlg = wx.MessageDialog(self, "Could not read datums: %s " % e,
                                   "Could not read file",  wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()

    def onPageChange(self,event):
        global projection
        projection = self.tproj.GetValue()

class GeoreferencedFilePage(TitledPage):
    def __init__(self, wizard, parent):
        TitledPage.__init__(self, wizard, "Select georeferenced file")

        self.georeffile = ''

        # create controls
        self.lfile= wx.StaticText(self, -1, "Georeferenced file: ",
                style=wx.ALIGN_RIGHT)
        self.tfile = wx.TextCtrl(self,-1, "", size=(300,-1))
        self.bbrowse = wx.Button(self, -1, "Browse...")

        # do layout
        self.sizer.Add(self.lfile, 0, wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTRE_VERTICAL |
                       wx.ALL, 5, row=1, col=2)
        self.sizer.Add(self.tfile, 0, wx.ALIGN_LEFT |
                       wx.ALIGN_CENTRE_VERTICAL |
                       wx.ALL, 5, row=1, col=3)
        self.sizer.Add(self.bbrowse, 0, wx.ALIGN_LEFT |
                       wx.ALL, 5, row=1, col=4)

        self.bbrowse.Bind(wx.EVT_BUTTON, self.OnBrowse)
        self.Bind(wx.EVT_TEXT, self.OnText, self.tfile)

    def OnText(self, event):
        self.georeffile = event.GetString()

    def OnBrowse(self, event):

        dlg = wx.FileDialog(self, "Choose a georeferenced file:", os.getcwd(), "", "*.*", wx.OPEN)
        if dlg.ShowModal() == wx.ID_OK:
                    path = dlg.GetPath()
                    self.tfile.SetValue(path)
        dlg.Destroy()

    def OnCreate(self, event):
        pass

class EPSGPage(TitledPage):
    def __init__(self, wizard, parent):
        TitledPage.__init__(self, wizard, "Choose EPSG Code")
        self.parent = parent

        # labels
        self.lfile= wx.StaticText(self, -1, "Path to the EPSG-codes file: ",
                style=wx.ALIGN_RIGHT)
        self.lcode= wx.StaticText(self, -1, "EPSG code: ",
                style=wx.ALIGN_RIGHT)
        self.lsearch= wx.StaticText(self, -1, "Search in code description: ",
                style=wx.ALIGN_RIGHT)

        # text input
        epsgdir = os.path.join(os.environ["GRASS_PROJSHARE"], 'epsg')
        self.tfile = wx.TextCtrl(self,-1, epsgdir, size=(200,-1))
        self.tcode = wx.TextCtrl(self,-1, "", size=(200,-1))
        self.epsgdesc = ''
        self.epsgcode = ''

        # buttons
        self.bbrowse = wx.Button(self, -1, "Browse...")
        self.bbcodes = wx.Button(self, -1, "Browse Codes...")

        # search box
        self.searchb = wx.SearchCtrl(self, size=(200,-1), style=wx.TE_PROCESS_ENTER)

        # table
        self.tablewidth=675
        self.epsgs = wx.ListCtrl(self, id=wx.ID_ANY,
                     size=(650,275),
                     style=wx.LC_REPORT|
                     wx.LC_HRULES|
                     wx.EXPAND)
        self.epsgs.InsertColumn(0, 'EPSG', wx.LIST_FORMAT_CENTRE)
        self.epsgs.InsertColumn(1, 'Description', wx.LIST_FORMAT_LEFT)
        self.epsgs.InsertColumn(2, 'Parameters', wx.LIST_FORMAT_LEFT)
        self.epsgs.SetColumnWidth(0, 50)
        self.epsgs.SetColumnWidth(1, 300)
        self.epsgs.SetColumnWidth(2, 325)

        # layout
        self.sizer.Add(self.lfile, 0, wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, col=1, row=1)
        self.sizer.Add(self.tfile, 0, wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=1, col=2)
        self.sizer.Add(self.bbrowse, 0, wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=1, col=3)

        self.sizer.Add(self.lcode, 0, wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, col=1, row=2)
        self.sizer.Add(self.tcode, 0, wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=2, col=2)

        self.sizer.Add(self.lsearch, 0, wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, col=1, row=3)
        self.sizer.Add(self.searchb, 0, wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=3, col=2)
        self.sizer.Add(self.bbcodes, 0 , wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5, row=3, col=3)

        self.sizer.Add(self.epsgs, wx.ALIGN_LEFT|wx.EXPAND, 0, row=4, col=1, colspan=5)

        # events
        self.Bind(wx.EVT_BUTTON, self.OnBrowse, self.bbrowse)
        self.Bind(wx.EVT_BUTTON, self.OnBrowseCodes, self.bbcodes)
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnItemSelected, self.epsgs)
        self.searchb.Bind(wx.EVT_TEXT_ENTER, self.OnDoSearch, self.searchb)
        self.Bind(wiz.EVT_WIZARD_PAGE_CHANGING, self.onPageChange)

    def onPageChange(self, event):
        self.parent.bboxpage.SetPrev(self)

    def OnDoSearch(self,event):
        str =  self.searchb.GetValue()
        listItem  = self.epsgs.GetColumn(1)

        for i in range(self.epsgs.GetItemCount()):
            listItem = self.epsgs.GetItem(i,1)
            if listItem.GetText().find(str) > -1:
                self.epsgcode = self.epsgs.GetItem(i, 0)
                self.tcode.SetValue(self.epsgcode.GetText())
                break

        self.OnBrowseCodes(None,str)


    def OnBrowse(self, event):

        dlg = wx.FileDialog(self, "Choose a georeferenced file:",
        "/", "", "*.*", wx.OPEN)
        if dlg.ShowModal() == wx.ID_OK:
                    path = dlg.GetPath()
                    self.tfile.SetValue(path)
        dlg.Destroy()

    def OnItemSelected(self,event):
        index = event.m_itemIndex
        item = event.GetItem()

        self.epsgcode = item.GetText()
        self.epsgdesc = self.epsgs.GetItem(index, 1).GetText()
        self.tcode.SetValue(str(self.epsgcode))

    def OnBrowseCodes(self,event,search=None):
        try:
            self.epsgs.DeleteAllItems()
            f = open(self.tfile.GetValue(),"r")
            i=1
            j = 0
            descr = None
            code = None
            params = ""
            #self.epsgs.ClearAll()
            for line in f.readlines():
                line = line.strip()
                if line.find("#") == 0:
                    descr = line[1:].strip()
                elif line.find("<") == 0:
                    code = line.split(" ")[0]
                    for par in line.split(" ")[1:]:
                        params += par + " "
                    code = code[1:-1]
                if code == None: code = 'no code'
                if descr == None: descr = 'no description'
                if params == None: params = 'no parameters'
                if i%2 == 0:
                    if search and descr.lower().find(search.lower()) > -1 or\
                        not search:
                        index = self.epsgs.InsertStringItem(j, code)
                        self.epsgs.SetStringItem(index, 1, descr)
                        self.epsgs.SetStringItem(index, 2, params)
                        j  += 1
                    # reset
                    descr = None; code = None; params = ""
#                if i%2 == 0:
#                    self.epsgs.SetItemBackgroundColour(i, "grey")
                i += 1
            f.close()
            self.epsgs.SetColumnWidth(1, wx.LIST_AUTOSIZE)
            self.epsgs.SetColumnWidth(2, wx.LIST_AUTOSIZE)
            self.SendSizeEvent()
        except StandardError, e:
            dlg = wx.MessageDialog(self, "Could not read EPGS codes: %s " % e,
                                   "Could not read file",  wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()

class CoordinateSystemPage(TitledPage):
    def __init__(self, wizard, parent):
        TitledPage.__init__(self, wizard, "Choose coordinate system for location")

        self.parent = parent
        self.cs = "xy"

        # toggles
        self.radio1 = wx.RadioButton( self, -1, " XY (arbitrary non-earth coordinate system)", style = wx.RB_GROUP )
        self.radio2 = wx.RadioButton( self, -1, " Latitude/Longitude (geographical) " )
        self.radio3 = wx.RadioButton( self, -1, " Universal Transverse Mercator (UTM) " )
        self.radio4 = wx.RadioButton( self, -1, " Other coordinate system " )
        self.radio5 = wx.RadioButton( self, -1, " Based on EPSG code " )
        self.radio6 = wx.RadioButton( self, -1, " Based on Georeferenced file " )

        # layout
        self.sizer.Add(self.radio1, 0, wx.ALIGN_LEFT, row=1, col=2)
        self.sizer.Add(self.radio2, 0, wx.ALIGN_LEFT, row=2, col=2)
        self.sizer.Add(self.radio3, 0, wx.ALIGN_LEFT, row=3, col=2)
        self.sizer.Add(self.radio4, 0, wx.ALIGN_LEFT, row=4, col=2)
        self.sizer.Add(self.radio5, 0, wx.ALIGN_LEFT, row=5, col=2)
        self.sizer.Add(self.radio6, 0, wx.ALIGN_LEFT, row=6, col=2)

        # bindings
        self.Bind(wx.EVT_RADIOBUTTON, self.SetVal, id=self.radio1.GetId())
        self.Bind(wx.EVT_RADIOBUTTON, self.SetVal, id=self.radio2.GetId())
        self.Bind(wx.EVT_RADIOBUTTON, self.SetVal, id=self.radio3.GetId())
        self.Bind(wx.EVT_RADIOBUTTON, self.SetVal, id=self.radio4.GetId())
        self.Bind(wx.EVT_RADIOBUTTON, self.SetVal, id=self.radio5.GetId())
        self.Bind(wx.EVT_RADIOBUTTON, self.SetVal, id=self.radio6.GetId())
        self.Bind(wiz.EVT_WIZARD_PAGE_CHANGING, self.onPageChange)

    def SetVal(self,event):
        global coordsys
        if event.GetId() == self.radio1.GetId():
            coordsys = "xy"
            self.SetNext(self.parent.bboxpage)
            self.parent.bboxpage.cstate.Enable(False)
        elif event.GetId() == self.radio2.GetId():
            coordsys = "latlong"
            self.SetNext(self.parent.datumpage)
            self.parent.datumpage.SetPrev(self.parent.csystemspage)
            self.parent.bboxpage.SetPrev(self.parent.datumpage)
        elif event.GetId() == self.radio3.GetId():
            coordsys = "utm"
            self.SetNext(self.parent.utmpage)
            self.parent.datumpage.SetPrev(self.parent.utmpage)
        elif event.GetId() == self.radio4.GetId():
            coordsys = "custom"
            self.SetNext(self.parent.projpage)
            self.parent.datumpage.SetPrev(self.parent.projpage)
            self.parent.bboxpage.SetPrev(self.parent.datumpage)
        elif event.GetId() == self.radio5.GetId():
            coordsys = "epsg"
            self.SetNext(self.parent.epsgpage)
            self.parent.sumpage.SetPrev(self.parent.epsgpage)
        elif event.GetId() == self.radio6.GetId():
            coordsys = "file"
            self.SetNext(self.parent.filepage)
            self.parent.sumpage.SetPrev(self.parent.filepage)

    def onPageChange(self,event=None):
        global coordsys
        if coordsys == "xy":
            self.parent.bboxpage.cstate.Enable(False)
        else:
            self.parent.bboxpage.cstate.Enable(True)

class UTMPage(TitledPage):
    def __init__(self, wizard, parent):
        TitledPage.__init__(self, wizard, "Choose zone for UTM coordinate system")

        self.parent = parent
        self.text_utm = self.MakeTextCtrl(size=(300,-1))
        self.label_utm = self.MakeLabel("Set UTM zone: ")

        self.sizer.Add(self.label_utm, 0, wx.ALIGN_LEFT, 5, row=1,col=1)
        self.sizer.Add(self.text_utm, 0, wx.ALIGN_LEFT, 5, row=1,col=2)


    def GetUtm(self):
        return int(self.text_utm.GetValue())



class DatabasePage(TitledPage):
    def __init__(self, wizard, parent, grassdatabase):
        TitledPage.__init__(self, wizard, "Define GRASS database and new Location Name")

        self.grassdatabase = ''
        self.location = ''

        # buttons
        self.bbrowse = self.MakeButton("Browse...")

        # text controls
        self.tgisdbase = self.MakeTextCtrl(grassdatabase, size=(300, -1))
        self.tlocation = self.MakeTextCtrl("newLocation", size=(300, -1))

        # layout
        self.sizer.Add(self.MakeLabel("GIS Data Directory:"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5,
                       row=1, col=2)
        self.sizer.Add(self.tgisdbase,0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5,
                       row=1, col=3)
        self.sizer.Add(self.bbrowse, 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5,
                       row=1, col=4)
        #
        self.sizer.Add(self.MakeLabel("Project Location"), 0,
                       wx.ALIGN_RIGHT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5,
                       row=2, col=2)
        self.sizer.Add(self.tlocation,0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5,
                       row=2, col=3)
        self.sizer.Add(self.MakeLabel("(projection/coordinate system)"), 0,
                       wx.ALIGN_LEFT |
                       wx.ALIGN_CENTER_VERTICAL |
                       wx.ALL, 5,
                       row=2, col=4)

        # bindings
        self.Bind(wiz.EVT_WIZARD_PAGE_CHANGING, self.onPageChanging)

        self.Bind(wiz.EVT_WIZARD_PAGE_CHANGED, self.onPageChanged)

    def onPageChanging(self,event=None):
        if os.path.isdir(os.path.join(self.tgisdbase.GetValue(),self.tlocation.GetValue())):
            dlg = wx.MessageDialog(self, "Could not create new location: <%s> directory exists "\
                    % str(self.tlocation.GetValue()),"Could not create location",  wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
            event.Veto()
            return

        if not self.tlocation.GetValue():
            dlg = wx.MessageDialog(self, "Could not create new location: location not set "\
                    ,"Could not create location",  wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
            event.Veto()
            return

        self.location = self.tlocation.GetValue()
        self.grassdatabase = self.tgisdbase.GetValue()

    def onPageChanged(self,event=None):
        self.grassdatabase = self.tgisdbase.GetValue()
        self.location = self.tlocation.GetValue()


class GWizard:
    def __init__(self, parent, grassdatabase):
        wizbmp = wx.Image(os.path.join(os.getenv("GISBASE"),"etc","wx","images","wizard.png"), wx.BITMAP_TYPE_PNG)
        wizbmp.Rescale(250,600)
        wizbmp = wizbmp.ConvertToBitmap()
        self.wizard = wiz.Wizard(parent, -1, "Define new Location",
                bitmap=wizbmp)
        self.startpage = DatabasePage(self.wizard, self, grassdatabase)
        self.csystemspage = CoordinateSystemPage(self.wizard, self)
        self.epsgpage = EPSGPage(self.wizard, self)
        self.bboxpage = BBoxPage(self.wizard, self)
        self.filepage = GeoreferencedFilePage(self.wizard, self)
        self.projpage = ProjectionsPage(self.wizard, self)
        self.sumpage = SummaryPage(self.wizard, self)
        self.datumpage = DatumPage(self.wizard, self)
        self.utmpage = UTMPage(self.wizard,self)

        global coordsys

        # Set the initial order of the pages
        # it should follow the epsg line
        self.startpage.SetNext(self.csystemspage)

        self.csystemspage.SetPrev(self.startpage)
        self.csystemspage.SetNext(self.bboxpage)

        self.epsgpage.SetPrev(self.csystemspage)
        self.epsgpage.SetNext(self.bboxpage)

        self.projpage.SetPrev(self.csystemspage)
        self.projpage.SetNext(self.datumpage)

        self.filepage.SetPrev(self.csystemspage)
        self.filepage.SetNext(self.bboxpage)

        self.datumpage.SetNext(self.bboxpage)

        if coordsys == "epsg":
            self.bboxpage.SetPrev(self.epsgpage)
        elif coordsys == "file":
            self.bboxpage.SetPrev(self.filepage)
        elif coordsys == "xy":
            self.bboxpage.SetPrev(self.startpage)
        else:
            self.bboxpage.SetPrev(self.csystemspage)

        self.bboxpage.SetNext(self.sumpage)

        self.sumpage.SetPrev(self.bboxpage)

        self.utmpage.SetPrev(self.csystemspage)
        self.utmpage.SetNext(self.datumpage)

        self.wizard.FitToPage(self.bboxpage)

        success = False

        if self.wizard.RunWizard(self.startpage):
            success = self.onWizFinished()
            if success == True:
                wx.MessageBox("New location created.")
            else:
                wx.MessageBox("Unable to create new location.")
        else:
            wx.MessageBox("Location wizard canceled. New location not created.")

        self.wizard.Destroy()

    def onWizFinished(self):
        database = self.startpage.grassdatabase
        location = self.startpage.location
        global coordsys
        success = ''

#        wx.MessageBox("finished database: %s, location: %s, coordsys: %s" % (database, location, coordsys))
        if os.path.isdir(os.path.join(database,location)):
            dlg = wx.MessageDialog(self, "Could not create new location: %s already exists"
                                   % os.path.join(database,location),"Could not create location",
                                   wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
            return False

        if coordsys == "xy":
            success = self.XYCreate()
        elif coordsys == "latlong":
            rows = int(round((float(north)-float(south))/float(resolution)))
            cols = int(round((float(east)-float(west))/float(resolution)))
            cells = int(rows*cols)
            success = self.LatlongCreate()
        elif coordsys == "utm":
            success = self.UTMCreate()
        elif coordsys == "custom":
            success = self.CustomCreate()
        elif coordsys == "epsg":
            success = self.EPSGCreate()
        elif coordsys == "file":
            success = self.FileCreate()

        return success

    def XYCreate(self):
        """
        Create an XY location
        """
        wx.MessageBox('Not implemented: Create xy location')

    def LatlongCreate(self):
        """
        Create a new Lat/Long location
        """
        projection = self.projpage.tproj.GetValue()
        datum = self.datumpage.tdatum.GetValue()
        transform = self.datumpage.ttrans.GetValue()

        wx.MessageBox('Not implemented: Create input for g.proj from \n%s \n%s \n%s' %
                      (projection, datum, transform))

    def UTMCreate(self):
        """
        Create a new UTM location
        """
        projection = self.projpage.tproj.GetValue()
        datum = self.datumpage.tdatum.GetValue()
        transform = self.datumpage.ttrans.GetValue()


        wx.MessageBox('Not implemented: Create input for g.proj from \n%s \n%s \n%s' %
                      (projection, datum, transform))

    def CustomCreate(self):
        """
        Create a new custom-defined location
        """
        projection = self.projpage.tproj.GetValue()
        datum = self.datumpage.tdatum.GetValue()
        transform = self.datumpage.ttrans.GetValue()

        wx.MessageBox('Not implemented: Create input for g.proj from \n%s \n%s \n%s' %
                      (projection, datum, transform))

    def EPSGCreate(self):
        """
        Create a new location from an EPSG code.
        """
        epsgcode = self.epsgpage.epsgcode
        epsgdesc = self.epsgpage.epsgdesc
        location = self.startpage.location
        cmdlist = []

        if not epsgcode:
            dlg = wx.MessageDialog(self.wizard, "Could not create new location: EPSG Code value missing",
                    "Could not create location",  wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
            return False

        dlg = wx.MessageDialog(self.wizard, "New location '%s' will be created georeferenced to \
                               EPSG code %s: %s" % (location, epsgcode, epsgdesc),
                               "Create new location from EPSG code?",
                               wx.YES_NO|wx.YES_DEFAULT|wx.ICON_QUESTION)
        if dlg.ShowModal() == wx.ID_NO:
            dlg.Destroy()
            return False
        else:
            dlg.Destroy()

        # creating location
        # all credit to Michael Barton and his file_option.tcl and
        # Markus Neteler
        try:
            cmdlist = ['g.proj','epsg=%s' % epsgcode,'datumtrans=-1']
            p = cmd.Command(cmdlist)
            dtoptions = p.module_stdout.read()
            if dtoptions != None:
                dtrans = ''
                # open a dialog to select datum transform number
                dlg = wx.TextEntryDialog(self.wizard, dtoptions,
                                         caption='Select the number of a datum transformation to use',
                                         defaultValue='1',
                                         style=wx.TE_WORDWRAP|wx.MINIMIZE_BOX|wx.MAXIMIZE_BOX|
                                         wx.RESIZE_BORDER|wx.VSCROLL|
                                         wx.OK|wx.CANCEL)

                if dlg.ShowModal() == wx.ID_CANCEL:
                    dlg.Destroy()
                    return False
                else:
                    dtrans = dlg.GetValue()
                    if dtrans != '':
                        dlg.Destroy()
                    else:
                        wx.MessageBox('You must select a datum transform')
                        return False

                cmdlist = ['g.proj','-c','epsg=%s' % epsgcode,'location=%s' % location,'datumtrans=%s' % dtrans]
            else:
                cmdlist = ['g.proj','-c','epsg=%s' % epsgcode,'location=%s' % location,'datumtrans=1']

            cmd.Command(cmdlist)
            return True

        except StandardError, e:
            dlg = wx.MessageDialog(self.wizard, "Could not create new location: %s " % str(e),
                                   "Could not create location",  wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
            return False

    def FileCreate(self):
        """
        Create a new location from a georeferenced file
        """
        georeffile = self.filepage.georeffile
        location = self.startpage.location

        cmdlist = []

        dlg = wx.MessageDialog(self.wizard, "New location '%s' will be created georeferenced to file '%s'"\
                               % (location, georeffile), "Create new location from georeferenced file?",
                               wx.YES_NO|wx.YES_DEFAULT|wx.ICON_QUESTION)
        if dlg.ShowModal() == wx.ID_NO:
            dlg.Destroy()
            return False
        else:
            dlg.Destroy()

        if not os.path.isfile(georeffile):
            dlg = wx.MessageDialog(self.wizard, "Could not create new location: could not find file %s" % georeffile,
                                   "Could not create location",  wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
            return False

        if not georeffile:
            dlg = wx.MessageDialog(self.wizard, "Could not create new location: georeferenced file not set",
                    "Could not create location",  wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
            return False

        # creating location
        # all credit to Michael Barton and his file_option.tcl and
        # Markus Neteler
        try:
            cmdlist = ['g.proj','-c','georef=%s' % georeffile,'location=%s' % location]
            cmd.Command(cmdlist)
            return True

        except StandardError, e:
            dlg = wx.MessageDialog(self.wizard, "Could not create new location: %s " % str(e),
                                   "Could not create location",  wx.OK|wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
            return False

if __name__ == "__main__":
    gWizard = GWizard(None,  "")
    GRASSStartUp = GWizard.StartUp(0)
    GRASSStartUp.MainLoop()
    #app.MainLoop()
