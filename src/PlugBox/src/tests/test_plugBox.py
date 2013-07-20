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
      <file plattform="windows" arch="i386" format="vst" filename="mda_vst_fx_win.zip">http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip</file>
      <file plattform="mac" arch="i386, x64, ppc" format="vst" filename="mda_vst_ub.zip">http://mda.smartelectronix.com/vst/mda_vst_ub.zip</file>
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
        tc._load_to_download()
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in tc.to_download)
        tc.to_download={}
        
        tc._load_to_download(plattform="windows")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" not in tc.to_download)
        tc.to_download={}
        
        tc._load_to_download(plattform="windows, mac")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in tc.to_download)
        tc.to_download={}

        tc._load_to_download(plattform="windows, mac", arch="x64")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" not in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in tc.to_download)
        tc.to_download={}

        tc._load_to_download(plattform="windows", arch="x64")
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
        soll = """<plugin-repository author="samba godschynski" install-loc="testrep" name="testrep" tags="Windows, Mac, VST, i386, x64">\n  <vendor install-loc="smart_electronix" name="smartelectronix" url="www.smartelectronix.com">\n    <plugin install-loc="mda" name="mda plugins">\n      <file arch="i386" filename="mda_vst_fx_win.zip" format="vst" plattform="windows">http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip</file>\n      <file arch="i386, x64, ppc" filename="mda_vst_ub.zip" format="vst" plattform="mac">http://mda.smartelectronix.com/vst/mda_vst_ub.zip</file>\n    </plugin>\n  </vendor>\n<vendor name="heimat"><plugin name="ppi"><file size="1033035">http://www.vstforx.de/tal.zip<binary md5="4a95d635143e50cc0466e140a6f05f02">TAL-Vocoder-2.dll</binary></file><file size="70656">http://www.vstforx.de/saro.dll<binary md5="ac69ed900c6c60df6591454d13e2fe58">Saro.dll</binary></file></plugin></vendor></plugin-repository>"""
        self.assertEqual(soll, tx.tostring(tc.root))
        tc.remove_file("http://www.vstforx.de/saro.dll", "heimat", "ppi")
        tc.remove_file("http://www.vstforx.de/tal.zip", "heimat", "ppi")
        
        soll="""<plugin-repository author="samba godschynski" install-loc="testrep" name="testrep" tags="Windows, Mac, VST, i386, x64">\n  <vendor install-loc="smart_electronix" name="smartelectronix" url="www.smartelectronix.com">\n    <plugin install-loc="mda" name="mda plugins">\n      <file arch="i386" filename="mda_vst_fx_win.zip" format="vst" plattform="windows">http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip</file>\n      <file arch="i386, x64, ppc" filename="mda_vst_ub.zip" format="vst" plattform="mac">http://mda.smartelectronix.com/vst/mda_vst_ub.zip</file>\n    </plugin>\n  </vendor>\n<vendor name="heimat"><plugin name="ppi" /></vendor></plugin-repository>"""
        self.assertEqual(soll, tx.tostring(tc.root))

        tc.remove_plugin("ppi", "heimat")
                
        soll="""<plugin-repository author="samba godschynski" install-loc="testrep" name="testrep" tags="Windows, Mac, VST, i386, x64">\n  <vendor install-loc="smart_electronix" name="smartelectronix" url="www.smartelectronix.com">\n    <plugin install-loc="mda" name="mda plugins">\n      <file arch="i386" filename="mda_vst_fx_win.zip" format="vst" plattform="windows">http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip</file>\n      <file arch="i386, x64, ppc" filename="mda_vst_ub.zip" format="vst" plattform="mac">http://mda.smartelectronix.com/vst/mda_vst_ub.zip</file>\n    </plugin>\n  </vendor>\n<vendor name="heimat" /></plugin-repository>"""
        self.assertEqual(soll, tx.tostring(tc.root))
        
        tc.remove_vendor("heimat")
                
        soll="""<plugin-repository author="samba godschynski" install-loc="testrep" name="testrep" tags="Windows, Mac, VST, i386, x64">\n  <vendor install-loc="smart_electronix" name="smartelectronix" url="www.smartelectronix.com">\n    <plugin install-loc="mda" name="mda plugins">\n      <file arch="i386" filename="mda_vst_fx_win.zip" format="vst" plattform="windows">http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip</file>\n      <file arch="i386, x64, ppc" filename="mda_vst_ub.zip" format="vst" plattform="mac">http://mda.smartelectronix.com/vst/mda_vst_ub.zip</file>\n    </plugin>\n  </vendor>\n</plugin-repository>"""
        self.assertEqual(soll, tx.tostring(tc.root))

        #print xt.tostring(tc.root)

if __name__ == '__main__':
    unittest.main()
