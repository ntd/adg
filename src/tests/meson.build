adgtest_sources = files([
    'adg-test.h',
    'adg-test.c',
])

adgtest_deps = [
    cpml_dep,
    adg_dep,
]

adgtest = library('adgtest',
    sources:      adgtest_sources,
    dependencies: adgtest_deps,
    include_directories: src_directories
)

adgtest_dep = declare_dependency(
    include_directories: include_directories('.'),
    dependencies:        adgtest_deps,
    link_with:           adgtest
)
