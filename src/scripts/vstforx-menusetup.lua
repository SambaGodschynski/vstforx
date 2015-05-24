-----------------------------------------------------------------
-- VSTForx menu setup script                                   --
-- While the init script will be executed when the editor      --
-- is opening, this module will be executed on startup only    --
-- a VSTForx.Lua documentation can be found under:             --
--      http://api.vstforx.de                                  --
-- author: Samba Godschynski                                   --
-----------------------------------------------------------------
require "vstforx-helper"

viewportMenu={}
viewports={}
currentViewport=1

-- main menu def
menus = {
   main={
      {name="VSTForx " .. frx.getVersionString() },
      {name="Modify Scene...", action="frx.runOnUIThread('onOpenBrowser()')"},
      {name="Open Setup Dialog", action="frx.runOnUIThread('frx.openSetup()')"},
      {name="Help", {
	  {name="About VSTForx", action="print(frx.getBuildHash()) frx.runOnUIThread('frx.openAbout()')"},
	  {name="Report A Bug", action="frx.openUrl('http://www.vstforx.de/index.php/2014-01-12-14-49-45/report-a-bug')"},
	  {name="Known Issues", action="frx.openUrl('http://issues.vstforx.de/roadmap_page.php?version_id=27')"}
      }},
      {name="Auxiliaries"},
      {name="Find Plugins...", action="frx.runOnUIThread('onAddPlugin()')"},
      {name="Viewports", viewportMenu},
      {name="State"},
      {name="Load...", action="frx.runOnUIThread('load()')"},
      {name="Save...", action="frx.runOnUIThread('save()')"},
      --{name="Lua"},
      --{name="Execute Command...", action="onExecute()"}
      
   },   
}

