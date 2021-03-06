import gi	
from gi.repository import Gio, GLib	
import sys	

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)
    
def trash(path):
    try:
        f = Gio.File.new_for_path(path)
        f.trash(cancellable=None)
    except GLib.Error as e:
        eprint(e.code)

trash(sys.argv[1])
print("")
