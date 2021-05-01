adg_directories = include_directories('.')


adg_canvas_h = configure_file(
    input:         'adg-canvas.h.in',
    output:        '@BASENAME@',
    configuration: data
)

adg_marshal = gnome.genmarshal('adg-marshal',
    sources:        'adg-marshal.genmarshal',
    prefix:         'adg_marshal',
    internal:       true,
    install_header: true,
    install_dir:    join_paths(includedir, 'adg'),
    extra_args:     [ '--quiet' ]
)
adg_marshal_c = adg_marshal[0]
adg_marshal_h = adg_marshal[1]

adg_type_builtins = gnome.mkenums('adg-type-builtins',
    sources:        'adg-enums.h',
    c_template:     'adg-type-builtins.c.mkenums',
    h_template:     'adg-type-builtins.h.mkenums',
    install_header: true,
    install_dir:    join_paths(includedir, 'adg')
)
adg_type_builtins_c = adg_type_builtins[0]
adg_type_builtins_h = adg_type_builtins[1]

adg_c_built = [ adg_type_builtins_c, adg_marshal_c ]
adg_h_built = [ adg_type_builtins_h, adg_marshal_h, adg_canvas_h ]

adg_c_files = files([
    'adg-adim.c',
    'adg-alignment.c',
    'adg-arrow.c',
    'adg-cairo-fallback.c',
    'adg-canvas.c',
    'adg-color-style.c',
    'adg-container.c',
    'adg-dash.c',
    'adg-dim-style.c',
    'adg-dim.c',
    'adg-dress.c',
    'adg-edges.c',
    'adg-entity.c',
    'adg-enums.c',
    'adg-fill-style.c',
    'adg-font-style.c',
    'adg-hatch.c',
    'adg-ldim.c',
    'adg-line-style.c',
    'adg-logo.c',
    'adg-marker.c',
    'adg-matrix.c',
    'adg-model.c',
    'adg-param-dress.c',
    'adg-path.c',
    'adg-point.c',
    'adg-projection.c',
    'adg-rdim.c',
    'adg-ruled-fill.c',
    'adg-stroke.c',
    'adg-style.c',
    'adg-table-cell.c',
    'adg-table-row.c',
    'adg-table-style.c',
    'adg-table.c',
    'adg-text.c',
    'adg-textual.c',
    'adg-title-block.c',
    'adg-toy-text.c',
    'adg-trail.c',
    'adg-utils.c',
])

adg_h_files = files([
    'adg-adim.h',
    'adg-alignment.h',
    'adg-arrow.h',
    'adg-cairo-fallback.h',
    'adg-color-style.h',
    'adg-container.h',
    'adg-dash.h',
    'adg-dim-style.h',
    'adg-dim.h',
    'adg-dress.h',
    'adg-edges.h',
    'adg-entity.h',
    'adg-enums.h',
    'adg-fill-style.h',
    'adg-font-style.h',
    'adg-hatch.h',
    'adg-ldim.h',
    'adg-line-style.h',
    'adg-logo.h',
    'adg-marker.h',
    'adg-matrix.h',
    'adg-model.h',
    'adg-param-dress.h',
    'adg-path.h',
    'adg-point.h',
    'adg-projection.h',
    'adg-rdim.h',
    'adg-ruled-fill.h',
    'adg-stroke.h',
    'adg-style.h',
    'adg-table-cell.h',
    'adg-table-row.h',
    'adg-table-style.h',
    'adg-table.h',
    'adg-text.h',
    'adg-text-internal.h',
    'adg-textual.h',
    'adg-toy-text.h',
    'adg-title-block.h',
    'adg-trail.h',
    'adg-utils.h',
])

