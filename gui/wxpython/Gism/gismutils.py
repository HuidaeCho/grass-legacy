import os,sys
import wx
import wx.lib.customtreectrl as CT
import wx.combo

import track
import select
import menuform
import optpanels.raster_prop as raster_prop
import optpanels.vectopt as vectopt
import optpanels.cmdopt as cmdopt

#FIXME??
try:
   import subprocess
except:
   from compat import subprocess


gmpath = os.getenv("GISBASE") + "/etc/wx/gism/"
sys.path.append(gmpath)

icons = ""

if not os.getenv("GRASS_ICONPATH"):
    icons = os.getenv("GISBASE") + "/etc/gui/icons/"
else:
    icons = os.environ["GRASS_ICONPATH"]

class LayerTree(CT.CustomTreeCtrl):
    """
    Creates layer tree structure
    """
    #	def __init__(self, parent, id, pos, size, style):
    def __init__(self, parent,
                 id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.SUNKEN_BORDER,
                 ctstyle=CT.TR_HAS_BUTTONS | CT.TR_HAS_VARIABLE_ROW_HEIGHT |
                 CT.TR_HIDE_ROOT | CT.TR_ROW_LINES | CT.TR_FULL_ROW_HIGHLIGHT,
                 disp=None, log=None):
        CT.CustomTreeCtrl.__init__(self, parent, id, pos, size, style,ctstyle)

        # we need this only for GIS Manager, but not for e.g. mapdisp
        import menuform

        self.SetAutoLayout(True)
        self.SetGradientStyle(1)
        self.EnableSelectionGradient(True)
        self.SetFirstGradientColour(wx.Colour(150, 150, 150))

        self.Map = "" # instance of render.Map associated with display
        self.root = ""      # ID of layer tree root node
        self.node = 0       # index value for layers
        self.optpage = {}   # dictionary of notebook option pages for each map layer
        self.layer_selected = ""   # ID of currently selected layer
        self.layertype = {} # dictionary of layer types for each layer
        self.layerctrl = {} # dictionary of layers indexed by special wind controls (spinctrl and textctrl)
        self.saveitem = {} # dictionary to preserve layer attributes for drag and drop
        self.first = True # indicates if a layer is just added or not
        self.drag = False # flag to indicate a drag event is in process

        self.Map = disp.getRender()

        self.root = self.AddRoot("Map Layers")
        self.SetPyData(self.root, None)

        #create image list to use with layer tree
        il = wx.ImageList(16, 16, False)

        trgif = wx.Image(icons + r'/element-cell.gif', wx.BITMAP_TYPE_GIF)
        trgif.Rescale(16, 16)
        trgif = trgif.ConvertToBitmap()
        self.rast_icon = il.Add(trgif)
        trgif = wx.Image(icons + r'/element-vector.gif', wx.BITMAP_TYPE_GIF)
        trgif.Rescale(16, 16)
        trgif = trgif.ConvertToBitmap()
        self.vect_icon = il.Add(trgif)

        trgif = wx.Image(icons + r'/gui-cmd.gif', wx.BITMAP_TYPE_GIF)
        trgif.Rescale(16, 16)
        trgif = trgif.ConvertToBitmap()
        self.cmd_icon = il.Add(trgif)

        checksize = il.GetSize(0)
        checkbmp = il.GetBitmap(0)
        self.AssignImageList(il)

        # use when groups implemented
        # self.tree.SetItemImage(self.root, fldridx, wx.TreeItemIcon_Normal)
        # self.tree.SetItemImage(self.root, fldropenidx, wx.TreeItemIcon_Expanded)

        self.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.onExpandNode)
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.onCollapseNode)
        self.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.onActivateLayer)
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.onChangeSel)
        self.Bind(CT.EVT_TREE_ITEM_CHECKED, self.onLayerChecked)
        self.Bind(wx.EVT_TREE_DELETE_ITEM, self.onDeleteLayer)
        self.Bind(wx.EVT_TREE_BEGIN_DRAG, self.onBeginDrag)
        self.Bind(wx.EVT_TREE_END_DRAG, self.onEndDrag)

    def AddLayer(self, idx, type):

        if type == 'command':
            # generic command layer
            self.ctrl = wx.TextCtrl(self, id=wx.ID_ANY, value='',
                               pos=wx.DefaultPosition, size=(250,40),
                               style=wx.TE_MULTILINE|wx.TE_WORDWRAP)
            self.ctrl.Bind(wx.EVT_TEXT_ENTER, self.onCmdChanged)
            self.ctrl.Bind(wx.EVT_TEXT, self.onCmdChanged)
        else:
            # all other layers
            self.ctrl = wx.SpinCtrl(self, id=wx.ID_ANY, value="", pos=(30, 50),
                                    style=wx.SP_ARROW_KEYS)
            self.ctrl.SetRange(1,100)
            self.ctrl.SetValue(100)
            self.ctrl.Bind(wx.EVT_SPINCTRL, self.onOpacity)
            self.ctrl.Bind(wx.EVT_TEXT, self.onOpacity)

        if self.layer_selected and self.layer_selected != self.GetRootItem():
            layer = self.InsertItem(self.root, self.GetPrevSibling(self.layer_selected),
                                '', ct_type=1, wnd=self.ctrl )
        else:
            layer = self.PrependItem(self.root, '', ct_type=1, wnd=self.ctrl)

        self.SelectItem(layer)

        # add to layertype and layerctrl dictionaries
        self.layertype[layer] = type
        self.layerctrl[self.ctrl] = layer

        # add a data object to hold the layer's command (does not apply to generic command layers)
        self.SetPyData(layer, None)

        #layer is initially unchecked as inactive
        self.CheckItem(layer, checked=False)

        # add layer to layers list in render.Map
        self.Map.addLayer(item=layer, command='', l_active=False,
                                      l_hidden=False, l_opacity=1, l_render=True)

        # add text and icons for each layer type
        if type == 'raster':
            self.SetItemImage(layer, self.rast_icon)
            self.SetItemText(layer, 'raster (double click to set properties)')
            # launch the properties dialog
            menuform.GUI().parseCommand('d.rast', gmpath, completed=(self.getOptData,layer), parentframe=self)
        elif type == 'vector':
            self.SetItemImage(layer, self.vect_icon)
            self.SetItemText(layer, 'vector (double click to set properties)')
            # launch the properties dialog
            menuform.GUI().parseCommand('d.vect', gmpath, completed=(self.getOptData,layer), parentframe=self)
        elif type == 'command':
            self.SetItemImage(layer, self.cmd_icon)

    def onActivateLayer(self, event):
        global gmpath
        layer = event.GetItem()
        self.layer_selected = layer
        completed = ''

       # When double clicked or first added, open options dialog
        if self.layertype[layer] == 'raster':
            menuform.GUI().parseCommand('d.rast', gmpath, completed=(self.getOptData,layer), parentframe=self)
        elif self.layertype[layer] == 'vector':
            menuform.GUI().parseCommand('d.vect', gmpath, completed=(self.getOptData,layer), parentframe=self)

    def onDeleteLayer(self, event):
        layer = event.GetItem()
        self.layertype.pop(layer)

        # delete layer in render.Map
        self.Map.delLayer(item=layer)

    def onLayerChecked(self, event):
        layer = event.GetItem()
