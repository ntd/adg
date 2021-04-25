src_directories = include_directories('.')


# Populate the configuration data table
data = configuration_data({
    'PACKAGE_NAME':      meson.project_name(),
    'PACKAGE_VERSION':   meson.project_version(),
    'PACKAGE_API':       metadata['api'],
    'PACKAGE_URL':       metadata['homepage'],
    'PACKAGE_BUGREPORT': metadata['tracker'],
})

adg_h_additional = []
adg_canvas_h_additional = []

if pangocairo_dep.found()
    data.set('PANGO_ENABLED', 1)
    adg_h_additional += [
        '#include <pango/pango.h>',
        '#include "adg/adg-text.h"',
        '#include "adg/adg-pango-style.h"',
    ]
endif

if gtk_dep.found()
    data.set(use_gtk2 ? 'GTK2_ENABLED' : 'GTK3_ENABLED', 1)
    adg_h_additional += [
        '#include <gtk/gtk.h>',
        '#include "adg/adg-gtk-utils.h"',
        '#include "adg/adg-gtk-area.h"',
        '#include "adg/adg-gtk-layout.h"',
    ]
    adg_canvas_h_additional += [
        '#include <gtk/gtk.h>',
        '',
        'void            adg_canvas_set_paper            (AdgCanvas      *canvas,',
        '                                                 const gchar    *paper_name,',
        '                                                 GtkPageOrientation orientation);',
        'void            adg_canvas_set_page_setup       (AdgCanvas      *canvas,',
        '                                                 GtkPageSetup   *page_setup);',
        'GtkPageSetup *  adg_canvas_get_page_setup       (AdgCanvas      *canvas);',
    ]
endif

if introspection_dep.found()
    data.set('CAIRO_GOBJECT_ENABLED', 1)
endif

data.set('ADG_H_ADDITIONAL', '\n'.join(adg_h_additional))
data.set('ADG_CANVAS_H_ADDITIONAL', '\n'.join(adg_canvas_h_additional))


# Define or generate main header files
config_h = configure_file(
    input: 'config.h.meson',
    output: '@BASENAME@',
    configuration: data
)

cpml_h = files('cpml.h')[0]

adg_h = configure_file(
    input: 'adg.h.in',
    output: '@BASENAME@',
    configuration: data,
    install_dir: includedir
)

if catalogdir != ''
    install_data('adg.xml', install_dir: catalogdir)
endif

subdir('cpml')
subdir('adg')

if testing.found()
    subdir('tests')
    subdir('cpml/tests')
    subdir('adg/tests')
endif
