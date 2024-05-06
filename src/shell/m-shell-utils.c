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
 * m_shell_run_create_dir_dialog:
 * @shell: an #EShell
 * @title: file chooser dialog title
 * @suggestion: file name suggestion, or %NULL
 * @filters: Possible filters for dialog, or %NULL
 * @customize_func: optional dialog customization function
 * @customize_data: optional data to pass to @customize_func
 *
 * Runs a #GtkFileChooserNative in create_folder mode with the given
 * title and returns the selected #GFile.  If @customize_func is
 * provided, the function is called just prior to running the dialog.
 * If the user cancels the dialog the function will return %NULL.
 *
 * Returns: the #GFile to save to, or %NULL
 **/
GFile *
m_shell_run_create_dir_dialog (EShell *shell,
			       const gchar *title,
			       const gchar *suggestion,
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
		GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
		_("_Create"), _("_Cancel"));

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
