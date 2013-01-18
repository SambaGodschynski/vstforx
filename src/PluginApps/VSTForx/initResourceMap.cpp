/*
 * initResourceMap.cpp
 *
 *  Created on: Fri Jan 18 12:29:18 2013
 *      Author: createResources.py
 */


namespace frx { namespace createResourcesPy {
/**
 * RegisterCallbackFunction
 */
typedef void (RegisterResourceFunc*) (const std::string &path, long id);

/**
 * maps filename -> resourceID
 */
void initResourceMap(RegisterResourceFunc* registerF) {
	registerF("images/plugin.png", 100);
	registerF("images/remoteChannel.png", 110);
	registerF("images/e.png", 120);
	registerF("images/disco-floor.png", 130);
	registerF("images/midiReceiver.png", 140);
	registerF("images/volume_node.png", 150);
	registerF("images/folder.png", 160);
	registerF("images/adsr_trigger.png", 170);
	registerF("images/processor.png", 180);
	registerF("images/vst-big-icon.png", 190);
	registerF("images/step_adapter.png", 200);
	registerF("images/channel_split.png", 210);
	registerF("images/knob.png", 220);
	registerF("images/bkgrey.png", 230);
	registerF("images/peak_tracker.png", 240);
	registerF("images/script01.png", 250);
	registerF("images/script01_valid.png", 260);
	registerF("images/warning_icon.png", 270);
	registerF("images/plus.png", 280);
	registerF("images/iplug.png", 290);
	registerF("images/oSwitch.png", 300);
	registerF("images/exit.svg", 310);
	registerF("images/entry.svg", 320);
	registerF("images/script01_failed.png", 330);
	registerF("images/inf_icon.png", 340);
	registerF("images/vstPlugNode.png", 350);
	registerF("images/Blank-icon.png", 360);

}

}} // namespace
