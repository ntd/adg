/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


#include <adg-test.h>
#include <adg.h>

#define ADG_TYPE_DUMMY      (adg_dummy_get_type())


typedef GObject AdgDummy;
typedef GObjectClass AdgDummyClass;

static void
adg_dummy_class_init(AdgDummyClass *klass)
{
}

static void
_adg_dummy_iface_init(AdgTextualIface *iface)
{
    iface->set_font_dress = NULL;
    iface->get_font_dress = NULL;
    iface->set_text = NULL;
    iface->dup_text = NULL;
    iface->text_changed = NULL;
}

static void
adg_dummy_init(AdgDummy *dummy)
{
}

G_DEFINE_TYPE_WITH_CODE(AdgDummy, adg_dummy, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(ADG_TYPE_TEXTUAL,
                                              _adg_dummy_iface_init))


static void
_adg_behavior_misc(void)
{
    AdgDummy *dummy = g_object_new(ADG_TYPE_DUMMY, NULL);

    /* Ensure NULL virtual methods do not crash the process */
    adg_textual_set_font_dress(ADG_TEXTUAL(dummy), ADG_DRESS_TABLE);
    g_assert_cmpint(adg_textual_get_font_dress(ADG_TEXTUAL(dummy)), ==, ADG_DRESS_UNDEFINED);
    adg_textual_set_text(ADG_TEXTUAL(dummy), "dummy");
    g_assert_null(adg_textual_dup_text(ADG_TEXTUAL(dummy)));

    g_object_unref(dummy);
}

static void
_adg_method_text_changed(void)
{
    AdgDummy *dummy = g_object_new(ADG_TYPE_DUMMY, NULL);

    adg_test_signal(dummy, "text-changed");
    adg_textual_text_changed(ADG_TEXTUAL(dummy), "old text");
    g_assert_true(adg_test_signal_check(TRUE));

    g_object_unref(dummy);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    g_test_add_func("/adg/textual/behavior/misc", _adg_behavior_misc);

    g_test_add_func("/adg/textual/method/text-changed", _adg_method_text_changed);

    return g_test_run();
}
