frx.openPlugin()
frx.openEditor()

function clean()
   for k,v in pairs(frx.view:getObjects()) do
      frx.view:remove(v)
   end
   assert(#frx.view:getObjects() == 2)
end

function testEq()
   a=frx.view:add("internal.Volume")
   assert(a~=nil)
   l=frx.view:getObjects()
   assert(#l>0)
   res=false
   for i=1,#l,1 do -- at least one object has to match
      b=l[i]
      if a==b then
	 res=true
	 break
      end
   end
   assert(res)
   b=frx.view:add("internal.Volume")
   assert(a~=b)
end

function testParameter()
   v = frx.view:add("internal.Volume")
   -- remove Knob
   knob = frx.view:getByType("parameter.StdKnob")[1]
   assert(knob:getValue()==1)
   knob:setValue(0.5)
   assert(knob:getValue()==0.5)
   frx.view:remove(knob)
   assert(#frx.view:getObjects() == 5)
   --(re)add knob by parameter
   p = v:getParameters()[1]
   assert(p:getValue()==0.5)
   p:setValue(1)
   assert(p:getValue()==1)
   frx.view:add(p)
   assert(#frx.view:getObjects() == 6)
end

function testMoving()
   v = frx.view:add("internal.InputStep")
   x, y = v:getLocation()
   v:setLocation(x+100, y+120)
   x2, y2 = v:getLocation()
   assert(x2==x+100 and y2==y+120)
end

testEq()
clean()
testParameter()
clean()
testMoving()
