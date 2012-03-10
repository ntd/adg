/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011  Nicola Fontana <ntd at entidi.it>
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
 * SECTION:adg-textual
 * @short_description: An interface for entities that embeds text
 *
 * The #AdgTextual interface should be implemented by entities that
 * can be expressed by text. #AdgToyText is a typical case but also
 * more complex entities could implement it.
 *
 * Since: 1.0
 **/

/**
 * AdgTextual:
 *
 * Dummy type of the #AdgTextualIface interface.
 *
 * Since: 1.0
 **/

/**
 * AdgTextualIface:
 * @set_font_dress: abstract virtual method to change the font dress.
 * @get_font_dress: abstract virtual method to get the active font dress.
 * @set_text:       abstract virtual method to set a new text.
 * @dup_text:       abstract virtual method that returns a duplicate of
 *                  the actual text.
 * @text_changed:   default signal handler for #AdgTextual::text-changed.
 *
 * The virtual methods @set_text and @dup_text must be implemented
 * by all the types which implement this interface.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include "adg-dress.h"
#include "adg-dress-builtins.h"

#include "adg-textual.h"


enum {
    TEXT_CHANGED,
    LAST_SIGNAL
};


static void     _adg_iface_base         (AdgTextualIface *iface);
static gchar *  _adg_dup_text           (AdgTextual      *textual,
                                         AdgTextualIface *iface);
static guint    _adg_signals[LAST_SIGNAL] = { 0 };


GType
adg_textual_get_type(void)
{
    static GType textual_type = 0;

    if (G_UNLIKELY(textual_type == 0)) {
        const GTypeInfo textual_info = {
            sizeof(AdgTextualIface),
            (GBaseInitFunc)      _adg_iface_base,
            (GBaseFinalizeFunc)  NULL,
            (GClassInitFunc)     NULL,
            (GClassFinalizeFunc) NULL,
        };
        textual_type = g_type_register_static(G_TYPE_INTERFACE, "AdgTextual",
                                              &textual_info, 0);
        g_type_interface_add_prerequisite(ADG_TYPE_TEXTUAL, G_TYPE_OBJECT);
    }

    return textual_type;
}

static void
_adg_iface_base(AdgTextualIface *iface)
{
    static gboolean initialized = FALSE;
    GParamSpec *param;

    if (G_LIKELY(initialized))
        return;

    initialized = TRUE;

    param = g_param_spec_string("text",
                                P_("Text"),
                                P_("The text associated to this entity"),
                                NULL,
                                G_PARAM_READWRITE);
    g_object_interface_install_property(iface, param);

    param = adg_param_spec_dress("font-dress",
                                 P_("Font Dress"),
                                 P_("The font dress to use for rendering this text"),
                                 ADG_DRESS_FONT_TEXT,
                                 G_PARAM_READWRITE);
    g_object_interface_install_property(iface, param);

    /**
     * AdgTextual::text-changed:
     * @textual: an entity that implements #AdgTextual
     * @old_text: the old text
     *
     * Emitted whenever the text of @textual has been changed.
     *
     * Since: 1.0
     **/
    _adg_signals[TEXT_CHANGED] =
        g_signal_new("text-changed",
                     G_OBJECT_CLASS_TYPE(iface),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(AdgTextualIface, text_changed),
                     NULL, NULL,
                     adg_marshal_VOID__POINTER,
                     G_TYPE_NONE, 1, G_TYPE_POINTER);
}


/**
 * adg_textual_set_font_dress:
 * @textual: an object that implements #AdgTextual
 * @dress: the new dress
 *
 * Sets the font dress of @textual to @dress.
 *
 * Since: 1.0
 **/
