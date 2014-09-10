/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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


/**
 * SECTION:adg-param-dress
 * @Section_Id:AdgParamDress
 * @title: AdgParamDress
 * @short_description: Metadata for dress specification
 *
 * The %ADG_TYPE_PARAM_DRESS type is a parameter specification
 * compatible with %G_TYPE_PARAM_ENUM that provides additional
 * validation: it rejects values that are incompatibles (that
 * is, that are not related) with the current one. Check the
 * adg_dress_are_related() documentation for details on what
 * <emphasis>related</emphasis> means.
 *
 * Internally, the value setting is performed by calling the
 * adg_dress_set() API.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-dress.h"

#include "adg-param-dress.h"


typedef struct _AdgParamSpecDress AdgParamSpecDress;

struct _AdgParamSpecDress {
    GParamSpecEnum parent;
    AdgDress       source_dress;
};


static void     _adg_param_dress_init           (GParamSpec   *pspec);
static void     _adg_param_dress_set_default    (GParamSpec   *pspec,
                                                 GValue       *value);
static gboolean _adg_param_dress_validate       (GParamSpec   *pspec,
                                                 GValue       *value);
static gint     _adg_param_dress_cmp            (GParamSpec   *pspec,
                                                 const GValue *value1,
                                                 const GValue *value2);


GType
adg_param_dress_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        /* const */ GParamSpecTypeInfo pspec_info = {
            sizeof(AdgParamSpecDress),    /* instance_size */
            0,                            /* n_preallocs */
            _adg_param_dress_init,
            G_TYPE_INVALID,               /* value_type */
            NULL,                         /* finalize */
            _adg_param_dress_set_default,
            _adg_param_dress_validate,
            _adg_param_dress_cmp,
        };
        pspec_info.value_type = ADG_TYPE_DRESS;
        type = g_param_type_register_static(g_intern_static_string("AdgParamDress"),
                                            &pspec_info);
    }

    return type;
}


/**
 * adg_param_spec_dress:
 * @name:  canonical name
 * @nick:  nickname of the param
 * @blurb: brief desciption
 * @dress: the #AdgDress dress
 * @flags: a combination of #GParamFlags
 *
 * Creates a param spec to hold a dress value. This is similar to
 * g_param_spec_enum() but rejects a new dress value if it is not
 * related with the old one. The setting is performed via
 * adg_dress_set(), so check its documentation for details.
 *
 * Returns: (transfer full): the newly allocated #GParamSpec.
 *
 * Since: 1.0
 **/
GParamSpec *
adg_param_spec_dress(const gchar *name, const gchar *nick, const gchar *blurb,
                     AdgDress dress, GParamFlags flags)
{
    AdgParamSpecDress *dspec;

    dspec = g_param_spec_internal(ADG_TYPE_PARAM_DRESS,
                                  name, nick, blurb, flags);
    dspec->source_dress = dress;

    return (GParamSpec *) dspec;
}


static void
_adg_param_dress_init(GParamSpec *pspec)
{
    AdgParamSpecDress *dspec = ADG_PARAM_SPEC_DRESS(pspec);
    dspec->source_dress = ADG_DRESS_UNDEFINED;
}

static void
_adg_param_dress_set_default(GParamSpec *pspec,
                             GValue     *value)
{
    value->data[0].v_long = ADG_PARAM_SPEC_DRESS(pspec)->source_dress;
}

static gboolean
_adg_param_dress_validate(GParamSpec *pspec, GValue *value)
{
    AdgParamSpecDress *dspec;
    AdgDress *dress;
    AdgDress wanted_dress;

    dspec = ADG_PARAM_SPEC_DRESS(pspec);
    dress = (AdgDress *) &value->data[0].v_long;
    wanted_dress = *dress;

    /* Fallback to the source dress, returned in case of errors */
    *dress = dspec->source_dress;

    /* This method will fail (that is, it leaves *dress untouched)
     * if the current *dress value (source_dress) and wanted_dress
     * are not related */
    adg_dress_set(dress, wanted_dress);

    return *dress != wanted_dress;
}

static gint
_adg_param_dress_cmp(GParamSpec   *pspec,
                     const GValue *value1,
                     const GValue *value2)
{
    glong v1 = value1->data[0].v_long;
    glong v2 = value2->data[0].v_long;

    return v1 < v2 ? -1 : (v1 > v2);
}
