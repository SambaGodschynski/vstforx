#!/usr/bin/python
import unittest
import imp
import os
import xml.etree.ElementTree as tx

_TEST_REP = """
<plugin-repository name="testrep" author="samba godschynski" install-loc="testrep" 
	tags="Windows, Mac, VST, i386, x64">
  <vendor name="smartelectronix" url="www.smartelectronix.com" install-loc="smart_electronix">
    <plugin name="mda plugins" install-loc="mda">
      <file plattform="windows" arch="i386" format="vst">http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip</file>
      <file plattform="mac" arch="i386, x64, ppc" format="vst">http://mda.smartelectronix.com/vst/mda_vst_ub.zip</file>
    </plugin>
  </vendor>
</plugin-repository>
"""

class TestPlugBox(unittest.TestCase):
    testClass = None

    def setUp(self):
        rep = imp.load_source('rep_sync', '../plugBox.py')
        self.testClass = rep.RepSync()
        f = open("testfile.xml", "w")
        f.write(_TEST_REP)
        f.close()
                    
    def tearDown(self):
        os.remove("testfile.xml")
        
    def test_download(self):
        tc = self.testClass
        tc.url="testfile.xml"
        tc.sync()
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in tc.to_download)
        tc.to_download={}
        
        tc.sync(plattform="windows")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" not in tc.to_download)
        tc.to_download={}
        
        tc.sync(plattform="windows, mac")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in tc.to_download)
        tc.to_download={}

        tc.sync(plattform="windows, mac", arch="x64")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" not in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in tc.to_download)
        tc.to_download={}

        tc.sync(plattform="windows", arch="x64")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" not in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" not in tc.to_download)
        tc.to_download={}
    
    def test_add_remove(self):
        import xml.etree.ElementTree as xt
        tc = self.testClass
        tc.url="testfile.xml"
        tc.add_vendor(name = "heimat")
        tc.add_plugin("heimat", name="ppi")
        tc.add_file("TAL-Vocoder-2.zip", "http://www.vstforx.de/tal.zip", "heimat", "ppi")
        tc.add_file("Saro.dll", "http://www.vstforx.de/saro.dll", "heimat", "ppi")
        soll = """plugin-repository author="samba godschynski" install-loc="testrep" name="testrep" tags="Windows, Mac, VST, i386, x64">
  <vendor install-loc="smart_electronix" name="smartelectronix" url="www.smartelectronix.com">
    <plugin install-loc="mda" name="mda plugins">
      <file arch="i386" format="vst" plattform="windows">http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip</file>
      <file arch="i386, x64, ppc" format="vst" plattform="mac">http://mda.smartelectronix.com/vst/mda_vst_ub.zip</file>
    </plugin>
  </vendor>
<vendor name="heimat"><plugin name="ppi"><file>http://www.vstforx.de/tal.zip<binary md5="4a95d635143e50cc0466e140a6f05f02">TAL-Vocoder-2.dll</binary></file><file>http://www.vstforx.de/saro.dll<binary md5="ac69ed900c6c60df6591454d13e2fe58">Saro.dll</binary></file></plugin></vendor></plugin-repository>"""
        self.assertTrue(soll, tx.tostring(tc.root))
        tc.remove_file("http://www.vstforx.de/saro.dll", "heimat", "ppi")
        tc.remove_file("http://www.vstforx.de/tal.zip", "heimat", "ppi")
        
        soll="""<plugin-repository author="samba godschynski" install-loc="testrep" name="testrep" tags="Windows, Mac, VST, i386, x64">
  <vendor install-loc="smart_electronix" name="smartelectronix" url="www.smartelectronix.com">
    <plugin install-loc="mda" name="mda plugins">
      <file arch="i386" format="vst" plattform="windows">http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip</file>
      <file arch="i386, x64, ppc" format="vst" plattform="mac">http://mda.smartelectronix.com/vst/mda_vst_ub.zip</file>
    </plugin>
  </vendor>
<vendor name="heimat"><plugin name="ppi" /></vendor></plugin-repository>"""
        self.assertTrue(soll, tx.tostring(tc.root))

        tc.remove_plugin("ppi", "heimat")
                
        soll="""<plugin-repository author="samba godschynski" install-loc="testrep" name="testrep" tags="Windows, Mac, VST, i386, x64">
  <vendor install-loc="smart_electronix" name="smartelectronix" url="www.smartelectronix.com">
    <plugin install-loc="mda" name="mda plugins">
      <file arch="i386" format="vst" plattform="windows">http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip</file>
      <file arch="i386, x64, ppc" format="vst" plattform="mac">http://mda.smartelectronix.com/vst/mda_vst_ub.zip</file>
    </plugin>
  </vendor>
<vendor name="heimat"></vendor></plugin-repository>"""
        self.assertTrue(soll, tx.tostring(tc.root))
        
        tc.remove_vendor("heimat")
                
        soll="""<plugin-repository author="samba godschynski" install-loc="testrep" name="testrep" tags="Windows, Mac, VST, i386, x64">
  <vendor install-loc="smart_electronix" name="smartelectronix" url="www.smartelectronix.com">
    <plugin install-loc="mda" name="mda plugins">
      <file arch="i386" format="vst" plattform="windows">http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip</file>
      <file arch="i386, x64, ppc" format="vst" plattform="mac">http://mda.smartelectronix.com/vst/mda_vst_ub.zip</file>
    </plugin>
  </vendor>
</plugin-repository>"""
        self.assertTrue(soll, tx.tostring(tc.root))

        #print xt.tostring(tc.root)

if __name__ == '__main__':
    unittest.main()