void
adg_textual_set_font_dress(AdgTextual *textual, AdgDress dress)
{
    AdgTextualIface *iface;

    g_return_if_fail(ADG_IS_TEXTUAL(textual));

    /* The set_font_dress() method must be defined */
    iface = ADG_TEXTUAL_GET_IFACE(textual);
    if (iface->set_font_dress == NULL) {
        g_warning(_("%s: `set_font_dress' method not implemented for type `%s'"),
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(textual)));
        return;
    }

    iface->set_font_dress(textual, dress);
}

/**
 * adg_textual_get_font_dress:
 * @textual: an object that implements #AdgTextual
 *
 * Gets the current font dress of @textual, eventually resolved to
 * an #AdgFontStyle before the rendering.
 *
 * Returns: (transfer none): the current font dress of @textual.
 *
 * Since: 1.0
 **/
AdgDress
adg_textual_get_font_dress(AdgTextual *textual)
{
    AdgTextualIface *iface;

    g_return_val_if_fail(ADG_IS_TEXTUAL(textual), ADG_DRESS_UNDEFINED);

    /* The get_font_dress() method must be defined */
    iface = ADG_TEXTUAL_GET_IFACE(textual);
    if (iface->get_font_dress == NULL) {
        g_warning(_("%s: `get_font_dress' method not implemented for type `%s'"),
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(textual)));
        return ADG_DRESS_UNDEFINED;
    }

    return iface->get_font_dress(textual);
}

/**
 * adg_textual_set_text:
 * @textual: an object that implements #AdgTextual
 * @text: the new text to be set
 *
 * Sets a new text on @textual. If @text is the same as the old text
 * no actions are performed, otherwise the set_text() virtual method
 * is called and the #AdgTextual::text-changed signal is emitted.
 *
 * Since: 1.0
 **/
void
adg_textual_set_text(AdgTextual *textual, const gchar *text)
{
    AdgTextualIface *iface;
    gchar *old_text;

    g_return_if_fail(ADG_IS_TEXTUAL(textual));

    /* The set_text() method must be defined */
    iface = ADG_TEXTUAL_GET_IFACE(textual);
    if (iface->set_text == NULL) {
        g_warning(_("%s: `set_text' method not implemented for type `%s'"),
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(textual)));
        return;
    }

    old_text = _adg_dup_text(textual, iface);
    if (g_strcmp0(text, old_text) != 0) {
        iface->set_text(textual, text);
        g_signal_emit(textual, _adg_signals[TEXT_CHANGED], 0, old_text);
    }

    g_free(old_text);
}

/**
 * adg_textual_dup_text:
 * @textual: an object that implements #AdgTextual
 *
 * Gets a duplicate of the current text bound to @textual.
 *
 * Returns: the current text of @textual: free with g_free() when finished
 *
 * Since: 1.0
 **/
gchar *
adg_textual_dup_text(AdgTextual *textual)
{
    g_return_val_if_fail(ADG_IS_TEXTUAL(textual), NULL);

    return _adg_dup_text(textual, ADG_TEXTUAL_GET_IFACE(textual));
}

/**
 * adg_textual_text_changed:
 * @textual: an object that implements #AdgTextual
 * @old_text: the old text bound to @textual
 *
 * Emits the #AdgTextual::text-changed signal on @textual.
 *
 * <note><para>
 * This function is only useful when creating a new class that implements
 * the #AdgTextual interface.
 * </para></note>
 *
 * Since: 1.0
 **/
void
adg_textual_text_changed(AdgTextual *textual, const gchar *old_text)
{
    g_return_if_fail(ADG_IS_TEXTUAL(textual));

    g_signal_emit(textual, _adg_signals[TEXT_CHANGED], 0, old_text);
}


static gchar *
_adg_dup_text(AdgTextual *textual, AdgTextualIface *iface)
{
    if (iface->dup_text == NULL) {
        g_warning(_("%s: `dup_text' method not implemented for type `%s'"),
                  G_STRLOC, g_type_name(G_OBJECT_TYPE(textual)));
        return NULL;
    }

    return iface->dup_text(textual);
}
