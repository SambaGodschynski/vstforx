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
   common={
      {name="remove", action="onRemove()"},
      {name="find in browser...", action="frx.openAbout()"}
   }
}
--- appends b's table entries to table a
function append(a,b)
   for i=1,#b,1 do
      table.insert(a, b[i])
   end
end

function setObjectMenu(obj)
   if obj==nil then
      return
   end
   objType = obj:getTypeId()
   if string.match(objType, "internal\\..*")~=nil       -- internal.* (e.g. internal.Volume)
      and string.match(objType, ".*\\.Plugin")~=nil     -- *.Plugin (e.g. vst2x.Plugin)
      and string.match(objType, "parameter\\..*")~=nil  -- parameter.* (e.g. parameter.StdKnob)
      and string.match(objType, "connection\\..*")~=nil -- connection.* (e.g. connection.IO)
   then
      -- no menu type
      return
   end
   objMenu = {{name=obj:getName()}}
   append(objMenu, menus.common)
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

function onOpenBrowser()
   frx.openSceneBrowser(frx.getLastSceneBrowserSelection())
end

