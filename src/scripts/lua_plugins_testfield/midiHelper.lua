midi = {
   MessageTypes = {
      UNKNOWN=0,
      NOTE_ON=0x9,
      NOTE_OFF=0x8,
      POLY_AFTERTOUCH=0xA,
      CC=0xB,
      PC=0xC,
      MONO_AFTERTOUCH=0xD,
      PITCH_BEND=0xE,
      SYSEX=0xF,
      -- status to string
      [0] = "Unknown",
      [0x9] = "Note On",
      [0x8] = "Note Off",
      [0xA] = "Poly Aftertouch",
      [0xB] = "Control Change",
      [0xC] = "Program Change",
      [0xD] = "Mono Aftertouch",
      [0xE] = "Pitch Bend",
      [0xF] = "Sysex"
   }
   ,
   -- returns a string representation of a type
   typeToString = function(type)
      return midi.MessageTypes[type]
   end
   ,
   -- returns true when given byte is a status byte
   isStatus=function(status)
      if status==nil then
	 return false
      end
      return status>=0x80 and status<=0xF0
   end
   ,
   -- returns a message type determined by its status byte  
   getMessageType = function(status)
      if not midi.isStatus(status) then
	 return midi.UNKNOWN
      end
      return bit32.rshift(status, 4)
   end
   ,
   -- returns the bytesize of a midi message determined by its status type
   -- returns 0 if type unknown or nil if sysex
   getMessageSize = function(mtype)
      if mtype == midi.MessageTypes.UNKNOWN then
	 return 0
      end
      if mtype == midi.MessageTypes.SYSEX then
	 return nil
      end
      if mtype == midi.MessageTypes.MONO_AFTERTOUCH 
      or mtype == midi.MessageTypes.PC then
	 return 2
      end
      return 3
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
	 local byte = self.__data[self.__bytePos]
	 self.__bytePos = self.__bytePos + 1
	 return byte
      end
      ,
      -- returns the current pitch if exists or nil
      pitch = function(self)
	 if self.type ~= midi.MessageTypes.NOTE_ON and
	    self.type ~= midi.MessageTypes.NOTE_OFF and
	    self.type ~= midi.MessageTypes.POLY_AFTERTOUCH
	 then
	    return nil
	 end
	 return self.data[1]
      end
      ,
      -- returns the current velocity if exists or nil
      velocity = function(self)
	 if self.type ~= midi.MessageTypes.NOTE_ON and
	    self.type ~= midi.MessageTypes.NOTE_OFF and
	    self.type ~= midi.MessageTypes.POLY_AFTERTOUCH and
	    self.type ~= midi.MessageTypes.POLY_AFTERTOUCH
	 then
	    return nil
	 end
	 return self.data[2]
      end
      ,
      -- returns the current pitchbend if exists or nil
      pitchBend = function(self)
	 if self.type ~= midi.MessageTypes.PITCH_BEND then
	    return nil
	 end
	 return bit32.lshift(self.data[2], 7) + self.data[1]
      end
      ,
      -- goto next event and return true or false if no further event
      next = function(self)
	 local byte = self:__nextByte()
	 while byte~=nil do
	    if midi.isStatus(byte) then
	       if self.type == midi.MessageTypes.SYSEX then
		  self.data={}
		  self.channel = nil
		  self.size=0
		  byte = self:__nextByte()
		  while byte~=0xF7 and byte~=nil do
		     self.size = self.size + 1
		     table.insert(self.data, byte)
		     byte = self:__nextByte()
		  end
		  return true
	       end
	       self.type = midi.getMessageType(byte)
	       self.size = midi.getMessageSize(self.type)
	       self.channel = bit32.band(byte, 0x0F)
	       if self.size == 2 then
		  self.data = {self:__nextByte()}
		  return true
	       end
	       if self.size == 3 then
		  self.data = {self:__nextByte(), self:__nextByte()}
		  return true
	       end
	    end
	    byte = self:__nextByte()
	 end
	 return false
      end
   }
}

