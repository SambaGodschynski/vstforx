--- find plugins by name and add them to view
function ___addPlugins(name)
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
function ___addPlugin(name)
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

function ___clearView()
   o = frx.view:getObjects()
   for k,v in pairs(o) do
      frx.view:remove(v)
   end
end

function ___getConnectables()
   res=frx.view:getByType("*Input")
   for k,v in pairs(frx.view:getByType("*Output")) do
      table.insert(res, v)
   end
   for k,v in pairs(frx.view:getByType("*Knob")) do
      table.insert(res, v)
   end
   for k,v in pairs(frx.view:getByType("*Entry")) do
      table.insert(res, v)
   end
   for k,v in pairs(frx.view:getByType("*Exit")) do
      table.insert(res, v)
   end
   return res
end

function ___getProcessorTypeNames()
   return {
      "internal.Volume",
      "internal.ADSRTrigger",
      "internal.InputStep",
      "internal.OutputStep",
      "internal.InputSwitch",
      "internal.OutputSwitch",
      "internal.Pan",
      "internal.PeakTracker",
      "internal.MidiProcessor"
   }
end

_ENV.viewHelper={
   addPlugins=___addPlugins,
   addPlugin=___addPlugin,
   removeAll=___clearView,
   getConnectables=___getConnectables,
   getProcessorTypeNames=___getProcessorTypeNames
}
