-----------------------------------------------------------------
-- VSTForx menu setup script                                   --
-- While the init script will be executed when the editor      --
-- is opening, this module will be executed on startup only    --
-- a VSTForx.Lua documantation can be found under:             --
--      xxx.xxx.xx                                             --
-- author: Samba Godschynski                                   --
-----------------------------------------------------------------
require "vstforx-helper"
-- main menu def
menus = {
   main={
      {name="VSTForx " .. frx.getVersionString() },
      {name="Modify Scene...", action="onOpenBrowser()"},
      {name="Open Setup Dialog", action="frx.openSetup()"},
      {name="execute command...", action="onExecute()"},
      {name="About...", action="frx.openAbout()"},
      {name="external"},
      {name="www.vstforx.de", action="frx.openUrl('http://www.vstforx.de')"}
   },   
}

function setObjectMenu(obj)
   if obj==nil then
      return
   end
   objType = obj:getTypeId()
   objMenu = {}
   if string.match(objType, "internal%..*")~=nil then     
      -- internal.* (e.g. internal.Volume)
      table.insert(objMenu, {name="show details...", 
			     action=string.format("onOpenBrowser('Main Scene/Plugins/%s')", obj:getName())})
   elseif string.match(objType, ".*%.Plugin")~=nil then
      -- *.Plugin (e.g. vst2x.Plugin)
      table.insert(objMenu, {name="show details...", 
			     action=string.format("onOpenBrowser('Main Scene/Plugins/%s')", obj:getName())})
   elseif string.match(objType, "parameter%..*")~=nil then
      -- parameter.* (e.g. parameter.StdKnob)
      table.insert(objMenu, {name="show details...", 
			     action=string.format("onOpenBrowser('Main Scene/Parameter/%s')", obj:getName())})
   elseif string.match(objType, "connection%..*")~=nil then
      -- connection.* (e.g. connection.IO)
      if string.match(objType, "connection.Parameter") then
	 table.insert(objMenu, {name="show details...", 
				action=string.format("onOpenBrowser('Main Scene/Parameter/Parameter Connections/%s')", obj:getName())})
      end
   else 
      -- no menu type
      return
   end
   table.insert(objMenu, 1, {name=obj:getName()})
   table.insert(objMenu, {name="remove", action="onRemove()"})
   obj:setMenu(objMenu)
end

function onRemove()
   o=frx.view:getContextObject()
   if o~=nil then
      frx.view:remove(o)
   end
end

function onExecute()
   s=frx.showInputTextDlg('Command','frx.openAbout()')
   print(s)
   --loadstring(s)()
end

function onSave()
   print(frx.getGraphDelay())
   _ENV.state = frx.serializePlugin()
end

function onLoad()	
   if _ENV.state == nil then
      return
   end
   frx.deserializePlugin(_ENV.state)
end

function onLoadFile()
end

function onSaveFile()
   path = frx.selectFile("")
   frx.messageBox(path)
end

function onOpenBrowser(x)
   if(x==nil) then
      frx.openSceneBrowser(frx.getLastSceneBrowserSelection())
      return
   end
   print(x)
   frx.openSceneBrowser(x)
end