-- Select Plugins dlg, to be considered as singleton->creating a new one will close
-- the previous instance
SelectPluginsDlg = {
   wnd = nil
   ,
   show = function(self, plugins)
      self:create()
      for k,v in ipairs(plugins) do
	 self.wnd:add(tostring(v['location']))
      end
      self.wnd:setSize(640,400)
      self.wnd:setTitle(string.format("Found %i plugins for '%s', please select:", #plugins, plugins['name']))
      self.wnd:open()
   end
   ,
   create = function(self)
      if self.wnd~=nil then
	 self.wnd:close()
      end
      self.wnd = frx.view:createListWindow()
      self.wnd:addButton("Add", "SelectPluginsDlg:onAdd()")
      self.wnd:addButton("Abbort", "SelectPluginsDlg:onAbbort()")
      self.wnd:addCloseListener("SelectPluginsDlg:onClose()")
   end
   ,
   onAbbort = function(self)
      self.wnd:close()
   end
   ,
   onAdd = function(self)
      loc=self.wnd:getSelection()
      x="unknown-plugin.Plugin('" .. loc .. "')"
      if not pcall( frx.view.add, frx.view, x) then
	 frx.messageBox("adding "..loc.." failed")
      end
   end
   ,
   onClose = function(self)
      self.wnd=nil
      collectgarbage() -- force immediate object destroying
   end
}

function onAddPlugin()
   name=frx.showInputTextDlg("Plugin Name","")
   if #name==0 then
      return
   end
   r = frx.queryDB("SELECT location FROM plugins WHERE name LIKE '%" .. name .. "%'\
 OR location LIKE '%" .. name .. "%';")
   if #r == 0 then
      frx.messageBox("no plugin "..name.." found")
      return 
   end
   if #r == 1 then
      loc = r[1]['location']
      frx.view:add("unknown-plugin.Plugin('" .. loc .. "')")
      return
   end
   r['name'] = name
   SelectPluginsDlg:show(r)
end

function getViewportName(index, active)
   res="Viewport "..index
   if active then
      res=res.." (X)"
   end
   return res
end

function saveMenuData()
   viewports[currentViewport] = {frx.view:getLocation()}
   for i=1, #viewports, 1 do
      frx.setPersistData(string.format("viewport.loc%i",i), viewports[i])
   end
   frx.setPersistData("viewport.index", {currentViewport})
end

function initViewportMenu(num)
   for i=1,#viewportMenu,1 do
      -- remove old entries, we can't just do
      -- x={} because it creates a new reference which is unknown to
      -- the main menu table
      table.remove(viewportMenu)
   end
   _ENV.viewports={}
   index=frx.getPersistData("viewport.index")
   if #index>0 then
      currentViewport=tonumber(index[1])
   end
   for i=1,num,1 do
      table.insert(viewportMenu, 
		   {name=getViewportName(i,i==currentViewport), 
		    action=string.format("toViewport(%i)", i)})
      loc = frx.getPersistData(string.format("viewport.loc%i",i))
      if #loc==0 then
	 loc={frx.view:getLocation()}
      end
      table.insert(viewports,loc)
   end
   moveViewTo(table.unpack(viewports[currentViewport]))
end

function moveViewTo(x,y)
   frx.view:setLocation(x,y)
end

function toViewport(newIndex)
   --save old loc
   oldIndex = currentViewport
   viewports[oldIndex] = {frx.view:getLocation()}
   -- move to new
   px,py = table.unpack(viewports[newIndex])
   moveViewTo(px,py)
   --update menu
   viewportMenu[oldIndex].name=getViewportName(oldIndex, false)
   viewportMenu[newIndex].name=getViewportName(newIndex, true)
   currentViewport=newIndex 
   frx.view:setMenu(menus.main)
end

function save()
   if _ENV.f==nil then
      _ENV.f=""
   end
   path=frx.showSaveFileDlg(_ENV.f)
   if path == nil or #path==0 then
      return
   end
   _ENV.f = path
   data=frx.serializePlugin()
   fh=io.open(_ENV.f,"w")
   fh:write(data)
   fh:close()
end

function load()
   if _ENV.f==nil then
      _ENV.f=""
   end
   path=frx.showSelectFileDlg(_ENV.f)
   if path == nil or #path==0 then
      return
   end
   _ENV.f = path
   fh=io.open(_ENV.f,"r")
   data=fh:read("*a")
   fh:close()
   frx.deserializePlugin(data)
end

function addOperator(x)
   o=frx.view:getContextObject()
   if o==nil then
      return
   end
   o:addOperator(x)
   -- reset menu
   addParameterConnectionMenu(o)
end

function removeOperator(i)
   o=frx.view:getContextObject()
   if o==nil then
      return
   end
   o:removeOperatorAt(i)
   -- reset menu
   addParameterConnectionMenu(o)
end


function addParameterConnectionMenu(obj)
   --get operators which can be added
   ops=helper.getConnectionOpNames()
   addEntries={}
   -- create submenu table
   for i=1,#ops,1 do
      table.insert(addEntries,{name=ops[i], action=string.format("addOperator('%s')",ops[i])})
   end
   --get operators which can be removed
   ops=obj:getOperatorNames() --op names on connection
   -- create submenu table
   removeEntries={}
   for i=1,#ops,1 do
      table.insert(removeEntries,{name=ops[i], action=string.format("removeOperator(%i)",i)})
   end
   
   objMenu ={ {name=obj:getName()},
	      {name="remove", action="onRemove()"},
	      {name="show details...", 
	       action=string.format("onOpenBrowser('Main Scene/Parameter/Parameter Connections/%s')", obj:getViewId())},
	      {name="add operator", addEntries}
	    }
   if #removeEntries>0 then
      table.insert(objMenu, 5, {name="remove operator", removeEntries})
   end
   obj:setMenu(objMenu)
end

function addInput()
   o=frx.view:getContextObject()
   o:addInput(true)
end

function addOutput()
   o=frx.view:getContextObject()
   o:addOutput(true)
end

function setObjectMenu(obj)
   if obj==nil then
      return
   end
   objType = obj:getTypeId()
   objMenu = {}
   if string.match(objType, "internal%..*")~=nil then     
      -- internal.* (e.g. internal.Volume)
      table.insert(objMenu, {name="show details...", 
			     action=string.format("onOpenBrowser('Main Scene/Plugins/%s')", obj:getViewId())})
      table.insert(objMenu, {name="clone", action="onClone()"})
      table.insert(objMenu, {name="assign A/B morpher...", action="onAssignAB()"})
      if string.match(objType, ".*Input.*") then
	 -- Input Step/Switch
	 table.insert(objMenu, {name="add input", 
				action="addInput()"})
      elseif string.match(objType, ".*Output.*") then
	 -- Output Step/Switch
	 table.insert(objMenu, {name="add output", 
				action="addOutput()"})
      end
   elseif string.match(objType, ".*%.RemoteChReceiver")~=nil then     
      -- interprocess.* (e.g. interprocess.RemoteChReceiver)
      table.insert(objMenu, {name="show details...", 
			     action=string.format("onOpenBrowser('Main Scene/Plugins/%s')", obj:getViewId())})
   elseif string.match(objType, ".*%.Plugin")~=nil then
      -- *.Plugin (e.g. vst2x.Plugin)
      table.insert(objMenu, {name="show details...", 
			     action=string.format("onOpenBrowser('Main Scene/Plugins/%s')", obj:getViewId())})
      table.insert(objMenu, {name="clone", action="onClone()"})
      table.insert(objMenu, {name="parameter assistant...", action="onCollectParameter()"})
      table.insert(objMenu, {name="open/close editor...", action="onOpenCloseEditor()"})
      table.insert(objMenu, {name="assign A/B morpher...", action="onAssignAB()"})
   elseif string.match(objType, "parameter%..*")~=nil then
      -- parameter.* (e.g. parameter.StdKnob)
      table.insert(objMenu, {name="set value...", action="onSetKnobValue()"})
      table.insert(objMenu, {name="show details...", 
			     action=string.format("onOpenBrowser('Main Scene/Parameter/%s')", obj:getViewId())})
   elseif string.match(objType, "connection%..*")~=nil then
      -- connection.* (e.g. connection.IO)
      if string.match(objType, "connection.Parameter") then
	 addParameterConnectionMenu(obj)
	 return
      end
   else 
      -- no menu type
      return
   end
   table.insert(objMenu, 1, {name=obj:getName()})
   table.insert(objMenu, 2, {name="remove", action="onRemove()"})
   table.insert(objMenu, 3, {name="rename...", action="onRename()"})
   obj:setMenu(objMenu)
end

function onSetKnobValue()
   o=frx.view:getContextObject()
   x=o:getValue()
   nx=frx.showInputTextDlg("Set Value for "..o:getName(), x)
   nx=tonumber(nx)
   if nx==nil or x==nx then
      return
   end
   nx=math.min(nx, 1)
   nx=math.max(nx, 0)
   o:setValue(nx)
end

function onClone()
   o=frx.view:getContextObject()
   helper.clone(o)
end

function onOpenCloseEditor()
   o=frx.view:getContextObject()
   o:openCloseEditor()
end

function onRename()
   o=frx.view:getContextObject()
   name=o:getName()
   name=frx.showInputTextDlg("rename "..name, name)
   if #name==0 then
      return
   end
   o:setName(name)
   setObjectMenu(o) -- reset menu
end

function onRemove()
   o=frx.view:getContextObject()
   if o~=nil then
      frx.view:remove(o)
   end
end

function onExecute()
   if _ENV.lastCommand == nil then
       _ENV.lastCommand="frx.openAbout()"
   end
   _ENV.lastCommand=frx.showInputTextDlg('Command', _ENV.lastCommand)
   frx.exec(_ENV.lastCommand)
end

function onOpenBrowser(x)
   if(x==nil) then
      frx.openSceneBrowser(frx.getLastSceneBrowserSelection())
      return
   end
   frx.openSceneBrowser(x)
end

-- Collect parameter dlg, waiting for plugins parameter 
-- changed and adding them to a list
CollectParameterDlg = {
   wnd = nil,
   nbInstances = 0,
   plug = nil,
   param = nil,
   res = nil,
   show = function(self)
      if self.plug==nil then
	 return
      end
      self.param = self.plug:getParameters()
      for k,v in pairs(self.param) do
	 v:addListener(string.format("CollectParameterDlg.callback('%s', 'onParamChanged', %i)", self.__id, k))
      end
      self.wnd:setSize(400, 550)
      self.wnd:setTitle(string.format("Change parameter in %s's editor", self.plug:getName()))
      self.wnd:open()
   end
   ,
   new = function(self, plug)
      local o = {}
      setmetatable(o, self)
      self.__index = self
      o.wnd = frx.view:createListWindow()
      self.nbInstances=self.nbInstances+1
      o.__id = string.format("__parDlg%i", self.nbInstances) 
      o.plug = plug
      o.res = {}
      _ENV[o.__id] = o --we cannot use instances for callbacks, so we use this
                       -- global table for accessing
      o.wnd:addButton("OK", string.format("CollectParameterDlg.callback('%s', 'onOk')", o.__id))
      o.wnd:addButton("Remove", string.format("CollectParameterDlg.callback('%s', 'onRemove')", o.__id))
      o.wnd:addButton("Abbort", string.format("CollectParameterDlg.callback('%s', 'onAbbort')", o.__id))
      o.wnd:addCloseListener(string.format("CollectParameterDlg.callback('%s', 'onClose')", o.__id))
      return o
   end
   ,
   callback = function(id, f, arg1)
      --delegate callback
      if _ENV[id] == nil then --check if exists
	 return
      end
      if arg1==nil then
	 _ENV[id][f](_ENV[id]);
      else
	 _ENV[id][f](_ENV[id], arg1);
      end
   end
   ,
   onAbbort = function(self)
      self.wnd:close()
   end
   ,
   onOk = function(self, x)
      for k,v in ipairs(self.res) do
	 frx.view:add(v)
      end
      self.wnd:close()
   end
   ,
   onRemove = function(self)
      i=self.wnd:getSelectedIndex()
      self.wnd:removeElementAt(i)
      table.remove(self.res, i)
   end
   ,
   onClose = function(self)
      -- delete global holder
      if self.__id==nil then
	 return
      end
      _ENV[self.__id].res = nil
      _ENV[self.__id].wnd = nil
      _ENV[self.__id].param = nil
      _ENV[self.__id].plug = nil
      _ENV[self.__id] = nil
      collectgarbage() -- force immediate object destroying
   end
   ,
   onParamChanged = function (self, pid) 
      if self.res[tostring(pid)] ~= nil then
	 return
      end
      local p = self.param[pid]
      self.res[tostring(pid)]=1 -- insert pid as string for checking if already added
      table.insert(self.res, p)
      self.wnd:add(p:getName())
   end
}

function onCollectParameter()
   local p = frx.view:getContextObject()
   if p==nil then
      return
   end
   dlg = CollectParameterDlg:new(p)
   dlg:show()
end

function onAssignAB()
   local p = frx.view:getContextObject()
   local num = p:getNumParameters()
   print (num)
   if num==0 then
      return
   end
   if num>50 then
      frx.messageBox(string.format("this plugin has %i parameter, for performace reasons its not recommended to use all parameter for A/B.", num))
      return
   end
   local url=string.format("lua.Plugin('scripts/lua_plugins/ABMorpher.lua////numParams=%i')", num)
   local x,y = p:getLocation()
   local params = p:getParameters()
   local ab = frx.view:add(url)
   local marginPrPa=120 --margin processor<->parameter
   local marginPrPr=230 --margin parameter<->parameter
   local offsetY = y-num/2*50
   --frx.view:addToSelection(ab)
   ab:setLocation(x-marginPrPa*2-marginPrPr, y)
   local p2 = ab:getParameters()
   local i=2
   local tmp={}
   local abx, aby = ab:getLocation()
   p2[1]:setLocation(abx-50, aby) -- A/B knob
   for k,v in pairs(params) do
      if v:getName()~="editor_X" and
         v:getName()~="editor_Y" then
	 local a= p2[i]
	 local b = v
	 frx.view:add(a)
	 frx.view:add(b)
	 frx.view:connect(a, b)
	 table.insert(tmp, a)
	 table.insert(tmp, b)
	 --set knob location
	 a:setLocation(x-marginPrPa-marginPrPr, offsetY+(i*50))
	 b:setLocation(x-marginPrPa, offsetY+(i*50))
	 --increment i
	 i=i+1
      end
   end
   frx.view:clearSelection()
   for k,v in pairs(tmp) do
      frx.view:addToSelection(v)
   end
end
