-----------------------------------------------------------------
-- VSTForx database init script                                --
-- While the init script will be executed when the editor      --
-- is opening, this module will be executed on startup only    --
-- a VSTForx.Lua documantation can be found under:             --
--      api.vstforx.de                                         --
-- author: Samba Godschynski                                   --
-----------------------------------------------------------------

plugs = {
   "LFO.lua",
}

function addFolder(name, path)
   q = string.format("INSERT OR IGNORE INTO folders(name, parentFolderID, location, doNotDelete) \
VALUES ('%s', 1, '%s', 1)", name, path)
   frx.queryDB(q)
end

function addLuaPlugin(name, path, folder_path)
   q = string.format("INSERT OR IGNORE INTO plugins(location,name,folderID,access,uid, plugin_type, doNotDelete) \
VALUES ('%s', '%s', %s, 1, %s, 5, 1)"
   ,path
   ,name
   ,"(SELECT id FROM folders WHERE location='"..folder_path.."')"
   ,"0"
   )
   frx.queryDB(q)
end

--add internal lua plugins (if not exist)
PATH="../../vstforx_next/src/".."./scripts/lua_plugins".."_testfield"
addFolder("Lua Plugins", PATH)
addLuaPlugin("Volume", PATH.."/volume.lua", PATH)
