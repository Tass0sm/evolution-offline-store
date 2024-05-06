/* Miscellaneous utility functions used by EMailReader actions. */

#include "config.h"

#include "m-mail-reader-utils.h"

#include <glib/gi18n.h>
#include <libxml/tree.h>
#include <camel/camel.h>

#include <shell/e-shell-utils.h>
#include "../shell/m-shell-utils.h"

#include <libemail-engine/libemail-engine.h>
#include "../libemail-engine/m-mail-folder-utils.h"

#include <em-format/e-mail-parser.h>
#include <em-format/e-mail-part-utils.h>

#include <composer/e-composer-actions.h>

#include <mail/e-mail-backend.h>
#include <mail/e-mail-browser.h>
#include <mail/e-mail-printer.h>
#include <mail/e-mail-display.h>
#include <mail/em-composer-utils.h>
#include <mail/em-utils.h>
#include <mail/mail-autofilter.h>
#include <mail/mail-vfolder-ui.h>
#include <mail/message-list.h>

#define d(x)

typedef struct _AsyncContext AsyncContext;

struct _AsyncContext {
	EActivity *activity;
	CamelFolder *folder;
	CamelMimeMessage *message;
	EMailPartList *part_list;
	EMailReader *reader;
	CamelInternetAddress *address;
	GPtrArray *uids;
	gchar *folder_name;
	gchar *message_uid;

	EMailReplyType reply_type;
	EMailReplyStyle reply_style;
	EMailForwardStyle forward_style;
	GtkPrintOperationAction print_action;
	const gchar *filter_source;
	gint filter_type;
	gboolean replace;
	gboolean keep_signature;
};

static void
async_context_free (AsyncContext *async_context)
{
	g_clear_object (&async_context->activity);
	g_clear_object (&async_context->folder);
	g_clear_object (&async_context->message);
	g_clear_object (&async_context->part_list);
	g_clear_object (&async_context->reader);
	g_clear_object (&async_context->address);

	if (async_context->uids != NULL)
		g_ptr_array_unref (async_context->uids);

	g_free (async_context->folder_name);
	g_free (async_context->message_uid);

	g_slice_free (AsyncContext, async_context);
}

static void
mail_reader_save_messages_cb (GObject *source_object,
                              GAsyncResult *result,
                              gpointer user_data)
{
	EActivity *activity;
	EAlertSink *alert_sink;
	AsyncContext *async_context;
	GError *local_error = NULL;

	async_context = (AsyncContext *) user_data;

	activity = async_context->activity;
	alert_sink = e_activity_get_alert_sink (activity);

	m_mail_folder_save_messages_finish (
		CAMEL_FOLDER (source_object), result, &local_error);

	if (e_activity_handle_cancellation (activity, local_error)) {
		g_error_free (local_error);

	} else if (local_error != NULL) {
		e_alert_submit (
			alert_sink,
			"mail:save-messages",
			local_error->message, NULL);
		g_error_free (local_error);
	}

	async_context_free (async_context);
}

void
m_mail_reader_save_messages (EMailReader *reader)
{
	EShell *shell;
	EActivity *activity;
	EMailBackend *backend;
	GCancellable *cancellable;
	AsyncContext *async_context;
	EShellBackend *shell_backend;
	CamelMessageInfo *info;
	CamelFolder *folder;
	GFile *destination;
	GPtrArray *uids;
	const gchar *message_uid;
	const gchar *title;
	gchar *suggestion = NULL;

	folder = e_mail_reader_ref_folder (reader);
	backend = e_mail_reader_get_backend (reader);

	uids = e_mail_reader_get_selected_uids (reader);
	g_return_if_fail (uids != NULL && uids->len > 0);

	if (uids->len > 1) {
		GtkWidget *message_list;

		message_list = e_mail_reader_get_message_list (reader);
		message_list_sort_uids (MESSAGE_LIST (message_list), uids);
	}

	message_uid = g_ptr_array_index (uids, 0);

	title = ngettext ("Save Message", "Save Messages", uids->len);

	/* Suggest as a filename the subject of the first message. */
	info = camel_folder_get_message_info (folder, message_uid);
	if (info != NULL) {
		const gchar *subject;

		subject = camel_message_info_get_subject (info);
		if (subject != NULL)
			suggestion = g_strconcat (subject, ".maildir", NULL);
		g_clear_object (&info);
	}

	if (suggestion == NULL) {
		const gchar *basename;

		/* Translators: This is part of a suggested file name
		 * used when saving a message or multiple messages to
		 * mbox format, when the first message doesn't have a
		 * subject.  The extension ".mbox" is appended to the
		 * string; for example "Message.mbox". */
		basename = ngettext ("Message", "Messages", uids->len);
		suggestion = g_strconcat (basename, ".mbox", NULL);
	}

	shell_backend = E_SHELL_BACKEND (backend);
	shell = e_shell_backend_get_shell (shell_backend);

	destination = m_shell_run_create_dir_dialog (
		shell, title, suggestion, NULL, NULL);

	if (destination == NULL)
		goto exit;

	/* Save messages asynchronously. */

	activity = e_mail_reader_new_activity (reader);
	cancellable = e_activity_get_cancellable (activity);

	async_context = g_slice_new0 (AsyncContext);
	async_context->activity = g_object_ref (activity);
	async_context->reader = g_object_ref (reader);

	m_mail_folder_save_messages_in_maildir (
		folder, uids,
		destination,
		G_PRIORITY_DEFAULT,
		cancellable,
		mail_reader_save_messages_cb,
		async_context);

	g_object_unref (activity);

	g_object_unref (destination);

exit:
	g_clear_object (&folder);
	g_ptr_array_unref (uids);
}
