midi = {
   UNKNOWN_STATUS=0,
   UNKNOWN_CHANNEL=0,
   NOTE_ON=0x90,
   NOTE_OFF=0x80,
   isStatus=function(byte)
      -- return true if byte is status byte
      return byte>=0x80 and byte<0xFF 
   end
   , getMidiEvents=function(this, data)
      -- return table: { {status, channel, {value1, ...}} }
      res={}
      this.lastEvent=nil
      for i=1, #data, 1 do
	 v=data[i]   
	 if this.isStatus(v) then
	    if this.lastEvent~=nil then
	       --finish previous event
	       table.insert(res, this.lastEvent)
	    end
	    status=bit32.band(0xF0, v)
	    channel=bit32.band(0x0F, v)
	    this.lastEvent={status, channel, {}}
	 else
	    --add data byte to this.lastEvent
	    if this.lastEvent~=nil then
	       table.insert(this.lastEvent[3], v)
	    end
	 end
      end
      if this.lastEvent~=nil then
	 --add last event
	 table.insert(res, this.lastEvent)
	 end
      return res
   end
   ,
   Iterator = {
      new = function (self, message) 
	 local o = {}
	 setmetatable(o, self)
	 self.__index = self
	 o.__msg = message
	 o.__nbEv = 1
	 o.__bytePos = 1
	 o.__data = {}
	 return o
      end
      ,
      __nextEvent = function(self)
	 if self.__nbEv >= #self.__msg + 1 then
	    return false
	 end
	 self.__data = self.__msg[self.__nbEv].data
	 self.__nbEv = self.__nbEv + 1
	 self.__bytePos = 1
	 return true
      end
      ,
      -- get next byte or nil if current data sequence is on end
      __nextByte = function(self)
	 if (self.__bytePos >= #self.__data+1) then
	    if self:__nextEvent() == false then
	       return nil
	    end
	 end
	 byte = self.__data[self.__bytePos]
	 self.__bytePos = self.__bytePos + 1
	 return byte
      end
      ,
      -- goto next event and return true or false if no further event
      next = function(self)
	 byte = self:__nextByte()
	 while byte~=nil do
	    print(string.format("0x%x", byte))
	    byte = self:__nextByte()
	 end
      end
   }
}

