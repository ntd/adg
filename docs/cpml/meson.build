bookinfo_xml = configure_file(
    input:         'bookinfo.xml.in',
    output:        '@BASENAME@',
    configuration: data
)

gnome.gtkdoc('cpml',
    namespace: 'cpml',
    src_dir: cpml_directories,
    dependencies: cpml_dep,
    main_xml: 'cpml-docs.xml',
    content_files: bookinfo_xml,
    mkdb_args: [
        '--default-includes=cpml.h',
    ],
    install_dir: 'cpml-' + metadata['api']
)
