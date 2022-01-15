/*  cssed quick search plugin (c) Iago Rubio 2004
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>


#include <gtk/gtk.h>
#include <gmodule.h>
#include <plugin.h>

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

/*
	You can use the gmodule provided functions:
	const gchar* g_module_check_init (GModule *module);
	void g_module_unload  (GModule *module);
	
	for preinitialization and post unload steps
	respectively.
*/

// module entry point. This must return a pointer to the plugin. Imported by
// cssed to load the plugin pointer.
G_MODULE_EXPORT CssedPlugin* init_plugin(void);
// cssed plugin creation function member of the plugin structure.
// Will be called by cssed through the plugin structure.
G_MODULE_EXPORT gboolean load_quicksearch_plugin ( CssedPlugin* );
// cssed plugin cleaning function member of the plugin's structure to destroy 
// the UI. You must free all memory used here.
// Will be called by cssed through the plugin structure.
G_MODULE_EXPORT void clean_quicksearch_plugin ( CssedPlugin* );



// One struct to save all UI to be destroyed on clean_plugin()
typedef struct _plugin_ui {
	GtkWidget* entry;
	GtkWidget* button_search_up;
	GtkWidget* button_search_down;
} PluginUI;

// the plugin structure
G_MODULE_EXPORT CssedPlugin quicksearch_plugin;

// this will return the plugin to the caller
G_MODULE_EXPORT CssedPlugin* init_plugin()
{
	quicksearch_plugin.name = _("Quick search");								// the plugin name	
	quicksearch_plugin.description = _("Adds a search entry to the toolbar"); 	// the plugin description
	quicksearch_plugin.load_plugin = &load_quicksearch_plugin;						// load plugin function, will build the UI
	quicksearch_plugin.clean_plugin = &clean_quicksearch_plugin;					// clean plugin function, will destroy the UI
	quicksearch_plugin.user_data = NULL;										// User data
	quicksearch_plugin.priv = NULL;											// Private data, this is opaque and should be ignored		

	return &quicksearch_plugin;
}

// util
static GtkWidget*
create_single_button_from_stock(gchar *stock_id)
{
  GtkWidget* button;
  GtkWidget* image;

  button = gtk_button_new();
  image = gtk_image_new_from_stock(stock_id,GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image);
  gtk_container_add(GTK_CONTAINER(button),image);

  return button;
}

// callbacks
static void
search_next( GtkWidget* widget, gpointer user_data )
{
	gchar* text;
	
	text = gtk_editable_get_chars( GTK_EDITABLE(user_data), 0, -1);
	if( strlen(text) > 0 )	cssed_plugin_search_next( &quicksearch_plugin, text );
	else cssed_plugin_error_message(_("No text"), _("There is no text to be searched. Please enter a string to be searched"));
	g_free( text );
}

static void
search_prev( GtkWidget* widget, gpointer user_data )
{
	gchar* text;
	
	text= gtk_editable_get_chars( GTK_EDITABLE(user_data), 0, -1);
	if( strlen(text) > 0 )	cssed_plugin_search_prev( &quicksearch_plugin, text );
	else cssed_plugin_error_message(_("No text"), _("There is no text to be searched. Please enter a string to be searched"));
	g_free( text );
}

//  load plugin, build the UI and attach it to cssed. Set callbacks for UI events
G_MODULE_EXPORT gboolean
load_quicksearch_plugin (CssedPlugin* plugin)
{
	static GtkWidget* entry;
	static GtkWidget* button_search_up;
	static GtkWidget* button_search_down;
	
	PluginUI* user_interface;
	
	gtk_set_locale ();
#ifdef ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);
#endif

	user_interface = g_malloc0(sizeof(PluginUI));
	
	entry = gtk_entry_new();
	gtk_widget_show( entry );
	
	button_search_up = create_single_button_from_stock(GTK_STOCK_GO_UP);
	gtk_button_set_relief( GTK_BUTTON(button_search_up), GTK_RELIEF_NONE );
	gtk_widget_show( button_search_up );	
	
	button_search_down = create_single_button_from_stock(GTK_STOCK_GO_DOWN);
	gtk_button_set_relief( GTK_BUTTON(button_search_down), GTK_RELIEF_NONE );
	gtk_widget_show( button_search_down );
	
	// store this to add to user_data on the plugin struct
	// to be destroyed on clean_plugin(). Just a test.
	user_interface->entry = entry;
	user_interface->button_search_up = button_search_up;
	user_interface->button_search_down = button_search_down;
	
	quicksearch_plugin.user_data = user_interface;
	
	if( cssed_plugin_add_widget_to_toolbar(&quicksearch_plugin, entry,_("Enter a text to search"),_("Enter a text to search"))  &&
		cssed_plugin_add_widget_to_toolbar( &quicksearch_plugin, button_search_up,_("Click to search backwards"),_("Click to search backwards")) &&
		cssed_plugin_add_widget_to_toolbar( &quicksearch_plugin, button_search_down,_("Click to search forwards"),_("Click to search forwards")) 
	){
		g_signal_connect ((gpointer) button_search_up, "clicked", G_CALLBACK ( search_prev ), entry);
		g_signal_connect ((gpointer) button_search_down, "clicked",	G_CALLBACK ( search_next ), entry);
	}else{
		cssed_plugin_error_message(_("Cannot add widget to toolbar"), _("There was an error while loading the plugin\nIt may fail at any point, please unload it"));
	}

	return TRUE;
}

// could be used to post UI destroy
/*
void g_module_unload (GModule *module)
{
	g_print(_("** Quick search plugin unloaded\n"));	
}
*/

// to destroy UI and stuff. called by cssed.
G_MODULE_EXPORT void clean_quicksearch_plugin ( CssedPlugin* p )
{
	PluginUI* user_interface;

	user_interface = (PluginUI*) p->user_data;	
	gtk_widget_destroy( user_interface->entry );
	gtk_widget_destroy( user_interface->button_search_up );
	gtk_widget_destroy( user_interface->button_search_down );
	g_free( p->user_data );
}


