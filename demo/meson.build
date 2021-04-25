demo_cflags = [
    '-DSRCDIR="' + meson.current_source_dir() + '"',
    '-DBUILDDIR="' + meson.current_build_dir() + '"',
    '-DPKGDATADIR="' + pkgdatadir + '"'
]

cpml_demo = executable('cpml-demo',
    sources:      [ 'cpml-demo.c', 'demo.c', 'demo.h' ],
    dependencies: adg_dep,
    c_args:       demo_cflags,
    install:      true
)

adg_demo = executable('adg-demo',
    sources:      [ 'adg-demo.c', 'demo.c', 'demo.h' ],
    dependencies: adg_dep,
    c_args:       demo_cflags,
    install:      true
)

adg_demo_ui = configure_file(
    input:         'adg-demo.ui.in',
    output:        '@BASENAME@',
    configuration: data,
    install_dir:   pkgdatadir
)
