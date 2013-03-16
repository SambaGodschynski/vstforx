import urllib2
import urllib
import argparse
import sys
import os
import getpass

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

args = parser.parse_args()
pwd = getpass.getpass("pwd: ")
args.pwd = pwd

t = get_title(args)
if not t == args.title:
    print "title missmatch: '%s' expected" % t
    sys.exit(0)
    

"""print post({
        "id": 89,
        "introtext": "bla",
        "fulltext": "",
})
"""