#        checked = self.IsItemChecked(layer)

        if self.drag == False:
            # change parameters for item in layers list in render.Map
            self.changeLayer(layer)

    def onCmdChanged(self, event):
        layer = self.layerctrl[event.GetEventObject()]
        cmd = event.GetString()

        if self.drag == False:
            # change parameters for item in layers list in render.Map
            self.changeLayer(layer)
        event.Skip()

    def onOpacity(self, event):
        if 'SpinCtrl' in str(event.GetEventObject()):
            layer = self.layerctrl[event.GetEventObject()]
        else:
            layer = self.layerctrl[event.GetEventObject().GetParent()]
#        opac = event.GetValue()

        if self.drag == False:
            # change parameters for item in layers list in render.Map
            self.changeLayer(layer)

    def onChangeSel(self, event):
        layer = event.GetItem()
        self.layer_selected = layer

    def onCollapseNode(self, event):
        print 'group collapsed'
        event.Skip()

    def onExpandNode(self, event):
        self.layer_selected = event.GetItem()
        print 'group expanded'
        event.Skip()

    def onBeginDrag(self, event):
        """ Drag and drop of single tree nodes
        """
        self.drag = True
        # node cannot be a parent
        if self.GetChildrenCount(event.GetItem()) == 0:
            event.Allow()

            # save everthing associated with item to drag
            self.dragItem = event.GetItem()
            self.saveitem['type'] = self.layertype[self.dragItem]
            self.saveitem['check'] = self.IsItemChecked(self.dragItem)
            self.saveitem['image'] = self.GetItemImage(self.dragItem, 0)
            self.saveitem['text'] = self.GetItemText(self.dragItem)
            self.saveitem['wind'] = self.GetItemWindow(self.dragItem)
            self.saveitem['windval'] = self.GetItemWindow(self.dragItem).GetValue()
            self.saveitem['data'] = self.GetPyData(self.dragItem)
        else:
            print ("Cant drag a node that has children")

    def onEndDrag(self, event):
        """
        Insert copy of layer in new position and
        delete original at old position
        """

        #If we dropped somewhere that isn't on top of an item, ignore the event
        if not event.GetItem():
            return

        # Make sure this memeber exists.
        try:
            old = self.dragItem
        except:
            return

        # Get the other IDs that are involved
        afteritem = event.GetItem()
        parent = self.GetItemParent(afteritem)
        if not parent:
            return

        # recreate old layer at new position
        if self.layertype[old] == 'command':
            self.dragctrl = wx.TextCtrl(self, id=wx.ID_ANY, value='',
                               pos=wx.DefaultPosition, size=(250,40),
                               style=wx.TE_MULTILINE|wx.TE_WORDWRAP)
            self.dragctrl.Bind(wx.EVT_TEXT_ENTER, self.onCmdChanged)
        else:
            self.dragctrl = wx.SpinCtrl(self, id=wx.ID_ANY, value="", pos=(30, 50),
                                    style=wx.SP_ARROW_KEYS)
            self.dragctrl.SetRange(1,100)
            self.dragctrl.SetValue(100)
            self.dragctrl.Bind(wx.EVT_SPINCTRL, self.onOpacity)

        new = self.InsertItem(parent, afteritem, text=self.saveitem['text'], \
                              ct_type=1, wnd=self.dragctrl, image=self.saveitem['image'], \
                              data=self.saveitem['data'])
        self.layertype[new] = self.saveitem['type']
        self.CheckItem(new, checked=self.saveitem['check'])
        self.GetItemWindow(new).SetValue(self.saveitem['windval'])

        # delete layer at original position
        self.Delete(old)

        # update layers list in render.Map
        if self.saveitem['type'] == 'command':
            self.Map.addLayer(item=new, command=self.saveitem['windval'], l_active=self.saveitem['check'],
                                      l_hidden=False, l_opacity=1, l_render=True)
        else:
            self.Map.addLayer(item=new, command=self.saveitem['data'], l_active=self.saveitem['check'],
                                      l_hidden=False, l_opacity=self.saveitem['windval'], l_render=True)

        self.reorderLayers()
        self.drag = False

    def getOptData(self, dcmd, layer):
        for item in dcmd.split(' '):
            if 'map=' in item:
                mapname = item.split('=')[1]

        # set layer text to map name
        self.SetItemText(layer, mapname)

        # add command to layer's data
        self.SetPyData(layer, dcmd)

        # check layer as active
        self.CheckItem(layer, checked=True)

        # change parameters for item in layers list in render.Map
        self.changeLayer(layer)

    def writeDCommand(self, dcmd):
        # echos d.* command to output console
        global goutput
        goutput.write(dcmd+"\n----------\n")

    def reorderLayers(self):
        """
        add commands from data associated with
        any valid and checked layers to layer list
        """
        # first empty the list of old layers
        self.Map.Clean()
        # make a list of visible layers
        treelayers = []
        vislayer = self.GetFirstVisibleItem()
        for item in range(0,self.GetCount()):
            if self.IsItemChecked(vislayer):
                treelayers.append(vislayer)
            if self.GetNextVisible(vislayer) == None:
                break
            else:
                vislayer = self.GetNextVisible(vislayer)
        treelayers.reverse()
        self.Map.reorderLayers(treelayers)

    def changeLayer(self, layer):
        if self.layertype[layer] == 'command':
            if self.GetItemWindow(layer).GetValue() != None:
                cmd = self.GetItemWindow(layer).GetValue()
                opac = 1.0
                chk = self.IsItemChecked(layer)
                hidden = not self.IsVisible(layer)
                self.Map.changeLayer(item=layer, command=cmd, l_active=chk,
                                  l_hidden=hidden, l_opacity=opac, l_render=True)
        else:
            if self.GetPyData(layer) != None:
                cmd = self.GetPyData(layer)
                opac = float(self.GetItemWindow(layer).GetValue())/100
                chk = self.IsItemChecked(layer)
                hidden = not self.IsVisible(layer)
                self.Map.changeLayer(item=layer, command=cmd, l_active=chk,
                                  l_hidden=hidden, l_opacity=opac, l_render=True)

