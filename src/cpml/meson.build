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
    'cpml-pair.h',
    'cpml-primitive.h',
    'cpml-segment.h',
    'cpml-utils.h',
])

cpml_internal_names = [
    'cpml-internal.h',
    'cpml-primitive-private.h',
]


cpml_internal_files = files(cpml_internal_names)

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

cpml = library('cpml-1',
    sources:      cpml_c_files + cpml_h_files + cpml_internal_files,
    dependencies: cpml_deps,
    version:      cpml_soversion,
    install:      true
)

cpml_dep = declare_dependency(
    dependencies:        cpml_deps,
    link_with:           cpml,
    include_directories: src_directories
)

install_headers(cpml_h_files, subdir: 'adg-1/cpml')


# pkg-config support for CPML library
pkgconfig.generate(cpml,
    name:        'CPML',
    filebase:    'cpml-1',
    description: 'Cairo Path Manipulation Library',
    url:         metadata['homepage'],
    subdirs:     pkg,
    version:     meson.project_version()
)


# GObject introspection support for CPML library
if introspection_dep.found()
    cpml_gir_args = [ '--quiet' ]
    # --source-top-dirs only available on recent gobject-introspection
    if introspection_dep.version().version_compare('>=1.59.1')
        cpml_gir_args += [
            '--sources-top-dirs=' + meson.current_source_dir(),
            '--sources-top-dirs=' + meson.current_build_dir(),
        ]
    endif
    cpml_gir = gnome.generate_gir(cpml,
        sources:           [ cpml_h ] + cpml_h_files + cpml_c_files,
        namespace:         'Cpml',
        nsversion:         metadata['api'],
        identifier_prefix: 'Cpml',
        symbol_prefix:     'cpml',
        includes:          [ 'cairo-1.0' ],
        header:            'cpml.h',
        install:           true,
        extra_args:        cpml_gir_args
    )
endif
