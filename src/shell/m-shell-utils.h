#ifndef M_SHELL_UTILS_H
#define M_SHELL_UTILS_H

#include <shell/e-shell.h>

G_BEGIN_DECLS

typedef void (* EShellOepnSaveCustomizeFunc)	(GtkFileChooserNative *file_chooser_native,
						 gpointer user_data);

GFile *		m_shell_run_save_dialog		(EShell *shell,
						 const gchar *title,
						 const gchar *suggestion,
						 const gchar *filters,
						 EShellOepnSaveCustomizeFunc customize_func,
						 gpointer customize_data);

G_END_DECLS

#endif /* M_SHELL_UTILS_H */
