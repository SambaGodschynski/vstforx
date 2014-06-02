midiHelper = {
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
}
