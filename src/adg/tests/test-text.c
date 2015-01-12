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


#include "test-internal.h"


static void
_adg_test_local_mix(void)
{
    AdgText *text;
    AdgEntity *entity;

    /* Check default local mix method */
    text = adg_text_new("");
    entity = (AdgEntity *) text;
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_ANCESTORS_NORMALIZED);
    adg_entity_destroy(entity);

    /* Check local mix method overriding */
    text = g_object_new(ADG_TYPE_TEXT, "local-mix", ADG_MIX_DISABLED, NULL);
    entity = (AdgEntity *) text;
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_DISABLED);
    adg_entity_destroy(entity);

    /* Check default mix using GObject methods */
    text = g_object_new(ADG_TYPE_TEXT, NULL);
    entity = (AdgEntity *) text;
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_ANCESTORS_NORMALIZED);
    adg_entity_destroy(entity);
}

static void
_adg_test_font_dress(void)
{
    AdgText *text;
    AdgTextual *textual;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress font_dress;

    text = adg_text_new(NULL);
    textual = (AdgTextual *) text;
    valid_dress_1 = ADG_DRESS_FONT_QUOTE_ANNOTATION;
    valid_dress_2 = ADG_DRESS_FONT;
    incompatible_dress = ADG_DRESS_LINE;

    /* Using the public APIs */
    adg_textual_set_font_dress(textual, valid_dress_1);
    font_dress = adg_textual_get_font_dress(textual);
    g_assert_cmpint(font_dress, ==, valid_dress_1);

    adg_textual_set_font_dress(textual, incompatible_dress);
    font_dress = adg_textual_get_font_dress(textual);
    g_assert_cmpint(font_dress, ==, valid_dress_1);

    adg_textual_set_font_dress(textual, valid_dress_2);
    font_dress = adg_textual_get_font_dress(textual);
    g_assert_cmpint(font_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(text, "font-dress", valid_dress_1, NULL);
    g_object_get(text, "font-dress", &font_dress, NULL);
    g_assert_cmpint(font_dress, ==, valid_dress_1);

    g_object_set(text, "font-dress", incompatible_dress, NULL);
    g_object_get(text, "font-dress", &font_dress, NULL);
    g_assert_cmpint(font_dress, ==, valid_dress_1);

    g_object_set(text, "font-dress", valid_dress_2, NULL);
    g_object_get(text, "font-dress", &font_dress, NULL);
    g_assert_cmpint(font_dress, ==, valid_dress_2);

    adg_entity_destroy(ADG_ENTITY(text));
}

static void
_adg_test_text(void)
{
    AdgText *text;
    AdgTextual *textual;
    const gchar *valid_text, *latin1_text;
    gchar *string;

    text = adg_text_new(NULL);
    textual = (AdgTextual *) text;
    valid_text = "This is some string...";
    latin1_text = "This is some àèìòù Latin1 string...";

    /* Using the public APIs */
    adg_textual_set_text(textual, valid_text);
    string = adg_textual_dup_text(textual);
    g_assert_cmpstr(string, ==, valid_text);
    g_free(string);

    adg_textual_set_text(textual, latin1_text);
    string = adg_textual_dup_text(textual);
    g_assert_cmpstr(string, ==, latin1_text);
    g_free(string);

    adg_textual_set_text(textual, NULL);
    string = adg_textual_dup_text(textual);
    g_assert(string == NULL);
    g_free(string);

    /* Using GObject property methods */
    g_object_set(text, "text", valid_text, NULL);
    g_object_get(text, "text", &string, NULL);
    g_assert_cmpstr(string, ==, valid_text);
    g_free(string);

    g_object_set(text, "text", latin1_text, NULL);
    g_object_get(text, "text", &string, NULL);
    g_assert_cmpstr(string, ==, latin1_text);
    g_free(string);

    g_object_set(text, "text", NULL, NULL);
    g_object_get(text, "text", &string, NULL);
    g_assert(string == NULL);

    adg_entity_destroy(ADG_ENTITY(text));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/text/local-mix", _adg_test_local_mix);
    adg_test_add_func("/adg/text/font-dress", _adg_test_font_dress);
    adg_test_add_func("/adg/text/string", _adg_test_text);

    return g_test_run();
}
