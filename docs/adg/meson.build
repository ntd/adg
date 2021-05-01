bookinfo_xml = configure_file(
    input:         'bookinfo.xml.in',
    output:        '@BASENAME@',
    configuration: data
)

adg_fixxref_args = cpml_fixxref_args + [
    '--extra-dir=../cpml',
]
if gtk_dep.found()
    adg_fixxref_args += [
        '--extra-dir=' + join_paths(
            gtk_dep.get_variable(pkgconfig: 'prefix'),
            'share', 'gtk-doc', 'html',
            use_gtk2 ? 'gtk2' : 'gtk3'
        )
    ]
endif
if pangocairo_dep.found()
    adg_fixxref_args += [
        '--extra-dir=' + join_paths(
            pangocairo_dep.get_variable(pkgconfig: 'prefix'),
            'share', 'gtk-doc', 'html', 'pango'
        )
    ]
endif

gnome.gtkdoc('adg',
    namespace:      'adg',
    src_dir:        adg_directories,
    ignore_headers: adg_internal_names + [ 'adg-marshal.h', 'adg-cairo-fallback.h' ],
    dependencies:   adg_dep,
    main_xml:       'adg-docs.xml',
    content_files:  bookinfo_xml,
    scan_args:      [ '--rebuild-types', '--rebuild-sections' ],
    mkdb_args:      [ '--default-includes=adg.h' ],
    fixxref_args:   adg_fixxref_args,
    install_dir:    'adg-' + metadata['api']
)
