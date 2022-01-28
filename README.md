General information
===================
[![Build Status](https://travis-ci.org/ntd/adg.svg?branch=master)](https://travis-ci.org/ntd/adg)
[![Coverage Status](https://coveralls.io/repos/ntd/adg/badge.svg?branch=master)](https://coveralls.io/r/ntd/adg?branch=master)

Automatic Drawing Generation is GObject-based library that provides
a non-interactive cairo canvas specifically designed for generating
technical drawings. It can be viewed as a 2D CAD without user
interface.

If you want to develop an application that needs to show and print
drawings where some data change but the overall design is similar,
the ADG library can be quite useful. In manufacturing industries
this is often the case and an ADG-based application could help to
greatly improve the productivity of technical offices.

You could reach similar targets using parametric CADs but a custom
application provides the following benefits:

 * more customizable;
 * quicker and not so bloated as a parametric CAD;
 * can be easily connected to a database;
 * an ADG based filter can generate drawing on-fly: this feature
   could be used in web-based applications.

Visit the [ADG web site](https://adg.entidi.com/) for further
details.


Design overview
===============

The project is based on the [GObject](https://www.gtk.org/) library:
the ADG canvas is developed in plain C using an object-oriented
approach. Applications based on ADG are not expected to be
developed in C, though: the basic idea is to have a set of bindings
for higher level languages (the garbage-collected ones, above all).
An application can then be developed using any specific language
available, much in the same way as [GNOME](https://www.gnome.org/)
applications are conceived.

Lua bindings based on [LGI](https://github.com/pavouk/lgi) are
already availables and effectively used upstream to test the APIs.
The [adg-lua](https://github.com/ntd/adg-lua) project provides code
examples on how to use them.

The mathematical and geometrical algorithms, together with a bunch
of other useful functions for manipulating cairo paths, are kept
in a separated library called `CPML` (Cairo Path Manipulation Library).
Although actually embedded into the ADG project, this library can be
easily splitted on its own, if needed.

The rendering leverages [cairo](https://www.cairographics.org/), so
the ADG canvas fully shares strong and weak points of that library.
Most notably, the availables export formats include PostScript, SVG,
PDF, PNG but no CAD specific ones (such as DXF).


External dependencies
=====================

The ADG library has the following dependencies:

 * [cairo](https://www.cairographics.org/) 1.7.4 or later, required
   by both CPML and ADG;
 * [GLib](https://www.gtk.org/) 2.38.0 or later, required by ADG;
 * [GTK+](https://www.gtk.org/) 3.0.0 or later (or GTK 2.18.0 or
   later for GTK2 support) to optionally include GTK support and build
   the `adg-demo` program;
 * [pango](https://pango.gnome.org/) 1.10.0 or later (optional) to
   support a serious engine for rendering text instead of using the
   cairo "toy" APIs (only the `pango-cairo` module will be used);
 * [gtk-doc](https://wiki.gnome.org/DocumentationProject/GtkDoc) 1.12
   or later (optional), used to regenerate the API documentation;
 * [GObject introspection](https://gi.readthedocs.io/) 1.0.0 or later
   (optional) to dinamically generate bindings metadata.

The required packages must be installed prior the ADG building:
they should be availables on nearly every decent unix-like system.

The ADG is mainly developed on GNU/Linux but its dependecies are
known to be cross platform so a porting should be quite easy, if
not automatic.


Installation
============

The ADG build system is based on GNU autotools, the de-facto
standard for handling free software projects. The following
step-by-step instructions are included only for reference: you are
free to install ADG in the way you prefer. Just remember if you
are copying and pasting to put the proper package version instead
of the 0.6.0 placeholder used in the examples.

1. Unpack the ADG tarball somewhere in your file system: your home
   folder is a good candidate. You can get the latest tarball directly
   from [GitHub](https://github.com/ntd/adg/releases).
    ```
    cd $HOME
    wget https://github.com/ntd/adg/releases/download/0.9.3/adg-0.9.3.tar.bz2
    tar xjvf adg-0.9.3.tar.bz2
    cd adg-0.9.3
    ```
2. Configure and run the build using autotools or the meson system
   (the latter only available on adg >= 0.9.4). Meson is using the
   `_build` folder instead of the usual `build` to avoid clashes
   with aldready existing folders.
    ```
    # Using autotools
    ./configure
    make
    # Using meson
    meson setup _build
    meson compile -C _build
    ```
3. Once the building has finished, you are able to run the demo
   program without installing anything. Just try it out by
   launching the uninstalled executable:
    ```
    demo/adg-demo-uninstalled
    ```
4. Install the library. You must usually have root privileges if you
   intend to install the library on your system folders.
    ```
    # Using autotools
    make install
    # Using meson
    meson intall -C _build
    ```
