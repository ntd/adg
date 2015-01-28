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


static void
_adg_test_local_mix(void)
{
    AdgTitleBlock *title_block;
    AdgEntity *entity;

    /* Ensure the title block behaves exaclty like an AdgTable */

    title_block = adg_title_block_new();
    entity = (AdgEntity *) title_block;
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_DISABLED);
    adg_entity_destroy(entity);

    /* Check local mix method overriding */
    title_block = g_object_new(ADG_TYPE_TITLE_BLOCK, "local-mix", ADG_MIX_ANCESTORS_NORMALIZED, NULL);
    entity = (AdgEntity *) title_block;
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_ANCESTORS_NORMALIZED);
    adg_entity_destroy(entity);

    /* Check default mix using GObject methods */
    title_block = g_object_new(ADG_TYPE_TITLE_BLOCK, NULL);
    entity = (AdgEntity *) title_block;
    g_assert_cmpint(adg_entity_get_local_mix(entity), ==, ADG_MIX_DISABLED);
    adg_entity_destroy(entity);
}

static void
_adg_test_author(void)
{
    AdgTitleBlock *title_block;
    const gchar *valid_text, *latin1_text;
    const gchar *author;
    gchar *author_dup;

    title_block = adg_title_block_new();
    valid_text = "This is some text...";
    latin1_text = "This is some àèìòù Latin1 text...";

    /* Using the public APIs */
    adg_title_block_set_author(title_block, valid_text);
    author = adg_title_block_get_author(title_block);
    g_assert_cmpstr(author, ==, valid_text);

    adg_title_block_set_author(title_block, latin1_text);
    author = adg_title_block_get_author(title_block);
    g_assert_cmpstr(author, ==, latin1_text);

    adg_title_block_set_author(title_block, NULL);
    author = adg_title_block_get_author(title_block);
    g_assert_null(author);

    /* Using GObject property methods */
    g_object_set(title_block, "author", valid_text, NULL);
    g_object_get(title_block, "author", &author_dup, NULL);
    g_assert_cmpstr(author_dup, ==, valid_text);
    g_free(author_dup);

    g_object_set(title_block, "author", latin1_text, NULL);
    g_object_get(title_block, "author", &author_dup, NULL);
    g_assert_cmpstr(author_dup, ==, latin1_text);
    g_free(author_dup);

    g_object_set(title_block, "author", NULL, NULL);
    g_object_get(title_block, "author", &author_dup, NULL);
    g_assert_null(author_dup);

    adg_entity_destroy(ADG_ENTITY(title_block));
}

static void
_adg_test_date(void)
{
    AdgTitleBlock *title_block;
    const gchar *valid_text, *latin1_text;
    const gchar *date;
    gchar *date_dup;

    title_block = adg_title_block_new();
    valid_text = "This is some text...";
    latin1_text = "This is some àèìòù Latin1 text...";

    /* Using the public APIs */
    adg_title_block_set_date(title_block, valid_text);
    date = adg_title_block_get_date(title_block);
    g_assert_cmpstr(date, ==, valid_text);

    adg_title_block_set_date(title_block, latin1_text);
    date = adg_title_block_get_date(title_block);
    g_assert_cmpstr(date, ==, latin1_text);

    adg_title_block_set_date(title_block, NULL);
    date = adg_title_block_get_date(title_block);
    g_assert_nonnull(date);

    /* Using GObject property methods */
    g_object_set(title_block, "date", valid_text, NULL);
    g_object_get(title_block, "date", &date_dup, NULL);
    g_assert_cmpstr(date_dup, ==, valid_text);
    g_free(date_dup);

    g_object_set(title_block, "date", latin1_text, NULL);
    g_object_get(title_block, "date", &date_dup, NULL);
    g_assert_cmpstr(date_dup, ==, latin1_text);
    g_free(date_dup);

    g_object_set(title_block, "date", NULL, NULL);
    g_object_get(title_block, "date", &date_dup, NULL);
    g_assert_nonnull(date_dup);
    g_free(date_dup);

    adg_entity_destroy(ADG_ENTITY(title_block));
}

static void
_adg_test_drawing(void)
{
    AdgTitleBlock *title_block;
    const gchar *valid_text, *latin1_text;
    const gchar *drawing;
    gchar *drawing_dup;

    title_block = adg_title_block_new();
    valid_text = "This is some text...";
    latin1_text = "This is some àèìòù Latin1 text...";

    /* Using the public APIs */
    adg_title_block_set_drawing(title_block, valid_text);
    drawing = adg_title_block_get_drawing(title_block);
    g_assert_cmpstr(drawing, ==, valid_text);

    adg_title_block_set_drawing(title_block, latin1_text);
    drawing = adg_title_block_get_drawing(title_block);
    g_assert_cmpstr(drawing, ==, latin1_text);

    adg_title_block_set_drawing(title_block, NULL);
    drawing = adg_title_block_get_drawing(title_block);
    g_assert_null(drawing);

    /* Using GObject property methods */
    g_object_set(title_block, "drawing", valid_text, NULL);
    g_object_get(title_block, "drawing", &drawing_dup, NULL);
    g_assert_cmpstr(drawing_dup, ==, valid_text);
    g_free(drawing_dup);

    g_object_set(title_block, "drawing", latin1_text, NULL);
    g_object_get(title_block, "drawing", &drawing_dup, NULL);
    g_assert_cmpstr(drawing_dup, ==, latin1_text);
    g_free(drawing_dup);

    g_object_set(title_block, "drawing", NULL, NULL);
    g_object_get(title_block, "drawing", &drawing_dup, NULL);
    g_assert_null(drawing_dup);

    adg_entity_destroy(ADG_ENTITY(title_block));
}