class TreeCtrlComboPopup(wx.combo.ComboPopup):
    """
    Create a tree ComboBox for selecting maps and other GIS elements
    in accessible mapsets within the current location
    """

    # overridden ComboPopup methods

    def Init(self):
        self.value = None
        self.curitem = None


    def Create(self, parent):
        self.tree = wx.TreeCtrl(parent, style=wx.TR_HIDE_ROOT
                                |wx.TR_HAS_BUTTONS
                                |wx.TR_SINGLE
                                |wx.TR_LINES_AT_ROOT
                                |wx.SIMPLE_BORDER
                                |wx.TR_FULL_ROW_HIGHLIGHT)
        self.tree.Bind(wx.EVT_MOTION, self.OnMotion)
        self.tree.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)


    def GetControl(self):
        return self.tree


    def GetStringValue(self):
        if self.value:
            return self.tree.GetItemText(self.value)
        return ""


    def OnPopup(self):
        if self.value:
            self.tree.EnsureVisible(self.value)
            self.tree.SelectItem(self.value)


    def SetStringValue(self, value):
        # this assumes that item strings are unique...
        root = self.tree.GetRootItem()
        if not root:
            return
        found = self.FindItem(root, value)
        if found:
            self.value = found
            self.tree.SelectItem(found)


    def GetAdjustedSize(self, minWidth, prefHeight, maxHeight):
        return wx.Size(minWidth, min(200, maxHeight))


    def getElementList(self, element):
        """
        Get list of GIS elements in accessible mapsets and display as tree
        with all relevant elements displayed beneath each mapset branch
        """
        #set environmental variables
        gisdbase = os.popen('g.gisenv get=GISDBASE', "r").read().strip()
        location = os.popen('g.gisenv get=LOCATION_NAME', "r").read().strip()
        curr_mapset = os.popen('g.gisenv get=MAPSET', "r").read().strip()
        location_path = os.path.join (gisdbase,location)
        windfile = os.path.join (location_path,'PERMANENT','WIND')
        symbol_path = os.path.join (os.environ['GISBASE'],'etc','symbol')

        #valid location test if needed
        if windfile != None:
            pass

        #mapsets in current location
        mapsets = os.popen('g.mapsets -p', "r").read().lstrip().rstrip().split(' ')

        #Get directory tree nodes
        for dir in mapsets:
            if dir == curr_mapset:
                #TODO: make current mapset node expanded
                dir_node = self.AddItem('Mapset: '+dir)
                self.tree.SetItemTextColour(dir_node,wx.Colour(50,50,200))
                self.tree.Expand(dir_node)
                elem_list = os.listdir(os.path.join (location_path, dir, element))
                #TODO: sort list items?
                for elem in elem_list:
                    self.AddItem(elem, parent=dir_node)
            else:
                dir_node = self.AddItem('Mapset: '+dir)
                self.tree.SetItemTextColour(dir_node,wx.Colour(50,50,200))
                elem_list = os.listdir(os.path.join (location_path, dir, element))
                #TODO: sort list items?
                for elem in elem_list:
                    self.AddItem(elem+'@'+dir, parent=dir_node)

    # helpers

    def FindItem(self, parentItem, text):
        item, cookie = self.tree.GetFirstChild(parentItem)
        while item:
            if self.tree.GetItemText(item) == text:
                return item
            if self.tree.ItemHasChildren(item):
                item = self.FindItem(item, text)
            item, cookie = self.tree.GetNextChild(parentItem, cookie)
        return wx.TreeItemId();


    def AddItem(self, value, parent=None):
        if not parent:
            root = self.tree.GetRootItem()
            if not root:
                root = self.tree.AddRoot("<hidden root>")
            parent = root

        item = self.tree.AppendItem(parent, value)
        return item


    def OnMotion(self, evt):
        # have the selection follow the mouse, like in a real combobox
        item, flags = self.tree.HitTest(evt.GetPosition())
        if item and flags & wx.TREE_HITTEST_ONITEMLABEL:
            self.tree.SelectItem(item)
            self.curitem = item
        evt.Skip()


    def OnLeftDown(self, evt):
        # do the combobox selection
        item, flags = self.tree.HitTest(evt.GetPosition())
        if item and flags & wx.TREE_HITTEST_ONITEMLABEL:
            self.curitem = item
            self.value = item
            self.Dismiss()
        evt.Skip()



