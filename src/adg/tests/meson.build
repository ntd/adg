adg_tests = [
    'test-adim',
    'test-alignment',
    'test-arrow',
    'test-canvas',
    'test-color-style',
    'test-container',
    'test-dash',
    'test-dim-style',
    'test-dim',
    'test-dress',
    'test-edges',
    'test-entity',
    'test-fill-style',
    'test-font-style',
    'test-hatch',
    'test-ldim',
    'test-line-style',
    'test-logo',
    'test-marker',
    'test-matrix',
    'test-model',
    'test-param-dress',
    'test-path',
    'test-point',
    'test-projection',
    'test-rdim',
    'test-ruled-fill',
    'test-stroke',
    'test-style',
    'test-table-cell',
    'test-table-row',
    'test-table-style',
    'test-table',
    'test-text',
    'test-textual',
    'test-title-block',
    'test-toy-text',
    'test-trail',
    'test-type-builtins',
    'test-utils',
]

if pangocairo_dep.found()
    adg_tests += [
        'test-pango-style',
    ]
endif

if gtk_dep.found() and gtk_dep.version().version_compare('<3.10.8')
    adg_tests += [
        'test-gtk-area',
        'test-gtk-layout',
        'test-gtk-utils',
    ]
endif

adg_tests_cflags = [
    '-DSRCDIR="' + meson.current_source_dir() + '"'
]

foreach test: adg_tests
    e = executable(test,
        sources:      '@0@.c'.format(test),
        c_args:       adg_tests_cflags,
        dependencies: adgtest_dep
    )
    test('adg/' + test, e)
endforeach
