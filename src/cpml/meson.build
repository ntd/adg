cpml_directories = include_directories('.')


cpml_c_files = files([
    'cpml-arc.c',
    'cpml-curve.c',
    'cpml-extents.c',
    'cpml-gobject.c',
    'cpml-line.c',
    'cpml-pair.c',
    'cpml-primitive.c',
    'cpml-segment.c',
    'cpml-utils.c',
])

cpml_h_files = files([
    'cpml-arc.h',
    'cpml-curve.h',
    'cpml-extents.h',
    'cpml-gobject.h',
    'cpml-internal.h',
    'cpml-pair.h',
    'cpml-primitive.h',
    'cpml-segment.h',
    'cpml-utils.h',
])

cpml_internals = files([
    'cpml-primitive-private.h',
])


# Build the CPML library
cpml_deps = [
    m_dep,
    gobject_dep,
    cairogobject_dep,
    cairo_dep,
]

cpml_soversion = '@0@.@1@.@2@'.format(
    cpml_current - cpml_age,
    cpml_age,
    cpml_revision
)

cpml_headers = cpml_h_files + [ cpml_h ]
cpml_sources = cpml_c_files + cpml_headers + cpml_internals

cpml = library('cpml-' + metadata['api'],
    sources:      cpml_sources,
    dependencies: cpml_deps,
    version:      cpml_soversion,
    install:      true
)

cpml_dep = declare_dependency(
    sources:             cpml_headers,
    dependencies:        cpml_deps,
    link_with:           cpml,
    include_directories: src_directories
)

install_headers(cpml_h_files, subdir: 'cpml')


# pkg-config support for CPML library
pkgconfig.generate(cpml,
    name:        'CPML',
    filebase:    'cpml-' + metadata['api'],
    description: 'Cairo Path Manipulation Library',
    url:         metadata['homepage'],
    subdirs:     pkgdir,
    version:     meson.project_version()
)


# GObject introspection support for CPML library
if introspection_dep.found()
    cpml_gir = gnome.generate_gir(cpml,
        sources:           cpml_c_files + [ cpml_h ] + cpml_h_files,
        namespace:         'Cpml',
        nsversion:         metadata['api'],
        identifier_prefix: 'Cpml',
        symbol_prefix:     'cpml',
        includes:          [ 'cairo-1.0' ],
        header:            'cpml.h',
        install:           true,
        extra_args: [
            '--quiet',
            '--sources-top-dirs=' + meson.current_source_dir(),
        ]
    )
endif