class GMConsole(wx.Panel):
    """
    Create and manage output console for commands entered on the
    GIS Manager command line.
    """
    def __init__(self, parent, id=-1,
                     pos=wx.DefaultPosition, size=wx.DefaultSize,
                     style=wx.TAB_TRAVERSAL|wx.FULL_REPAINT_ON_RESIZE):
        wx.Panel.__init__(self, parent, id, pos, size, style)
        #initialize variables

        self.cmd_output = ""
        self.console_command = ""
        self.console_clear = ""
        self.console_save = ""
        self.gcmdlst = [] #list of commands in bin and scripts

        #text control for command output
        self.cmd_output = wx.TextCtrl(self, -1, "",
                                                  style=wx.TE_MULTILINE|
                                                  wx.TE_READONLY|wx.HSCROLL)

        global goutput
        goutput = self.cmd_output
    	self.console_clear = wx.Button(self, -1, _("Clear"))
    	self.console_save = wx.Button(self, -1, _("Save"))

    	self.Bind(wx.EVT_BUTTON, self.clearHistory, self.console_clear)
    	self.Bind(wx.EVT_BUTTON, self.saveHistory, self.console_save)

		# output control layout
    	boxsizer1 = wx.BoxSizer(wx.VERTICAL)
    	gridsizer1 = wx.GridSizer(1, 2, 0, 0)
    	boxsizer1.Add(self.cmd_output, 1,
                              wx.EXPAND|wx.ADJUST_MINSIZE, 0)
    	gridsizer1.Add(self.console_clear, 0,
                               wx.ALIGN_CENTER_HORIZONTAL|wx.ADJUST_MINSIZE, 0)
    	gridsizer1.Add(self.console_save, 0,
                               wx.ALIGN_CENTER_HORIZONTAL|wx.ADJUST_MINSIZE, 0)

        boxsizer1.Add((0,5))
    	boxsizer1.Add(gridsizer1, 0, wx.EXPAND|wx.ALIGN_CENTRE_VERTICAL)
        boxsizer1.Add((0,5))
        boxsizer1.Fit(self)
    	boxsizer1.SetSizeHints(self)
    	self.SetAutoLayout(True)
    	self.SetSizer(boxsizer1)

    def getGRASSCmds(self):
		'''
        Create list of all available GRASS commands to use when
        parsing string from the command line
        '''
		gisbase = os.environ['GISBASE']
		self.gcmdlst = os.listdir(gisbase+r'/bin')
		self.gcmdlst.append(os.listdir(gisbase+r'/scripts'))
		return self.gcmdlst

    def runCmd(self, cmd):
    	"""
        Run in GUI or shell GRASS (or other) commands typed into
    	console command text widget, echo command to
    	output text widget, and send stdout output to output
    	text widget.

        TODO: Display commands (*.d) are captured and
    	processed separately by mapdisp.py. Display commands are
    	rendered in map display widget that currently has
    	the focus (as indicted by mdidx).
        """

    	gcmdlst = self.getGRASSCmds()
    	cmdlst = []
