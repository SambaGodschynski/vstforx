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
   if evName~="object added" then return end
   setObjectMenu(evObj) -- set a custom menu, will override the origin
end

function initViewObjects()
   -- since all changes on view objects, caused
   -- by this script, are lost after the editor 
   -- is closed, we need to redo this.
   o = frx.view:getObjects()
   for i=1, #o, 1 do
      setObjectMenu(o[i])
   end
end

-- global statements will be executed while editor is opening
-- so we can use them for setting things up:
frx.view:addViewListener("onViewEvent") 
initViewObjects()

 -- set custom main menu: (see vstforx-menusetup.lua)
initViewportMenu(4)
frx.view:setMenu(menus.main)