static void
_adg_test_logo(void)
{
    AdgTitleBlock *title_block;
    AdgEntity *valid_entity, *invalid_entity, *logo;

    title_block = adg_title_block_new();
    valid_entity = ADG_ENTITY(adg_hatch_new(NULL));
    invalid_entity = adg_test_invalid_pointer();

    /* Prevent valid_entity from being destroyed
     * the first time AdgTitleBlock:logo is set to NULL */
    g_object_ref(valid_entity);

    /* Using the public APIs */
    adg_title_block_set_logo(title_block, valid_entity);
    logo = adg_title_block_logo(title_block);
    g_assert_true(logo == valid_entity);

    adg_title_block_set_logo(title_block, invalid_entity);
    logo = adg_title_block_logo(title_block);
    g_assert_true(logo == valid_entity);

    adg_title_block_set_logo(title_block, valid_entity);
    logo = adg_title_block_logo(title_block);
    g_assert_true(logo == valid_entity);

    adg_title_block_set_logo(title_block, NULL);
    logo = adg_title_block_logo(title_block);
    g_assert_null(logo);

    /* Using GObject property methods */
    g_object_set(title_block, "logo", valid_entity, NULL);
    g_object_get(title_block, "logo", &logo, NULL);
    g_assert_true(logo == valid_entity);
    adg_entity_destroy(logo);

    g_object_set(title_block, "logo", invalid_entity, NULL);
    g_object_get(title_block, "logo", &logo, NULL);
    g_assert_true(logo == valid_entity);
    adg_entity_destroy(logo);

    g_object_set(title_block, "logo", valid_entity, NULL);
    g_object_get(title_block, "logo", &logo, NULL);
    g_assert_true(logo == valid_entity);
    adg_entity_destroy(logo);

    g_object_set(title_block, "logo", NULL, NULL);
    g_object_get(title_block, "logo", &logo, NULL);
    g_assert_null(logo);

    adg_entity_destroy(ADG_ENTITY(title_block));
    g_object_unref(valid_entity);
}

static void
_adg_test_projection(void)
{
    AdgTitleBlock *title_block;
    AdgEntity *valid_entity, *invalid_entity, *projection;

    title_block = adg_title_block_new();
    valid_entity = ADG_ENTITY(adg_hatch_new(NULL));
    invalid_entity = adg_test_invalid_pointer();

    /* Prevent valid_entity from being destroyed
     * the first time AdgTitleBlock:projection is set to NULL */
    g_object_ref(valid_entity);

    /* Using the public APIs */
    adg_title_block_set_projection(title_block, valid_entity);
    projection = adg_title_block_projection(title_block);
    g_assert_true(projection == valid_entity);

    adg_title_block_set_projection(title_block, invalid_entity);
    projection = adg_title_block_projection(title_block);
    g_assert_true(projection == valid_entity);

    adg_title_block_set_projection(title_block, NULL);
    projection = adg_title_block_projection(title_block);
    g_assert_null(projection);

    /* Using GObject property methods */
    g_object_set(title_block, "projection", valid_entity, NULL);
    g_object_get(title_block, "projection", &projection, NULL);
    g_assert_true(projection == valid_entity);
    adg_entity_destroy(projection);

    g_object_set(title_block, "projection", invalid_entity, NULL);
    g_object_get(title_block, "projection", &projection, NULL);
    g_assert_true(projection == valid_entity);
    adg_entity_destroy(projection);

    g_object_set(title_block, "projection", NULL, NULL);
    g_object_get(title_block, "projection", &projection, NULL);
    g_assert_null(projection);

    adg_entity_destroy(ADG_ENTITY(title_block));
    g_object_unref(valid_entity);
}