#    	cmd = self.console_command.GetLineText(0)
    	cmdlst = cmd.split(' ')
    	disp_idx = int(track.Track().GetDisp()[0])
    	curr_disp = track.Track().GetDisp()[1]

    	if len(cmdlst) == 1 and cmd in gcmdlst:
    		# Send GRASS command without arguments to GUI command interface
    		# except display commands (they are handled differently)
            global gmpath
            if cmd[0:2] == "d.":
                if cmd == 'd.rast':
                    layertype = 'raster'
                elif cmd == 'd.vect':
                    layertype = 'vector'
                else:
                    print 'Command type not yet implemented'
                    return

                if disp_idx != None:
                    # get layer tree for active display
                    layertree = track.Track().GetCtrls(disp_idx, 2)
                    # add layer
                    layertree.AddLayer(disp_idx, layertype)

            else:
                menuform.GUI().parseCommand(cmd, gmpath)
                self.cmd_output.write(cmdlst[0] +
                                                          "\n----------\n")

    	elif cmd[0:2] == "d." and len(cmdlst) > 1 and cmdlst[0] in gcmdlst:
            """
            Send GRASS display command(s)with arguments
            to the display processor and echo to command output console.
            Accepts a list of d.* commands separated by commas.
            Display with focus receives display command(s).
            """
            self.cmd_output.write(cmd +
                                                  "\n----------\n")
            dcmds = cmd.split(',')
            curr_disp.addMapsToList(type='command', map=dcmds, mset=None)
            curr_disp.ReDrawCommand()

    	else:
    		# Send any other command to the shell. Send output to
    		# console output window.
            try:
                retcode = subprocess.call(cmd, shell=True)

                if retcode < 0:
    				print >> sys.stderr, "Child was terminated by signal", retcode
                elif retcode > 0:
    				print >> sys.stderr, "Child returned", retcode
            except OSError, e:
    			print >> sys.stderr, "Execution failed:", e

            self.cmd_output.write(cmd+"\n----------\n")
            #FIXME - why is PIPE not recognized?
