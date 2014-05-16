-----------------------------------------------------------------
-- main VSTForx init script                                    --
-- will be (re)loaded after every "open editor" event          --
-- a VSTForx.Lua documantation can be found under:             --
--      xxx.xxx.xx                                             --
-- author: Samba Godschynski                                   --
-----------------------------------------------------------------
require "vstforx-helper"
require "vstforx-menusetup"

function onViewEvent(evName, evObj)
   if evName=="requesting context menu" then 
      setObjectMenu(evObj) -- setting a custom menu, will override the origin
      return
   end
   if evName=="on saving view state" then
      saveMenuData()
   end
end

-- global statements will be executed while editor is opening
-- so we can use them for setting things up:
frx.view:addViewListener("onViewEvent") 

 -- set custom main menu: (see vstforx-menusetup.lua)
initViewportMenu(4)
frx.view:setMenu(menus.main)
