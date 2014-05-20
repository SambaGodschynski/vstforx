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

function ___getConnectionOpNames()
   return {"Inverse Operator", 
	   "Offset Operator", 
	   "EXP Operator",
	   "LOG Operator",
	   "Multiplier Operator"
   }
end

function ___clone(o)
   id = o:getTypeId()
   if (string.match(id, ".*%.Plugin")) then
      id=string.format("%s('%s')", id, o:getPluginLocation())
   end
   if (string.match(id, ".*Input.*")) then
      id=string.format("%s(%i, 2)", id, o:getNumInputs()) 
   end
   if (string.match(id, ".*Output.*")) then
      id=string.format("%s(2, %i)", id, o:getNumOutputs()) 
   end
   new=frx.view:add(id)
   op = o:getParameters()
   np = new:getParameters()
   if #op ~= #np then
      return
   end
   for i=1,#np,1 do
      np[i]:setValue( op[i]:getValue() )
   end
end


_ENV.helper={
   clone=___clone,
   addPlugins=___addPlugins,
   addPlugin=___addPlugin,
   removeAll=___clearView,
   getConnectables=___getConnectables,
   getProcessorTypeNames=___getProcessorTypeNames,
   getConnectionOpNames=___getConnectionOpNames
}

_ENV.viewHelper=_ENV.helper -- deprecated use helper instead