static void
_adg_test_scale(void)
{
    AdgTitleBlock *title_block;
    const gchar *valid_text, *latin1_text;
    const gchar *scale;
    gchar *scale_dup;

    title_block = adg_title_block_new();
    valid_text = "This is some text...";
    latin1_text = "This is some àèìòù Latin1 text...";

    /* Using the public APIs */
    adg_title_block_set_scale(title_block, valid_text);
    scale = adg_title_block_get_scale(title_block);
    g_assert_cmpstr(scale, ==, valid_text);

    adg_title_block_set_scale(title_block, latin1_text);
    scale = adg_title_block_get_scale(title_block);
    g_assert_cmpstr(scale, ==, latin1_text);

    adg_title_block_set_scale(title_block, NULL);
    scale = adg_title_block_get_scale(title_block);
    g_assert_null(scale);

    /* Using GObject property methods */
    g_object_set(title_block, "scale", valid_text, NULL);
    g_object_get(title_block, "scale", &scale_dup, NULL);
    g_assert_cmpstr(scale_dup, ==, valid_text);
    g_free(scale_dup);

    g_object_set(title_block, "scale", latin1_text, NULL);
    g_object_get(title_block, "scale", &scale_dup, NULL);
    g_assert_cmpstr(scale_dup, ==, latin1_text);
    g_free(scale_dup);

    g_object_set(title_block, "scale", NULL, NULL);
    g_object_get(title_block, "scale", &scale_dup, NULL);
    g_assert_null(scale_dup);

    adg_entity_destroy(ADG_ENTITY(title_block));
}

static void
_adg_test_size(void)
{
    AdgTitleBlock *title_block;
    const gchar *valid_text, *latin1_text;
    const gchar *size;
    gchar *size_dup;

    title_block = adg_title_block_new();
    valid_text = "This is some text...";
    latin1_text = "This is some àèìòù Latin1 text...";

    /* Using the public APIs */
    adg_title_block_set_size(title_block, valid_text);
    size = adg_title_block_get_size(title_block);
    g_assert_cmpstr(size, ==, valid_text);

    adg_title_block_set_size(title_block, latin1_text);
    size = adg_title_block_get_size(title_block);
    g_assert_cmpstr(size, ==, latin1_text);

    adg_title_block_set_size(title_block, NULL);
    size = adg_title_block_get_size(title_block);
    g_assert_null(size);

    /* Using GObject property methods */
    g_object_set(title_block, "size", valid_text, NULL);
    g_object_get(title_block, "size", &size_dup, NULL);
    g_assert_cmpstr(size_dup, ==, valid_text);
    g_free(size_dup);

    g_object_set(title_block, "size", latin1_text, NULL);
    g_object_get(title_block, "size", &size_dup, NULL);
    g_assert_cmpstr(size_dup, ==, latin1_text);
    g_free(size_dup);

    g_object_set(title_block, "size", NULL, NULL);
    g_object_get(title_block, "size", &size_dup, NULL);
    g_assert_null(size_dup);

    adg_entity_destroy(ADG_ENTITY(title_block));
}

static void
_adg_test_title(void)
{
    AdgTitleBlock *title_block;
    const gchar *valid_text, *latin1_text;
    const gchar *title;
    gchar *title_dup;

    title_block = adg_title_block_new();
    valid_text = "This is some text...";
    latin1_text = "This is some àèìòù Latin1 text...";

    /* Using the public APIs */
    adg_title_block_set_title(title_block, valid_text);
    title = adg_title_block_get_title(title_block);
    g_assert_cmpstr(title, ==, valid_text);

    adg_title_block_set_title(title_block, latin1_text);
    title = adg_title_block_get_title(title_block);
    g_assert_cmpstr(title, ==, latin1_text);

    adg_title_block_set_title(title_block, NULL);
    title = adg_title_block_get_title(title_block);
    g_assert_null(title);

    /* Using GObject property methods */
    g_object_set(title_block, "title", valid_text, NULL);
    g_object_get(title_block, "title", &title_dup, NULL);
    g_assert_cmpstr(title_dup, ==, valid_text);
    g_free(title_dup);

    g_object_set(title_block, "title", latin1_text, NULL);
    g_object_get(title_block, "title", &title_dup, NULL);
    g_assert_cmpstr(title_dup, ==, latin1_text);
    g_free(title_dup);

    g_object_set(title_block, "title", NULL, NULL);
    g_object_get(title_block, "title", &title_dup, NULL);
    g_assert_null(title_dup);

    adg_entity_destroy(ADG_ENTITY(title_block));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/title-block/type/object", ADG_TYPE_TITLE_BLOCK);
    adg_test_add_entity_checks("/adg/title-block/type/entity", ADG_TYPE_TITLE_BLOCK);

    adg_test_add_func("/adg/title-block/property/local-mix", _adg_test_local_mix);
    adg_test_add_func("/adg/title-block/property/author", _adg_test_author);
    adg_test_add_func("/adg/title-block/property/date", _adg_test_date);
    adg_test_add_func("/adg/title-block/property/drawing", _adg_test_drawing);
    adg_test_add_func("/adg/title-block/property/logo", _adg_test_logo);
    adg_test_add_func("/adg/title-block/property/projection", _adg_test_projection);
    adg_test_add_func("/adg/title-block/property/scale", _adg_test_scale);
    adg_test_add_func("/adg/title-block/property/size", _adg_test_size);
    adg_test_add_func("/adg/title-block/property/title", _adg_test_title);

    return g_test_run();
}
