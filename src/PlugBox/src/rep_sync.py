"""
downloads and install plugins automatically according to 
repository input file. 
"""

import urllib
import urllib2
import os
import re
import time
import xml.etree.ElementTree as xt
import HTMLParser
import zipfile as zip
import hashlib

HEADER = { 'User-Agent' : 'user' }
_CACHING_ON = True 
_CACHE_PATH = ".tmp"


def _hashfile(f, hasher, blocksize=65536):
    buf = f.read(blocksize)
    while len(buf) > 0:
        hasher.update(buf)
        buf = f.read(blocksize)
    return hasher.digest().encode('hex_codec')


def _download(url, post_txt="downloading"):
    """
    downloads url with statusbar output: post_txt filename: [n%]
    """
    file_name = url.split('/')[-1]
    u = urllib2.urlopen(url)
    f = open(file_name, 'wb')
    meta = u.info()
    file_size = int(meta.getheaders("Content-Length")[0])
    file_size_dl = 0
    block_sz = 8192
    while True:
        buffer = u.read(block_sz)
        if not buffer:
            break
        file_size_dl += len(buffer)
        f.write(buffer)
        status = r"%s %s: %10d [%3.2f%%]" % (post_txt, file_name, file_size_dl, file_size_dl * 100. / file_size)
        status = status + chr(8)*(len(status)+1)
        print status,
    f.close()

def _log(str):
    print str

def _norm_str(x):
    x = x.strip()
    x = re.sub(r"<.*?>", " ", x)
    x = re.sub(r"[^\w]", "-", x)
    return x

def _norm_url(url, **values):
    url = url.lower().replace("http://","")
    url = _norm_str(url)
    for x,y in values.items():
        url = url + '-' + re.sub("[^\w]", "-", y)
    return url

def _req_GET(_url, **values):
    if not os.path.exists(_CACHE_PATH):
        os.makedirs(_CACHE_PATH)
    cache_path = _CACHE_PATH + '/' + "tmp_"+_norm_url(_url, **values)
    if _CACHING_ON == True and os.path.exists(cache_path):
        _log("** CACHING ON: using local file %s **" % cache_path)
        f = open(cache_path, 'rb')
        res = f.read();
        f.close()
        return res
        
    if len(values) != 0:
        data = urllib.urlencode(values)
        url=_url+'?'+data
    req = urllib2.Request(url, None, HEADER)
    try:
        response = urllib2.urlopen(req)
        res = response.read()
    except Exception, ex:
        print "req: %s failed!: %s" % (url, ex) 
        return ""
    time.sleep(1)
    if _CACHING_ON == True:
        f = open(cache_path, 'wb')
        f.write(res)
        f.close()
    return res

def _save_data(path, data):
    f = open(path, 'wb')
    f.write(data)
    f.close()

def _is_localfile(url):
    if url.find("http://") < 0:
        return True
    return False

class RepSync:
    url="http://public.plugbox.vstforx.de"
    dst_path="."
    install_loc=""
    __filter=None
        
    def __init__(self):
        pass

    def __unpack_commasep(self, x):
        x = x.lower()
        x = x.split(",")
        x = map(lambda v: v.strip(), x)
        return x
    
    def __filter_attr_values(self, filter_values, element_values):
         f_v = self.__unpack_commasep(filter_values)
         e_v = self.__unpack_commasep(element_values)
         for x in f_v:
             if x in e_v:
                 return True
         return False

    def __pass_filter(self, el):
        """
        return attrib( x | y | z ) && attrib ( x | y | z )
        """
        res = True
        if self.__filter==None:
            return True
        for key in self.__filter:
            if el.attrib.has_key(key):
                res = res & self.__filter_attr_values(self.__filter[key], el.attrib[key])
               
        return res

    def __download(self, url):
        pass

    def __process_pluginpack(self, el):
        self.install_loc = "%s/%s" % (self.install_loc, el.attrib["install-loc"])
        for x in el.iter("file"):
            if not self.__pass_filter(x):
                continue
            url = HTMLParser.HTMLParser().unescape(x.text)
            self.__download(url)
            
    def __process_vendor(self, el):
        print "  fetching '%s':" % el.attrib['name']
        self.install_loc = "%s/%s" % (self.install_loc, el.attrib["install-loc"])
        for x in el.iter('plugin-package'):
            self.__process_pluginpack(x)
            
    def __process_tree(self, root):
        if root.tag != "plugin-repository":
            raise StandardError("invalid repository file")
        print "fetching '%s' by '%s'" % (root.attrib["name"], root.attrib["author"])
        print "tags: %s" % root.attrib['tags']
        self.install_loc = "%s/%s" % (self.dst_path, root.attrib["install-loc"])
        for x in root.iter('vendor'):
            self.__process_vendor(x)
        
    def __load_repository(self, url):
        if _is_localfile(url):
            f = open(url, "r")
            data = f.read()
            f.close()
        else:
            data = _req_GET(url)
        self.root = xt.XML(data)
        
        
    def sync(self, **_filter):
        """
        downloads and install plugins
          filter:
            os, arch, format
        """
        self.__filter = _filter
        self.__load_repository(self.url)
        self.__process_tree(self.root)
    
    def __get_zip_content(self, path):
        res=[]
        with zip.ZipFile(path, 'r') as z:
            for x in z.namelist():
                f = z.open(x, "r")
                md5 = _hashfile(f, hashlib.md5())
                f.close()
                res.append((x, md5))
        return res

    def __unpack_file(self, path):
        pass
        #if os.path.splitext(path)[1] == ".zip":
            #self.__unzip_file(path)
            
    def __get_archive_content(self, path):
        """return list with filenames and md5 values"""
        if os.path.splitext(path)[1] == ".zip":
            return self.__get_zip_content(path)


    def add_vendor(self, parent, **vendorinfo):
        """adds a new vendor and return new element 
        or if exists returns existing vendor element.
        if vendorinfo None or without useful information
        parent will be returned."""
        if vendorinfo == None:
            return parent
        if not vendorinfo.has_key('name'):
            return parent
        for x in parent.iter("vendor"):
            if x.attrib['name'] == vendorinfo['name']:
                return x
        node = xt.SubElement(parent, "vendor", vendorinfo)
        return node

    def add_package(self, parent, **pkginf):
        """adds a new package and return new element 
        or if exists returns existing package element.
        if package None or without useful information
        the parent will be returned."""
        if pkginf == None:
            return parent
        if not pkginf.has_key('name'):
            return parent
        for x in parent.iter("plugin-package"):
            if x.attrib['name'] == pkginf['name']:
                return x
        node = xt.SubElement(parent, "plugin-package", pkginf)
        return node

    def add_file(self, path, url, vendorinfo, pluginpackage, **attr):
        if not _is_localfile(self.url):
            raise StandardError("repository url has to be a local file.")
        self.__load_repository(self.url)
        a_cnt = self.__get_archive_content(path)

        v = self.add_vendor(self.root, **vendorinfo)
        v = self.add_package(v, **pluginpackage)
        n = xt.SubElement(v, "file", attr)
        n.text=url

if __name__ == "__main__":
    rs = RepSync()
    rs.url="../testrep.xml"
    rs.add_file("pb.zip", "http://www.vstforx.de/pb.zip", {'name': 'smartelectronix'}, {'name':'sonstwas'}, arch="i386")
    print xt.tostring(rs.root)
    
