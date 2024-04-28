/**
 * SECTION: m-shell-utils
 * @short_description: high-level utilities with shell integration
 * @include: shell/m-shell-utils.h
 **/

#include "config.h"

#include <glib/gi18n-lib.h>

#include <libedataserver/libedataserver.h>

#include <shell/e-shell-view.h>
#include <shell/e-shell-window.h>
#include <shell/e-shell-utils.h>
#include "m-shell-utils.h"

/**
 * m_shell_run_save_dialog:
 * @shell: an #EShell
 * @title: file chooser dialog title
 * @suggestion: file name suggestion, or %NULL
 * @filters: Possible filters for dialog, or %NULL
 * @customize_func: optional dialog customization function
 * @customize_data: optional data to pass to @customize_func
 *
 * Runs a #GtkFileChooserNative in save mode with the given title and
 * returns the selected #GFile.  If @customize_func is provided, the
 * function is called just prior to running the dialog.  If the user
 * cancels the dialog the function will return %NULL.
 *
 * With non-%NULL @filters will be added also file filters to the dialog.
 * The string format is "pat1:mt1;pat2:mt2:...", where 'pat' is a pattern
 * and 'mt' is a MIME type for the pattern to be used.  There can be more
 * than one MIME type, those are separated by comma.
 *
 * Returns: the #GFile to save to, or %NULL
 **/
GFile *
m_shell_run_save_dialog (EShell *shell,
                         const gchar *title,
                         const gchar *suggestion,
                         const gchar *filters,
                         EShellOepnSaveCustomizeFunc customize_func,
                         gpointer customize_data)
{
	GtkFileChooser *file_chooser;
	GFile *chosen_file = NULL;
	GtkFileChooserNative *native;
	GtkWindow *parent;

	g_return_val_if_fail (E_IS_SHELL (shell), NULL);

	parent = e_shell_get_active_window (shell);

	native = gtk_file_chooser_native_new (
		title, parent,
		GTK_FILE_CHOOSER_ACTION_SAVE,
		_("_Save"), _("_Cancel"));

	file_chooser = GTK_FILE_CHOOSER (native);

	gtk_file_chooser_set_local_only (file_chooser, FALSE);
	gtk_file_chooser_set_do_overwrite_confirmation (file_chooser, TRUE);

	if (suggestion != NULL) {
		gchar *current_name;

		current_name = g_strdup (suggestion);
		e_util_make_safe_filename (current_name);
		gtk_file_chooser_set_current_name (file_chooser, current_name);
		g_free (current_name);
	}

	if (filters != NULL) {
		gchar **flts = g_strsplit (filters, ";", -1);
		gint i;

		for (i = 0; flts && flts[i]; i++) {
			GtkFileFilter *filter = gtk_file_filter_new ();
			gchar *flt = flts[i];
			gchar *delim = strchr (flt, ':'), *next = NULL;

			if (delim) {
				*delim = 0;
				next = strchr (delim + 1, ',');
			}

			gtk_file_filter_add_pattern (filter, flt);
			if (g_ascii_strcasecmp (flt, "*.mbox") == 0)
				gtk_file_filter_set_name (
					filter, _("Berkeley Mailbox (mbox)"));
			else if (g_ascii_strcasecmp (flt, "*.vcf") == 0)
				gtk_file_filter_set_name (
					filter, _("vCard (.vcf)"));
			else if (g_ascii_strcasecmp (flt, "*.ics") == 0)
				gtk_file_filter_set_name (
					filter, _("iCalendar (.ics)"));
			else
				gtk_file_filter_set_name (filter, flt);

			while (delim) {
				delim++;
				if (next)
					*next = 0;

				gtk_file_filter_add_mime_type (filter, delim);

				delim = next;
				if (next)
					next = strchr (next + 1, ',');
			}

			gtk_file_chooser_add_filter (file_chooser, filter);
		}

		if (flts && flts[0]) {
			GtkFileFilter *filter = gtk_file_filter_new ();

			gtk_file_filter_add_pattern (filter, "*");
			gtk_file_filter_set_name (filter, _("All Files (*)"));
			gtk_file_chooser_add_filter (file_chooser, filter);
		}

		g_strfreev (flts);
	}

	e_util_load_file_chooser_folder (file_chooser);

	/* Allow further customizations before running the dialog. */
	if (customize_func != NULL)
		customize_func (native, customize_data);

	if (gtk_native_dialog_run (GTK_NATIVE_DIALOG (native)) == GTK_RESPONSE_ACCEPT) {
		e_util_save_file_chooser_folder (file_chooser);

		chosen_file = gtk_file_chooser_get_file (file_chooser);
	}

	g_object_unref (native);

	return chosen_file;
}
