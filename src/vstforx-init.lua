-----------------------------------------------------------------
-- main VSTForx init script                                    --
-- will be (re)loaded after every "open editor" event          --
-- a VSTForx.Lua documantation can be found under:             --
--      xxx.xxx.xx                                             --
-- author: Samba Godschynski                                   --
-----------------------------------------------------------------
require "vstforx-helper"
require "vstforx-menusetup"

-- global lines will be executed at startup
-- so we can use them for setting up:
frx.view:setMenu(menus.main)
frx.view:addViewListener("onViewEvent")



function onViewEvent(evName, evObj)
   if evName~="object added" then return end
   setObjectMenu(evObj)
end
