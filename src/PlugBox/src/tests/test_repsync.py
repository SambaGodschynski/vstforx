import unittest
import imp
import os

_TEST_REP = """
<plugin-repository name="testrep" author="samba godschynski" install-loc="testrep" 
	tags="Windows, Mac, VST, i386, x64">
  <vendor name="smartelectronix" url="www.smartelectronix.com" install-loc="smart_electronix">
    <plugin-package name="mda plugins" install-loc="mda">
      <file plattform="windows" arch="i386" format="vst">http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip</file>
      <file plattform="mac" arch="i386, x64, ppc" format="vst">http://mda.smartelectronix.com/vst/mda_vst_ub.zip</file>
    </plugin-package>
  </vendor>
</plugin-repository>
"""

_last_downloads = []

def test_download(url):
    _last_downloads.append(url)
    pass

class TestRepSync(unittest.TestCase):
    testClass = None

    def setUp(self):
        rep = imp.load_source('rep_sync', '../rep_sync.py')
        self.testClass = rep.RepSync()
        self.testClass._RepSync__download = test_download
        f = open("testfile.xml", "w")
        f.write(_TEST_REP)
        f.close()
        global _last_downloads
        _last_downloads = []
    
    def tearDown(self):
        os.remove("testfile.xml")
        
    def test_download(self):
        global _last_downloads
        tc = self.testClass
        tc.url="testfile.xml"
        tc.sync()
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in _last_downloads)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in _last_downloads)
        _last_downloads=[]
        
        tc.sync(plattform="windows")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in _last_downloads)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" not in _last_downloads)
        _last_downloads=[]
        
        tc.sync(plattform="windows, mac")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" in _last_downloads)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in _last_downloads)
        _last_downloads=[]

        tc.sync(plattform="windows, mac", arch="x64")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" not in _last_downloads)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" in _last_downloads)
        _last_downloads=[]

        tc.sync(plattform="windows", arch="x64")
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_fx_win.zip" not in _last_downloads)
        self.assertTrue("http://mda.smartelectronix.com/vst/mda_vst_ub.zip" not in _last_downloads)
        _last_downloads=[]
        

if __name__ == '__main__':
    unittest.main()
