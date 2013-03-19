import urllib2
import urllib
import argparse
import sys
import os
import getpass
import time
from exceptions import *
import datetime
import re
import pickle

running = True
isdaemon = False

def _post(args, values):
    url=args.url
    values['mgk'] = args.pwd
    data = urllib.urlencode(values)
    req = urllib2.Request(url, data)
    return urllib2.urlopen(req).read().strip()

def get_title(args):
    return _post(args,
                {"id": args.id, "cmd": "get_title"}
                )
def _parse(txt):
    res = re.split('< *hr *id=["\'] *system-readmore *["\'] */ *>', txt)
    if len(res) == 1:
        return res[0], ""
    return res[0], res[1]

def commit(args):
    _print ("commiting: %s ... " % args.path, False)
    f = open(args.path, "r")
    txt = f.read()
    f.close()
    intro, full = _parse(txt)
    res = _post(args, {"id": args.id, "introtext": intro, "fulltext": full})
    if (res=="0"):
        raise StandardError("commit failed.")
    _print("done.", True, False)

def checkfile(args):
    f = open(args.path, "r")
    txt = f.read()
    f.close()
    return hash(txt)

def _sighandler(signum, frame):
    global running
    running = False

def _print(s, newline=True, showdate=True):
    if isdaemon:
        out = open("log.txt", "a")
    else:
        out = sys.stdout
    if showdate:
        s = "%s: %s" % ( datetime.datetime.now().ctime(), s )
    if newline:
        out.write(s + "\n")
        out.flush()
        return
    out.write(s)
    out.flush()
    if isdaemon:
        out.close()
    
def _lock(args):
    ids = []
    if os.path.exists(".lock"):
        f = open(".lock", "r")
        ids = pickle.load(f)
        f.close()
    for x in ids:
        if x == args.id:
            _print("another instance is observing %s already." % x)
            sys.exit(0)
    f = open(".lock", "w")
    ids.append(str(args.id))
    pickle.dump(ids, f)
    f.close()

def _unlock(args):
    ids = []
    f = open(".lock", "r")
    ids = pickle.load(f)
    f.close()
    ids = filter( lambda x: x!=args.id, ids )
    f = open(".lock", "w")
    pickle.dump(ids, f)
    f.close()

parser = argparse.ArgumentParser(description="""joomla auto-commiter.
Observes a file and commits new content when file changed. """)
parser.add_argument('--file','-f',  
                    dest='path', 
                    help='the file to observe', 
                    required=True)
parser.add_argument('--url', '-u', 
                    dest='url', 
                    help='the url of the auto_commit.php file.', 
                    required=True)
parser.add_argument('--id', 
                    dest='id', 
                    help='the article id', 
                    required=True)
parser.add_argument('--title', '-t',
                   dest='title',
                   help="the article string(has to match)",
                   required=True)
parser.add_argument('--fork',
                    action="store_true",
                    help="starts child process and return")

args = parser.parse_args()
if not os.path.exists( args.path ):
    print args.path + " does not exists."
    sys.exit(0)
pwd = getpass.getpass("pwd: ")
args.pwd = pwd
t = get_title(args)
if not t == args.title:
    print ("title missmatch: '%s' expected" % t)
    sys.exit(0)

if args.fork:
    if os.name == "nt":
        print "os dosen't support forking."
        sys.exit(0)
    import signal
    pid = os.fork()
    if not pid==0:
        print ("stop the process with: kill -SIGTERM %d" % pid)
        sys.exit(0)
    signal.signal(signal.SIGTERM, _sighandler)
    isdaemon = True

_lock(args)
commit(args)
dstamp = os.path.getmtime(args.path)
stamp = checkfile(args) 
_print ("observer for %s started." % args.path)
try:
    while running:
        ndstamp = os.path.getmtime(args.path)
        if dstamp == ndstamp:
            time.sleep(1)
            continue
        dstamp = ndstamp
        nstamp = checkfile(args)
        if not nstamp == stamp:
            commit(args)
            stamp = nstamp
        time.sleep(1)
except KeyboardInterrupt:
    pass
_unlock(args)
_print ("observer for %s stoped." % args.path)
if isdaemon: 
    print "process %d exited." % os.getpid()

