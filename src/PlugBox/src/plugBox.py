#!/usr/bin/python
"""
downloads and install audio plugins automatically according to 
repository input file. 

Copyright (C) 2013 saba godschynski, www.vstforx.de

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
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
import shutil
import stat

HEADER = { 'User-Agent' : 'PlugBox' }


def _prettify(elem):
    return xt.tostring(elem)

    #TODO doesn't work:
    """Return a pretty-printed XML string for the Element.
       sources: http://stackoverflow.com/questions/749796/pretty-printing-xml-in-python
                http://doughellmann.com/2010/03/pymotw-creating-xml-documents-with-elementtree.html
    """
    from xml.dom import minidom
    rough_string = xt.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    uglyXml = reparsed.toprettyxml(indent="  ")
    text_re = re.compile('>\n\s+([^<>\s].*?)\n\s+</', re.DOTALL)    
    return text_re.sub('>\g<1></', uglyXml)

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
    if ext==".dll" or ext==".vst": #.vst means vst3 not mac vst
        return True
    #Mac vst == directory
    if re.match(".*?.vst/Contents/MacOS/.+$", f):
        return True
    return False
    

def _hashfile(f, hasher, blocksize=65536):
    buf = f.read(blocksize)
    while len(buf) > 0:
        hasher.update(buf)
        buf = f.read(blocksize)
    return hasher.digest().encode('hex_codec')

def _get_hash(path):
    f = open(path, "rb")
    hash = _hashfile(f, hashlib.md5())
    f.close()
    return hash


def _download(url, file_name, post_txt="downloading"):
    """
    downloads url with statusbar output: post_txt filename: [n%]
    """
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
        status = r"%s %10d [%3.2f%%]" % (post_txt, file_size_dl, file_size_dl * 100. / file_size)
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
    install_loc=""
    __filter=None
    to_download = {}
    verbose = True
    download_path = ".tmp"
    failed = []
    succeed = []
    def __print(self, str):
        if not self.verbose:
            return
        print (str)
    
    def __init__(self):
        if not os.path.exists(self.download_path):
            os.makedirs(self.download_path)
    
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

    def __download(self, url, dst_path):
        fname = os.path.basename(dst_path)
        if os.path.exists(dst_path):
            self.__print("        %s downloaded already" % fname)
            return
        _download(url, dst_path, "        downloading %s:" % fname)
        

    def __unzip(self,file,destdir):
        z = zip.ZipFile(open(file, "rb"))
        for f in z.namelist():
            if os.sep == "\\" and "/" in f:
                destfile = os.path.join(destdir,f.replace("/","\\"))
            else:
                destfile = os.path.join(destdir,f)
            if destfile.endswith(os.sep):
                if not os.path.exists(destfile):
                    os.makedirs(destfile)
            else:
                file = open(destfile,"wb")
                file.write(z.read(f))
                file.close()
                ext = os.path.splitext(destfile)
                if len(ext) <= 1:
                    continue
                if ext[1] != '.dll':
                    continue
                # .dll files are'nt loadable in windows
                # using cygwin fix:
                os.chmod(destfile, stat.S_IWRITE |
                         stat.S_IREAD | 
                         stat.S_IEXEC)
        z.close()
    

    def __deploy(self, src, dst):
        self.__print("        deploying into %s" %dst)
        if _is_plugfile(src):
            shutil.copy(src, dst)
            return
        ext = os.path.splitext(src)[1]
        if ext==".zip":
            self.__unzip(src, dst)
                
        

    def __install(self, pluginfo):
        fname = "%s/%s" % (self.download_path, pluginfo[1]['filename'])
        self.__print("    installing %s" % pluginfo[0])
        self.__download(pluginfo[0], fname)
        self.__deploy(fname, pluginfo[1]['install_loc'])

    def __need_to_update(self, x):
        path = x['install_loc']
        for v in x['binaries']:
            f = path + '/' + v[0]
            if not os.path.exists(f):
                return True
            if _get_hash(f) != v[1]:
                return True
        return False
        

    def __process_downloads(self):
        for x in self.to_download.items():
            loc = x[1]['install_loc']
            if not os.path.exists(loc):
                os.makedirs(loc)
            if not self.__need_to_update(x[1]):
                self.__print("    %s is up to date." % x[0])
                self.succeed.append(x)
                continue
            try:
                self.__install(x)
            except Exception, ex:
                self.__print("    installation failed: '%s'" % ex)
                self.failed.append(x)
                continue
            if self.__need_to_update(x[1]):
                self.failed.append(x)
                self.__print("    %s: installation failed" % x[0])
            else:
                self.succeed.append(x)
                
    def __update_install_loc(self, el):
        tmp = self.install_loc
        if not el.attrib.has_key("location"):
            return tmp
        self.install_loc = "%s/%s" % (self.install_loc, el.attrib["location"])
        return tmp
        
    def __process_binaries(self, el, out_dict):
        out_dict['binaries'] = l = []
        for x in el.iter("binary"):
            l.append((HTMLParser.HTMLParser().unescape(x.text), x.attrib['md5']))

    def __process_plugin(self, el):
        tmp = self.__update_install_loc(el)
        for x in el.iter("file"):
            if not self.__pass_filter(x):
                continue
            url = HTMLParser.HTMLParser().unescape(x.text)
            self.to_download[url] = v = {}
            v['element'] = x
            v['install_loc'] = self.install_loc
            v['filename'] = x.attrib['filename']
            self.__process_binaries(x, v)
        self.install_loc = tmp

    def __process_vendor(self, el):
        tmp = self.__update_install_loc(el)
        for x in el.iter('plugin'):
            self.__process_plugin(x)
        self.install_loc = tmp

    def __process_tree(self, root):
        if root.tag != "plugin-repository":
            raise self.RepError("invalid repository file")
        tmp = self.__update_install_loc(root)
        for x in root.iter('vendor'):
            self.__process_vendor(x)
        self.install_loc = tmp
        
    def __load_repository(self, url):
        if _is_localfile(url):
            f = open(url, "r")
            data = f.read()
            f.close()
        else:
            data = _req_GET(url)
            data = data.replace("\n", "").replace(" ", "")
        self.root = xt.XML(data)
        
        
    def __load_rep_if_neccessary(self):
        if vars(self).has_key('root'):
            if self.root != None:
                return
        self.__load_repository(self.url)
         
        
    def _load_to_download(self, **filter):
        """for test purpose"""
        self.__filter = filter
        self.__load_rep_if_neccessary()
        self.__process_tree(self.root)
        
    def load_rep(self, force_reload=False):
        if force_reload:
            self.root = None
        self.__load_rep_if_neccessary()
        self.__process_tree(self.root)
        
    def sync(self, **filter):
        self.__print("start syncing:")
        self.__filter = filter
        self.__load_rep_if_neccessary()
        self.__process_tree(self.root)
        if len(self.to_download)==0:
            raise self.RepError("no matching downloads found")
        
        self.__process_downloads()
        ss = "%i installations succeed" % len(self.succeed)
        sf = "%i installations failed" % len(self.failed)
        self.__print("- " * (max(len(sf), len(ss)) / 2))
        self.__print(ss)
        self.__print(sf)
        
        
        
    
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
        return []

    def __get_binarylist(self, path):
        """ return list of filenames and md5 values """
        if _is_plugfile(path):
            md5 = _get_hash(path)
            return [(os.path.basename(path), md5)]
        return filter(lambda x: _is_plugfile(x[0]), self.__get_archive_content(path))

    def __add_binaries(self, element, binarylist):
        for x in binarylist:
            n = xt.SubElement(element, "binary")
            n.attrib['md5'] = x[1]
            n.text = x[0]

    def add_vendor(self, **vendorinfo):
        self.__load_rep_if_neccessary()
        if self.find_vendor(vendorinfo['name'])!=None:
            raise self.RepError("vendor already exists")
        xt.SubElement(self.root, "vendor", vendorinfo)

    def find_vendor(self, vendor):
        self.__load_rep_if_neccessary()
        for x in self.root.iter("vendor"):
            if x.attrib['name'] == vendor:
                return x
        return None

    def remove_vendor(self, vendor):
        self.__load_rep_if_neccessary()
        v = self.find_vendor(vendor)
        if v==None:
            return
        self.root.remove(v)
        
    def add_plugin(self, vendor, **pkginf):
        self.__load_rep_if_neccessary()
        v = self.find_vendor(vendor)
        if v == None:
            raise self.RepError("unknown vendor")
        if self.find_plugin(v, pkginf['name']) != None:
            raise self.RepError("plugin already exists")
        xt.SubElement(v, "plugin", **pkginf)

    def find_plugin(self, vendorel, plugin):
        self.__load_rep_if_neccessary()
        if vendorel == None:
            raise self.RepError("invalid vendor")
        for x in vendorel:
            if x.attrib['name'] == plugin:
                return x
        return None

    def remove_plugin(self, plugin, vendor):
        self.__load_rep_if_neccessary()
        
        v = self.find_vendor(vendor)
        if v==None:
            raise self.RepError("vendor not found")
            

        n = self.find_plugin(v, plugin)
        if n == None:
            raise self.RepError("plugin not found")
        v.remove(n)

    def add_file(self, path, url, vendor, plugin, **attr):
        self.__load_rep_if_neccessary()
        binaries = self.__get_binarylist(path)
        if len(binaries) == 0:
            raise self.RepError("%s contains no pluginfiles" % path)
        v = self.find_vendor(vendor)
        p = self.find_plugin(v, plugin)
        if p == None:
            raise self.RepError("plugin not found")
        if self.find_file(p, url) != None:
            raise self.RepError("url already exists")
        n = xt.SubElement(p, "file", attr)
        n.text=url
        self.__add_binaries(n, binaries)
    
    def find_file(self,  pluginel, url):
        self.__load_rep_if_neccessary()
        for x in pluginel:
            if x.text == url:
                return x
        return None

    def remove_file(self, url, vendor, plugin):
        self.__load_rep_if_neccessary()
        v = self.find_vendor(vendor)
        p = self.find_plugin(v, plugin)
        if p == None:
            raise self.RepError("plugin not found")
        f = self.find_file(p, url)
        if f==None:
             raise self.RepError("url not found")
        p.remove(f)

    def init_rep(self, **attr):
        if os.path.exists(self.url):
            raise self.RepError("repository file already exists")
        self.root = xt.Element("plugin-repository", attr)

    def save(self):
        if not _is_localfile(self.url):
            raise self.RepError("repository has to be a local file")
        txt = _prettify(self.root)
        f = open(self.url, "w")
        f.write(txt)
        f.close()
        

def _add_vendor(rep, attr):
    rep.add_vendor(**attr)
    rep.save()

def _remove_vendor(rep, attr):
    rep.remove_vendor(attr['name'])
    rep.save()

def _add_plugin(rep, attr):
    vendor = attr.pop("vendor")
    rep.add_plugin(vendor, **attr)
    rep.save()

def _remove_plugin(rep, attr):
    rep.remove_plugin(attr['name'], attr['vendor'])
    rep.save()

def _add_file(rep, attr):
    path = attr.pop('path')
    url = attr.pop('url')
    vendor = attr.pop('vendor')
    plugin = attr.pop('plugin')
    attr['filename'] = os.path.basename(path)
    rep.add_file(path, url, vendor, plugin, **attr)
    rep.save()

def _remove_file(rep, attr):
    url = attr.pop('url')
    vendor = attr.pop('vendor')
    plugin = attr.pop('plugin')
    rep.remove_file(url, vendor, plugin)
    rep.save()

def _init(rep, attr):
    rep.init_rep( **attr )
    rep.save()
        
    

def _sync(rep, attr):
    t = attr.pop('target')
    rep.install_loc = t
    rep.sync(**attr)

def _add_default_args(parser):
    parser.add_argument('--name', help="specifies a name")
    parser.add_argument('-t', '--tags', help="specifies tags")  
    parser.add_argument('-l', '--location', help="specifies deployment location")  
    
def _add_filter_args(parser):
    parser.add_argument('--plattform', help="specifies a plattform")
    parser.add_argument('--arch', help="specifies a architecture")
    parser.add_argument('--format', help="specifies a plugin format")
    
if __name__ == "__main__":
    from argparse import *
    parser = ArgumentParser(description="Welcome to PlugBox! I manage your plugins")
    parser.add_argument('repository', help="the repository, can be a local file or an url")
  

    txt="""valid subcommands are: 
        general: init, sync
        vendor: add-vendor, remove-vendor
        plugin: add-plugin, remove-plugin, add-file, remove-file 
    """

    subparsers = parser.add_subparsers(title='vendor commands',
                                       description=txt)
    avp = subparsers.add_parser('add-vendor')
    avp.add_argument('name', help="the vendor name")
    avp.add_argument('--url', help="the file related source url")
    _add_default_args(avp)
    avp.set_defaults(func=_add_vendor)
    
    rvp = subparsers.add_parser('remove-vendor')
    rvp.add_argument('name', help="the vendor name")
    rvp.set_defaults(func=_remove_vendor)

    app = subparsers.add_parser('add-plugin')
    app.add_argument('name', help="the plugin name")
    app.add_argument('vendor', help="the plugin vendor")
    _add_default_args(app)
    app.set_defaults(func=_add_plugin)

    rpp = subparsers.add_parser('remove-plugin')
    rpp.add_argument('name', help="the plugin name")
    rpp.add_argument('vendor', help="the plugin vendor")
    rpp.set_defaults(func=_remove_plugin)
    
    
    afp = subparsers.add_parser('add-file')
    afp.add_argument('path', help="the file path")
    afp.add_argument('url', help="the file related source url")
    afp.add_argument('vendor', help="the file related vendor")
    afp.add_argument('plugin', help="the file related plugin")
    afp.add_argument('--version', help="specifies the plugin version")
    _add_filter_args(afp)
    afp.set_defaults(func=_add_file)


    rfp = subparsers.add_parser('remove-file')
    rfp.add_argument('url', help="the file related source url")
    rfp.add_argument('vendor', help="the file related vendor")
    rfp.add_argument('plugin', help="the file related plugin")
    rfp.set_defaults(func=_remove_file)
    
    gip = subparsers.add_parser('init')
    gip.add_argument('--author', help="specifies an author")
    gip.add_argument('--url', help="the repository related source url")
    _add_default_args(gip)
    gip.set_defaults(func=_init)
    
    gsp = subparsers.add_parser('sync')
    _add_filter_args(gsp)
    gsp.add_argument('target', help="specifies the target path")
    gsp.set_defaults(func=_sync)

    args=parser.parse_args()

    try:
        rep = RepSync()
        attr = _filter_attr(vars(args))
        rep.url = attr.pop("repository")
        args.func(rep, attr)
    except RepSync.RepError, ex:
        print ex
    except IOError, ex:
        print ex
    
