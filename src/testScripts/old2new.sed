s/addProcessor(\([a-zA-Z0-9_,.\"-]*\))/view:add(\1)/
s/frx.getProcessorInputs(\([a-zA-Z0-9_,.\"-]*\))/\1:getInputs()/
s/frx.getProcessorOutputs(\([a-zA-Z0-9_,.\"-]*\))/\1:getOutputs()/
s/frx.getEntryExit()/frx.view:getEntry(), frx.view:getExit()/
s/connectComponents/view:connect/
s/frx.clearView/viewHelper.removeAll/
s/frx.addProcessorInput(\([a-zA-Z0-9_,.\"-]*\))/\1:addInput()/
s/frx.addProcessorOutput(\([a-zA-Z0-9_,.\"-]*\))/\1:addOutput()/
s/frx.getViewNodes/viewHelper.getConnectables/
s/frx.getProcessorTypes/viewHelper.getProcessorTypeNames/
s/frx.getComponentParameter(\([a-zA-Z0-9_,.\"-]*\))/\1:getParameters()/
s/frx.setParameterValue(\([a-zA-Z0-9_,.\"-]*\), \([a-zA-Z0-9_,.\"-]*\))/\1:setValue(\2)/