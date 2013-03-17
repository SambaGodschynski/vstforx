import urllib2
import urllib
import argparse
import sys
import os
import getpass
import time
from exceptions import *
import signal
import datetime
import re

running = True

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
    if showdate:
        s = "%s: %s" % ( datetime.datetime.now().ctime(), s )
    if newline:
        sys.stdout.write(s + "\n")
        sys.stdout.flush()
        return
    sys.stdout.write(s)
    sys.stdout.flush()
    
def _lock(args):
    ids = []
    if os.path.exists(".lock"):
        f = open(".lock", "r")
        ids = f.readlines()
        f.close()
    for x in ids:
        if x == args.id:
            _print("another instance is observing %s already." % x)
            sys.exit(0)
    f = open(".lock", "w")
    ids.append(str(args.id))
    f.writelines(ids)
    f.close()

def _unlock(args):
    ids = []
    f = open(".lock", "r")
    ids = f.readlines()
    f.close()
    ids = filter( lambda x: x!=args.id, ids )
    f = open(".lock", "w")
    f.writelines(ids)
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
pwd = getpass.getpass("pwd: ")
args.pwd = pwd
_lock(args)
t = get_title(args)
if not t == args.title:
    _print ("title missmatch: '%s' expected" % t)
    sys.exit(0)

if args.fork:
    pid = os.fork()
    if not pid==0:
        _print ("stop the process with: kill -SIGTERM %d" % pid)
        sys.exit(0)
    signal.signal(signal.SIGINT, _sighandler)

commit(args)
dstamp = os.path.getmtime(args.path)
stamp = checkfile(args) 
_print ("starting observer...")
try:
    while running:
        ndstamp = os.path.getmtime(args.path)
        if dstamp == ndstamp:
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
_print ("done.")


