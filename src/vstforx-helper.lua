--- find plugins by name and add them to view
function addPlugins(name)
   r = frx.queryDB("SELECT location FROM plugins WHERE name LIKE '%" .. name .. "%';")
   if #r == 0 then
      print("no plugin found")
      return
   end
   for i=1,#r,1 do
      loc = r[i]['location']
      frx.view:add("unknown-plugin.Plugin('" .. loc .. "')")
   end
end

--- find plugins by name and add first match to view
function addPlugin(name)
   r = frx.queryDB("SELECT location FROM plugins WHERE name LIKE '%" .. name .. "%';")
   if #r == 0 then
      print("no plugin found")
      return
   end
   for i=1,#r,1 do
      loc = r[i]['location']
      frx.view:add("unknown-plugin.Plugin('" .. loc .. "')")
      break
   end
end
