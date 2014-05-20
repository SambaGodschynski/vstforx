-----------------------------------------------------------------
-- VSTForx menu setup script                                   --
-- While the init script will be executed when the editor      --
-- is opening, this module will be executed on startup only    --
-- a VSTForx.Lua documantation can be found under:             --
--      api.vstforx.de                                         --
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
      {name="Modify Scene...", action="onOpenBrowser()"},
      {name="Open Setup Dialog", action="frx.openSetup()"},
      {name="Help", {
	  {name="About VSTForx", action="frx.openAbout()"},
	  {name="Report A Bug", action="frx.openUrl('http://www.vstforx.de/index.php/2014-01-12-14-49-45/report-a-bug')"},
	  {name="Known Issues", action="frx.openUrl('http://issues.vstforx.de/roadmap_page.php?version_id=27')"}
      }},
      {name="Auxiliaries"},
      {name="Find Plugin...", action="onAddPlugin()"},
      {name="Viewports", viewportMenu},
      {name="State"},
      {name="Load...", action="load()"},
      {name="Save...", action="save()"},
      --{name="Lua"},
      --{name="Execute Command...", action="onExecute()"}
      
   },   
}

function onAddPlugin()
   name=frx.showInputTextDlg("Plugin Name","")
   if #name==0 then
      return
   end
   r = frx.queryDB("SELECT location FROM plugins WHERE name LIKE '%" .. name .. "%';")
   if #r == 0 then
      frx.messageBox("no plugin "..name.." found")
      return 
   end
   if #r > 1 then
      if not frx.showYesNoDlg("found " .. #r .. " plugins '".. name .. "'. Add them all?") then
	 return
      end
   end
   for i=1,#r,1 do
      loc = r[i]['location']
      frx.view:add("unknown-plugin.Plugin('" .. loc .. "')")
   end
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
      table.insert(objMenu, {name="open/close editor...", action="onOpenCloseEditor()"})
   elseif string.match(objType, "parameter%..*")~=nil then
      -- parameter.* (e.g. parameter.StdKnob)
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

