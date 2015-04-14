-----------------------------------------------------------------
-- VSTForx database init script                                --
-- While the init script will be executed when the editor      --
-- is opening, this module will be executed on startup only    --
-- a VSTForx.Lua documentation can be found under:             --
--      http://api.vstforx.de                                  --
-- author: Samba Godschynski                                   --
-----------------------------------------------------------------

function addFolder(name, path)
   q = string.format("INSERT OR IGNORE INTO folders(name, parentFolderID, location, doNotDelete) \
VALUES ('%s', 1, '%s', 1)", name, path)
   frx.queryDB(q)
end

function addLuaPlugin(name, path, folder_path)
   q = string.format("INSERT OR IGNORE INTO plugins(location,name,folderID,access,uidX, plugin_type, doNotDelete) \
VALUES ('%s', '%s', %s, 1, %s, 5, 1)"
   ,path
   ,name
   ,"(SELECT id FROM folders WHERE location='"..folder_path.."')"
   ,"0"
   )
   frx.queryDB(q)
end

--add internal lua plugins manually (override scanning)
PATH="scripts/lua_plugins"
addFolder("Lua Plugins", PATH)
addLuaPlugin("LFO", PATH.."/LFO.lua", PATH)
addLuaPlugin("SimpleDelay", PATH.."/SimpleDelay.lua", PATH)
addLuaPlugin("SimpleGain", PATH.."/SimpleGain.lua", PATH)
addLuaPlugin("ABMorpher", PATH.."/ABMorpher.lua", PATH)
