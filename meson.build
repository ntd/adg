project('adg', 'c',
    meson_version: '>=0.56.0',
    version:       '0.9.4',
    license:       'LGPL-2.1-or-later'
)

metadata = {
    'api':      '1.0',
    'homepage': 'http://adg.entidi.com/',
    'tracker':  'https://track.entidi.com/project/view/2/',
}


# How to handle LT versions (current:revision:age):
# - If the library source code has changed at all since the last
#   update, then increment revision (‘c:r:a’ becomes ‘c:r+1:a’).
# - If any interfaces have been added, removed, or changed since the
#   last update, increment current, and set revision to 0.
# - If any interfaces have been added since the last public release,
#   then increment age.
# - If any interfaces have been removed or changed since the last
#   public release, then set age to 0.
cpml_current  = 4
cpml_revision = 3
cpml_age      = 0
adg_current  = 9
adg_revision = 0
adg_age      = 1


cc         = meson.get_compiler('c')
gnome      = import('gnome')
i18n       = import('i18n')
pkgconfig  = import('pkgconfig')
pkg        = meson.project_name() + '-' + metadata['api'].split('.')[0]
prefix     = get_option('prefix')
datadir    = join_paths(prefix, get_option('datadir'))
pkgdatadir = join_paths(datadir, pkg)
localedir  = join_paths(prefix, get_option('localedir'))
includedir = join_paths(prefix, get_option('includedir'), pkg)


m_dep = cc.find_library('m', required: false)

# G_ADD_PRIVATE macro required
gobject_dep = dependency('gobject-2.0', version: '>=2.38.0')

# cairo_scaled_font_text_to_glyphs() API required
cairo_dep = dependency('cairo', version: '>=1.7.4')

# Any release
cairogobject_dep = dependency('cairo-gobject')

# Cairo support in Pango required
pangocairo_dep = dependency('pangocairo', version: '>=1.10.0',
                            required: get_option('pango'))

# Lookup which GTK package to use
gtk = get_option('gtk')
if gtk == 'no'
    gtk_dep = dependency('', required: false)
else
    if gtk != 'gtk2'
        # First stable release
        gtk_dep = dependency('gtk+-3.0', version: '>= 3.0.0',
                             required: gtk == 'gtk3')
    endif
    use_gtk2 = gtk == 'gtk2' or not gtk_dep.found()
    if use_gtk2
        # gtk_widget_get_allocation() API required
        gtk_dep = dependency('gtk+-2.0', version: '>=2.18.0')
    endif
endif

# Both "check" and "yes" mean gladeui autodetection,
# but "yes" will fail if gladeui is not found
catalogdir = get_option('catalogdir')
if catalogdir == 'yes' or catalogdir == 'check'
    glade_dep = dependency('gladeui-2.0', required: false)
    if not glade_dep.found()
        glade_dep = dependency('gladeui-1.0', required: catalogdir == 'yes')
    endif
    if glade_dep.found()
        catalogdir = glade_dep.get_variable(pkgconfig: 'catalogdir')
    else
        catalogdir = ''
    endif
elif catalogdir == 'no'
    # Both 'no' and '' mean "Do not install catalogs"
    catalogdir = ''
endif

# Support for introspection annotations required
gtkdoc_dep = dependency('gtk-doc', required: get_option('gtk-doc'),
                        version: '>=1.12')

gtkdocdir = get_option('gtkdocdir')
if gtkdocdir == ''
    gtkdocdir = join_paths(get_option('prefix'), get_option('datadir'), 'gtk-doc', 'html')
endif

# First stable release
introspection_dep = dependency('gobject-introspection-1.0', version: '>=1.0.0',
                               required: get_option('introspection'))

# Check if GLib supports the testing framework
testing = dependency('glib-2.0', version: '>=2.16.0',
                     required: get_option('tests'))


subdir('src')
subdir('po')
if gtk_dep.found()
    subdir('demo')
endif
if gtkdoc_dep.found()
    subdir('docs/cpml')
    subdir('docs/adg')
endif


summary({
    'Compiler':
        cc.get_id(),
    'CPML library to use':
        'internal (cpml-1 @0@)'.format(meson.project_version()),
    'Use pango for text':
        pangocairo_dep.found() ?
            'yes (@0@ @1@)'.format(pangocairo_dep.name(), pangocairo_dep.version()) :
            'no (use cairo toy API)',
    'GTK support':
        gtk_dep.found() ?
            'yes (@0@ @1@)'.format(gtk_dep.name(), gtk_dep.version()) :
            'no',
    'Install glade catalogs':
        catalogdir == '' ? 'no' : 'yes, install into @0@'.format(catalogdir),
    'Build API reference':
        gtkdoc_dep.found() ?
            'yes (@0@ @1@), install into @2@'.
                format(gtkdoc_dep.name(), gtkdoc_dep.version(), gtkdocdir) :
            'no',
    'GObject instrospection':
        introspection_dep.found() ?
            'yes (@0@ @1@)'.format(introspection_dep.name(), introspection_dep.version()) :
            'no',
    'Test framework support':
        testing.found() ? 'yes' : 'no'
})
