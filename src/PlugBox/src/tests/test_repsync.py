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

class TestRepSync(unittest.TestCase):
    testClass = None

    def setUp(self):
        rep = imp.load_source('rep_sync', '../rep_sync.py')
        self.testClass = rep.RepSync()
        f = open("testfile.xml", "w")
        f.write(_TEST_REP)
        f.close()
                    
    def tearDown(self):
        os.remove("testfile.xml")
        
    def test_download(self):
        tc = self.testClass
        tc.url="testfile.xml"
        tc.load_rep()
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in tc.to_download)
        tc.to_download=[]
        
        tc.load_rep(plattform="windows")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" not in tc.to_download)
        tc.to_download=[]
        
        tc.load_rep(plattform="windows, mac")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in tc.to_download)
        tc.to_download=[]

        tc.load_rep(plattform="windows, mac", arch="x64")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" not in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in tc.to_download)
        tc.to_download=[]

        tc.load_rep(plattform="windows", arch="x64")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" not in tc.to_download)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" not in tc.to_download)
        tc.to_download=[]
        
    def test_add_zip(self):
        tc = self.testClass
        tc.url="testfile.xml"
        tc.load_rep()
        
        tc.add_file("TAL-Vocoder-2.zip", 
                    "http://kunz.corrupt.ch/downloads/plugins/TAL-Vocoder-2.zip", 
                    {'name':'Togu Audio Line', 'url':'http://kunz.corrupt.ch/'},
                    {'name':'TAL-Vocoder'})
        
        tc.add_file("Saro.dll", 
                    "http://www.smartelectronix.com/~antti/Saro.dll", 
                    {'name':'smartelectronix'},
                    {'name':'Saro'})
        
        print tx.tostring(tc.root)
        

if __name__ == '__main__':
    unittest.main()