#            self.out = subprocess.Popen(cmd, shell=True, stdout=PIPE).stdout
            self.out = os.popen(cmd, "r").read()
            self.cmd_output.write(self.out+"\n")

    def clearHistory(self, event):
		self.cmd_output.Clear()

    def saveHistory(self, event):
        self.history = self.cmd_output.GetStringSelection()
        if self.history == "":
            self.cmd_output.SetSelection(-1,-1)
            self.history = self.cmd_output.GetStringSelection()

        #Use a standard dialog for this
        wildcard = "Text file (*.txt)|*.txt"
        dlg = wx.FileDialog(
            self, message="Save file as ...", defaultDir=os.getcwd(),
            defaultFile="grass_cmd_history.txt", wildcard=wildcard, style=wx.SAVE|wx.FD_OVERWRITE_PROMPT
            )

        # Show the dialog and retrieve the user response. If it is the OK response,
        # process the data.
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()

        output = open(path,"w")
        output.write(self.history)
        output.close()
        dlg.Destroy()

def GetTempfile( pref=None):
    """
    Creates GRASS temporary file using defined prefix.

    Returns:
        Path to file name (string) or None
    """

    tempfile = os.popen("g.tempfile pid=%d" %
                        os.getpid()).readlines()[0].strip()

    if not tempfile:
        return None
    else:
        path,file = os.path.split(tempfile)
        if pref:
            file = "%s%s" % (pref,file)
        return os.path.join(path,file)
