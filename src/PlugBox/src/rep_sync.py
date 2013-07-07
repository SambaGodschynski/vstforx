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


def _filter_attr(x):
    """removes entries which are not str"""
    l=[]
    a = x.copy()
    for x in a:
        if not isinstance(a[x], str):
            l.append(x)
    for x in l:
        a.pop(x)
    return a

def _unpack_commasep(x):
    x = x.lower()
    x = x.split(",")
    x = map(lambda v: v.strip(), x)
    return x

def _is_plugfile(f):
    ext = os.path.splitext(f)[1]
    return ext==".dll" or ext==".vst"

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
    class RepError(StandardError):
        pass
    url="http://public.plugbox.vstforx.de"
    dst_path="."
    install_loc=""
    __filter=None
    to_download = []
    element_map={}
        
    def __init__(self):
        pass
    
    def __filter_attr_values(self, filter_values, element_values):
         f_v = _unpack_commasep(filter_values)
         e_v = _unpack_commasep(element_values)
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

    def __process_plugin(self, el):
        self.install_loc = "%s/%s" % (self.install_loc, el.attrib["install-loc"])
        for x in el.iter("file"):
            if not self.__pass_filter(x):
                continue
            url = HTMLParser.HTMLParser().unescape(x.text)
            self.to_download.append(url)
            self.element_map[url] = x

    def __process_vendor(self, el):
        print "  fetching '%s':" % el.attrib['name']
        self.install_loc = "%s/%s" % (self.install_loc, el.attrib["install-loc"])
        for x in el.iter('plugin'):
            self.__process_plugin(x)
            
    def __process_tree(self, root):
        if root.tag != "plugin-repository":
            raise self.RepError("invalid repository file")
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
        
        
    def __load_rep_if_neccessary(self, **_filter):
        self.__filter = _filter
        self.__load_repository(self.url)
         
        
    def load_rep(self, **_filter):
        self.__load_rep_if_neccessary(**_filter)
        
    def sync(self, **filter):
        self.__load_rep_if_neccessary(**filter)
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

    def __get_binarylist(self, path):
        """ return list of filenames and md5 values """
        if _is_plugfile(path):
            f = open(path, "rb")
            md5 = _hashfile(f, hashlib.md5())
            f.close
            return [(os.path.basename(path), md5)]
        return filter(lambda x: _is_plugfile(x[0]), self.__get_archive_content(path))

    def __add_binaries(self, element, binarylist):
        for x in binarylist:
            n = xt.SubElement(element, "binary")
            n.attrib['md5'] = x[1]
            n.text = x[0]

    def __add_vendor(self, parent,  **vendorinfo):
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
        
    def add_vendor(self, **vendorinfo):
        self.__load_rep_if_neccessary()
        self.__add_vendor(self.root, **vendorinfo)

    def remove_vendor(self, vendor):
        self.__load_rep_if_neccessary()
        for x in self.root.iter("vendor"):
            if x.attrib['name'] == vendor:
                self.root.remove(x)
        
    def add_plugin(self, parent, **pkginf):
        """adds a new plugin and return new element 
        or if exists returns existing plugin element.
        if plugin None or without useful information
        the parent will be returned."""
        if pkginf == None:
            return parent
        if not pkginf.has_key('name'):
            return parent
        for x in parent.iter("plugin"):
            if x.attrib['name'] == pkginf['name']:
                return x
        node = xt.SubElement(parent, "plugin", pkginf)
        return node

    def add_file(self, path, url, vendorinfo, plugin, **attr):
        if not _is_localfile(self.url):
            raise self.RepError("repository url has to be a local file.")
        binaries = self.__get_binarylist(path)
        v = self.__add_vendor(self.root, **vendorinfo)
        v = self.add_plugin(v, **plugin)
        n = xt.SubElement(v, "file", attr)
        n.text=url
        self.__add_binaries(n, binaries)

    def init_rep(self, **attr):
        if not _is_localfile(self.url):
            raise self.RepError("repository has to be a local file")
        if os.path.exists(self.url):
            raise self.RepError("repository file already exists")
        self.root = xt.Element("plugin-repository", attr)

    def save(self):
        tree = xt.ElementTree(self.root)
        tree.write(self.url)

def _add_vendor(rep, attr):
    rep.add_vendor(**attr)
    rep.save()

def _remove_vendor(rep, attr):
    rep.remove_vendor(attr['name'])
    rep.save()

def _add_file(rep, attr):
    print attr

def _remove_file(rep, attr):
    print attr

def _init(rep, attr):
    rep.init_rep( **attr )
    rep.save()
        
    

def _sync(args):
    print args

def _add_default_args(parser):
    parser.add_argument('-url', '--url', help="specifies an url")
    parser.add_argument('-v', '--vendor', help="specifies a vendor")
    parser.add_argument('-pt', '--plattform', help="specifies a plattform")
    parser.add_argument('-ar', '--arch', help="specifies a architecture")
    parser.add_argument('-fm', '--format', help="specifies a plugin format")
    parser.add_argument('--author', help="specifies an author")
    parser.add_argument('-t', '--tags', help="specifies tags")  
    parser.add_argument('--location', help="specifies deploy target location")  
    
if __name__ == "__main__":
    from argparse import *
    parser = ArgumentParser(description="Welcome to PlugBox! I manage your plugins")
    parser.add_argument('repository', help="the repository, can be a local file or an url")
  

    txt="""valid subcommands are: 
        general: init, sync
        vendor: add-vendor, remove-vendor
        pluginfile: add-file, remove-file 
    """

    subparsers = parser.add_subparsers(title='vendor commands',
                                       description=txt)
    avp = subparsers.add_parser('add-vendor')
    avp.add_argument('name', help="the vendor name")
    _add_default_args(avp)
    avp.set_defaults(func=_add_vendor)
    
    rvp = subparsers.add_parser('remove-vendor')
    rvp.add_argument('name', help="the vendor name")
    _add_default_args(rvp)
    rvp.set_defaults(func=_remove_vendor)

    afp = subparsers.add_parser('add-file')
    afp.add_argument('name', help="the file path")
    _add_default_args(afp)
    afp.set_defaults(func=_add_file)

    rfp = subparsers.add_parser('remove-file')
    rfp.add_argument('name', help="the file path")
    _add_default_args(rfp)
    rfp.set_defaults(func=_add_file)

    gip = subparsers.add_parser('init')
    _add_default_args(gip)
    gip.set_defaults(func=_init)
    
    gsp = subparsers.add_parser('sync')
    _add_default_args(gsp)
    gsp.set_defaults(func=_sync)

    args=parser.parse_args()

    try:
        rep = RepSync()
        attr = _filter_attr(vars(args))
        rep.url = attr.pop("repository")
        args.func(rep, attr)
    except RepSync.RepError, ex:
        print ex
    
