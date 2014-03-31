-----------------------------------------------------------------
-- main VSTForx init script                                    --
-- will be (re)loaded after every "open editor" event          --
-- a VSTForx.Lua documantation can be found under:             --
--      xxx.xxx.xx                                             --
-- author: Samba Godschynski                                   --
-----------------------------------------------------------------

-- main menu def
gpCustomMenus= { 
   {name="VSTForx " .. frx.getVersionString() },
   {name="Modify Scene...", action="onOpenBrowser()"},
   {name="Open Setup Dialog", action="frx.openSetup()"},
   {name="execute command...", action="onExecute()"},
   {name="About...", action="frx.openAbout()"},
   {Name="external"},
   {name="www.vstforx.de", action="frx.openUrl('http://www.vstforx.de')"},
}

function onExecute()
   s=frx.showInputTextDlg('Command','')
   loadstring(s)()
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

function onAddDCTester()
   frx.addProcessor('internal-private.DCTester')
end

function onShowGraphDelay()
   frx.messageBox('delay: '..tostring(frx.getGraphDelay()))
end

function onOpenBrowser()
   frx.openSceneBrowser(frx.getLastSceneBrowserSelection())
end