adg_internal_names = [
    'adg-adim-private.h',
    'adg-alignment-private.h',
    'adg-arrow-private.h',
    'adg-canvas-private.h',
    'adg-color-style-private.h',
    'adg-container-private.h',
    'adg-dash-private.h',
    'adg-dim-private.h',
    'adg-dim-style-private.h',
    'adg-dress-private.h',
    'adg-edges-private.h',
    'adg-entity-private.h',
    'adg-fill-style-private.h',
    'adg-font-style-private.h',
    'adg-forward-declarations.h',
    'adg-hatch-private.h',
    'adg-internal.h',
    'adg-ldim-private.h',
    'adg-line-style-private.h',
    'adg-logo-private.h',
    'adg-marker-private.h',
    'adg-model-private.h',
    'adg-path-private.h',
    'adg-projection-private.h',
    'adg-rdim-private.h',
    'adg-ruled-fill-private.h',
    'adg-stroke-private.h',
    'adg-table-private.h',
    'adg-table-style-private.h',
    'adg-text-private.h',
    'adg-title-block-private.h',
    'adg-toy-text-private.h',
    'adg-trail-private.h',
]

if pangocairo_dep.found()
    adg_c_files += files([
        'adg-pango-style.c',
    ])
    adg_h_files += files([
        'adg-pango-style.h',
    ])
    adg_internal_names += 'adg-pango-style-private.h'
endif

if gtk_dep.found()
    adg_c_files += files([
        'adg-gtk-area.c',
        'adg-gtk-layout.c',
        'adg-gtk-utils.c',
    ])
    adg_h_files += files([
        'adg-gtk-area.h',
        'adg-gtk-layout.h',
        'adg-gtk-utils.h',
    ])
    adg_internal_names += [
        'adg-gtk-area-private.h',
        'adg-gtk-layout-private.h',
    ]
endif

adg_internal_files = files(adg_internal_names)


# Build the ADG library
adg_deps = [
    cpml_dep,
    pangocairo_dep,
    gtk_dep,
]

adg_cflags = [
    '-DLOCALEDIR="' + localedir + '"',
]

adg_soversion = '@0@.@1@.@2@'.format(
    adg_current - adg_age,
    adg_age,
    adg_revision
)

adg = library('adg-' + metadata['api'],
    sources:      adg_c_files + adg_c_built + adg_h_files + adg_h_built + adg_internal_files,
    dependencies: adg_deps,
    version:      adg_soversion,
    c_args:       adg_cflags,
    install:      true
)

adg_dep = declare_dependency(
    sources:             adg_h_built,
    dependencies:        adg_deps,
    compile_args:        adg_cflags,
    link_with:           adg,
    include_directories: src_directories
)

install_headers(adg_h_files, subdir: 'adg')


# pkg-config support for ADG library
pkgconfig.generate(adg,
    name:        'ADG',
    filebase:    'adg-' + metadata['api'],
    description: 'Automatic Drawing Generation',
    url:         metadata['homepage'],
    subdirs:     pkgdir,
    version:     meson.project_version()
)


# GObject introspection support for ADG library
if introspection_dep.found()
    adg_gir_includes = [ 'GObject-2.0', 'cairo-1.0', cpml_gir[0] ]
    if pangocairo_dep.found()
        adg_gir_includes += 'PangoCairo-1.0'
    endif
    if gtk_dep.found()
        adg_gir_includes += use_gtk2 ? 'Gtk-2.0' : 'Gtk-3.0'
    endif
    adg_gir = gnome.generate_gir(adg,
        sources:             [ adg_h ] + adg_h_files + adg_h_built + adg_c_files + adg_c_built,
        namespace:           'Adg',
        nsversion:           metadata['api'],
        identifier_prefix:   'Adg',
        symbol_prefix:       'adg',
        includes:            adg_gir_includes,
        include_directories: src_directories,
        header:              'adg.h',
        install:             true,
        extra_args: [
            '--quiet',
            '--sources-top-dirs=' + meson.current_source_dir(),
            '--sources-top-dirs=' + meson.current_build_dir(),
        ]
    )
endif
