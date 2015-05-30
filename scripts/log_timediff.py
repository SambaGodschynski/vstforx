#!/usr/bin/python
# calculates time differences between boost::log messages

import sys
import re
from datetime import datetime

def getdiffs(lines):
    """
    caclucaltes time diffs of log lines and reurns [(line, datetime.timedelta)]
    """
    l = None
    res=[]
    for x in lines:
        m = re.match(".*?\[[0-9A-Za-z-]+ (\d+:\d+:\d+\.\d+)\].*", x)
        if (m==None):
            continue
        t = datetime.strptime(m.group(1), "%H:%M:%S.%f")
        if (l==None):
            l = t
            continue
        res.append((m.group(0), t-l))
        l = t
    return res

if __name__ == "__main__":
    if len(sys.argv)<=1:
        print("file missing")
        sys.exit(0)
    filename = sys.argv[1]
    f = open(filename, "r")
    lines = f.readlines();
    f.close()
    l = getdiffs(lines)
    for x in l:
        s = x[1].total_seconds()
        print (("%s;%s") % (x[0], s))
