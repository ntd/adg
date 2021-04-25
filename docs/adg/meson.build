bookinfo_xml = configure_file(
    input:         'bookinfo.xml.in',
    output:        '@BASENAME@',
    configuration: data
)

gnome.gtkdoc('adg',
    namespace: 'adg',
    src_dir: adg_directories,
    ignore_headers: adg_internal_names + [ 'adg-marshal.h' ],
    dependencies: adg_dep,
    main_xml: 'adg-docs.xml',
    content_files: bookinfo_xml,
    scan_args: [
        '--rebuild-types',
        '--rebuild-sections',
    ],
    mkdb_args: [
        '--default-includes=adg.h',
    ],
    fixxref_args: [
        '--extra-dir=' + meson.current_build_dir() + '../adg/html',
    ],
    install_dir: 'adg-' + metadata['api']
)
