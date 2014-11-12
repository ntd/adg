General information
===================

Automatic Drawing Generation is GObject-based library that provides
a non-interactive cairo canvas specifically designed for generating
technical drawings. It can be viewed as a tool for programmatically
representing an abstract model into a layout (2D drafting).

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

Visit the [ADG web site](http://adg.entidi.com/) for further
details.


Design overview
===============

The project is based on the [GObject](http://www.gtk.org/) library:
the ADG canvas is developed in plain C using an object-oriented
approach. Applications based on ADG are not expected to be
developed in C, though: the basic idea is to have a set of language
bindings for higher level languages (the garbage-collected ones,
above all). An application can then be developed using any
specific language available, much in the same way as
[GNOME](http://www.gnome.org/) applications are conceived.
Lua bindings based on [LGI](http://github.com/pavouk/lgi) are
yet availables and effectively used upstream to test the APIs.
The adg-lua (http://dev.entidi.com/p/adg-lua/) project
provides code examples on how to use them.

The mathematical and geometrical algorithms, together with a bunch
of other useful functions for manipulating cairo paths, are kept
in a separated library called [CPML](http://adg.entidi.com/cpml/)
(Cairo Path Manipulation Library). Although actually embedded into
the ADG project, this library can be easily splitted on its own,
if needed.

The rendering leverages [cairo](http://cairographics.org/), so the
ADG canvas fully shares strong and weak points of that library.
Most notably, the availables export formats include PostScript,
SVG, PDF, PNG but no CAD specific ones (such as DXF). A complete
list can be [browsed online](http://cairographics.org/backends/).


External dependencies
=====================

The ADG library has the following dependencies:

 * [cairo](http://cairographics.org/) 1.7.4 or later, required by
   either CPML and ADG;
 * [GLib](http://www.gtk.org/) 2.10.1 or later, required by ADG;
 * [GTK+](http://www.gtk.org/) 3.0.0 or later (or GTK+ 2.12.0 or
   later for GTK+2 support) to optionally include GTK+ support and
   build the adg-demo program;
 * [pango](http://www.pango.org/) 1.18.0 or later (optional)
   to support a serious engine for rendering text instead of using
   the cairo "toy" APIs (only the pango-cairo module will be used);
 * [gtk-doc](http://www.gtk.org/gtk-doc/) 1.12 or later (optional),
   used to regenerate the API documentation
 * [GObject introspection](http://live.gnome.org/GObjectIntrospection)
   0.9.5 or later (optional) to dinamically generate bindings
   metadata.

The required packages must be installed prior the ADG building:
they should be availables on nearly every decent unix-like system.

The ADG is mainly developed on GNU/Linux but its dependecies are
known to be cross platform so a porting should be quite easy, if
not automatic. Anyway, the cross-compilation of a Windows
installer is supported upstream and it is based on the ArchLinux
[Fedora mingw port](http://github.com/ntd/aur-fedora-mingw).
