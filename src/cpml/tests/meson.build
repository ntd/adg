cpml_tests = [
    'test-arc',
    'test-curve',
    'test-extents',
    'test-gobject',
    'test-pair',
    'test-primitive',
    'test-segment',
    'test-utils',
]

foreach test: cpml_tests
    e = executable(test,
        sources:      '@0@.c'.format(test),
        dependencies: adgtest_dep
    )
    test('cpml/' + test, e)
endforeach
