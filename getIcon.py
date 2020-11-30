import gi	
from gi.repository import Gio	
gi.require_version('Gtk', '3.0')	
from gi.repository import Gtk	
import mimetypes	
import sys	

def get_icon_path(extension, size=16):	
    type_, encoding = mimetypes.guess_type('x.' + extension)	
    if type_:	
        icon = Gio.content_type_get_icon(type_)	
        theme = Gtk.IconTheme.get_default()	
        info = theme.choose_icon(icon.get_names(), size, 0)	
        if info:	
            return info.get_filename()	

print(get_icon_path(sys.argv[1])) 	
