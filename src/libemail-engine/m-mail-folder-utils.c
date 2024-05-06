#include "config.h"

#include "m-mail-folder-utils.h"

#include <glib/gi18n-lib.h>

#include <libedataserver/libedataserver.h>

typedef struct _AsyncContext AsyncContext;

struct _AsyncContext {
	CamelMimeMessage *message;
	CamelMessageInfo *info;
	CamelMimePart *part;
	GHashTable *hash_table;
	GPtrArray *ptr_array;
	GFile *destination;
	gchar *orig_subject;
	gchar *message_uid;
};

static void
async_context_free (AsyncContext *context)
{
	if (context->hash_table != NULL)
		g_hash_table_unref (context->hash_table);

	if (context->ptr_array != NULL)
		g_ptr_array_unref (context->ptr_array);

	g_clear_object (&context->message);
	g_clear_object (&context->info);
	g_clear_object (&context->part);
	g_clear_object (&context->destination);

	g_free (context->orig_subject);
	g_free (context->message_uid);

	g_slice_free (AsyncContext, context);
}

static void
mail_folder_save_messages_thread (GSimpleAsyncResult *simple,
                                  GObject *object,
                                  GCancellable *cancellable)
{
	AsyncContext *context;
	GError *error = NULL;

	context = g_simple_async_result_get_op_res_gpointer (simple);

	m_mail_folder_save_messages_sync (
		CAMEL_FOLDER (object), context->ptr_array,
		context->destination, cancellable, &error);

	if (error != NULL)
		g_simple_async_result_take_error (simple, error);
}

/* Helper for m_mail_folder_save_messages_sync() */
static void
mail_folder_save_prepare_part (CamelMimePart *mime_part)
{
	CamelDataWrapper *content;

	content = camel_medium_get_content (CAMEL_MEDIUM (mime_part));

	if (content == NULL)
		return;

	if (CAMEL_IS_MULTIPART (content)) {
		guint n_parts, ii;

		n_parts = camel_multipart_get_number (
			CAMEL_MULTIPART (content));
		for (ii = 0; ii < n_parts; ii++) {
			mime_part = camel_multipart_get_part (
				CAMEL_MULTIPART (content), ii);
			mail_folder_save_prepare_part (mime_part);
		}

	} else if (CAMEL_IS_MIME_MESSAGE (content)) {
		mail_folder_save_prepare_part (CAMEL_MIME_PART (content));

	} else {
		CamelContentType *type;

		/* Save textual parts as 8-bit, not encoded. */
		type = camel_data_wrapper_get_mime_type_field (content);
		if (camel_content_type_is (type, "text", "*"))
			camel_mime_part_set_encoding (
				mime_part, CAMEL_TRANSFER_ENCODING_8BIT);
	}
}

gboolean
m_mail_folder_save_messages_sync (CamelFolder *folder,
                                  GPtrArray *message_uids,
                                  GFile *destination,
                                  GCancellable *cancellable,
                                  GError **error)
{
	GFileOutputStream *file_output_stream;
	CamelStream *base_stream = NULL;
	GByteArray *byte_array;
	gboolean success = TRUE;
	guint ii;

	g_return_val_if_fail (CAMEL_IS_FOLDER (folder), FALSE);
	g_return_val_if_fail (message_uids != NULL, FALSE);
	g_return_val_if_fail (G_IS_FILE (destination), FALSE);

	/* Need at least one message UID to save. */
	g_return_val_if_fail (message_uids->len > 0, FALSE);

	camel_operation_push_message (
		cancellable, ngettext (
			"Saving %d message",
			"Saving %d messages",
			message_uids->len),
		message_uids->len);

	byte_array = g_byte_array_new ();

	for (ii = 0; ii < message_uids->len; ii++) {
		GFile *message_file;
		CamelMimeMessage *message;
		CamelMimeFilter *filter;
		CamelStream *stream;
		const gchar *uid;
		gchar *from_line;
		gint percent;
		gint retval;

		// TODO: Change message_file name and use maildir.
		message_file = g_file_get_child (destination, "tmp_message_file_name.txt");

		file_output_stream = g_file_replace (
			message_file, NULL, FALSE,
			G_FILE_CREATE_PRIVATE |
			G_FILE_CREATE_REPLACE_DESTINATION,
			cancellable, error);

		if (file_output_stream == NULL) {
			camel_operation_pop_message (cancellable);
			return FALSE;
		}


		if (base_stream != NULL)
			g_object_unref (base_stream);

		/* CamelStreamMem does NOT take ownership of the byte
		 * array when set with camel_stream_mem_set_byte_array().
		 * This allows us to reuse the same memory slab for each
		 * message, which is slightly more efficient. */
		base_stream = camel_stream_mem_new ();
		camel_stream_mem_set_byte_array (
			CAMEL_STREAM_MEM (base_stream), byte_array);

		uid = g_ptr_array_index (message_uids, ii);

		message = camel_folder_get_message_sync (
			folder, uid, cancellable, error);
		if (message == NULL) {
			success = FALSE;
			goto exit;
		}

		mail_folder_save_prepare_part (CAMEL_MIME_PART (message));

		from_line = camel_mime_message_build_mbox_from (message);
		g_return_val_if_fail (from_line != NULL, FALSE);

		success = g_output_stream_write_all (
			G_OUTPUT_STREAM (file_output_stream),
			from_line, strlen (from_line), NULL,
			cancellable, error);

		g_free (from_line);

		if (!success) {
			g_object_unref (message);
			goto exit;
		}

		filter = camel_mime_filter_from_new ();
		stream = camel_stream_filter_new (base_stream);
		camel_stream_filter_add (CAMEL_STREAM_FILTER (stream), filter);

		retval = camel_data_wrapper_write_to_stream_sync (
			CAMEL_DATA_WRAPPER (message),
			stream, cancellable, error);

		g_object_unref (filter);
		g_object_unref (stream);

		if (retval == -1) {
			g_object_unref (message);
			goto exit;
		}

		g_byte_array_append (byte_array, (guint8 *) "\n", 1);

		success = g_output_stream_write_all (
			G_OUTPUT_STREAM (file_output_stream),
			byte_array->data, byte_array->len,
			NULL, cancellable, error);

		if (!success) {
			g_object_unref (message);
			goto exit;
		}

		percent = ((ii + 1) * 100) / message_uids->len;
		camel_operation_progress (cancellable, percent);

		/* Reset the byte array for the next message. */
		g_byte_array_set_size (byte_array, 0);

		g_object_unref (message);
	}

exit:
	if (base_stream != NULL)
		g_object_unref (base_stream);

	g_byte_array_free (byte_array, TRUE);

	g_object_unref (file_output_stream);

	camel_operation_pop_message (cancellable);

	if (!success) {
		/* Try deleting the destination file. */
		g_file_delete (destination, NULL, NULL);
	}

	return success;
}

void
m_mail_folder_save_messages_in_maildir (CamelFolder *folder,
					GPtrArray *message_uids,
					GFile *destination,
					gint io_priority,
					GCancellable *cancellable,
					GAsyncReadyCallback callback,
					gpointer user_data)
{
	GSimpleAsyncResult *simple;
	AsyncContext *context;

	g_return_if_fail (CAMEL_IS_FOLDER (folder));
	g_return_if_fail (message_uids != NULL);
	g_return_if_fail (G_IS_FILE (destination));

	/* Need at least one message UID to save. */
	g_return_if_fail (message_uids->len > 0);

	context = g_slice_new0 (AsyncContext);
	context->ptr_array = g_ptr_array_ref (message_uids);
	context->destination = g_object_ref (destination);

	simple = g_simple_async_result_new (
		G_OBJECT (folder), callback, user_data,
		m_mail_folder_save_messages_in_maildir);

	g_simple_async_result_set_check_cancellable (simple, cancellable);

	g_simple_async_result_set_op_res_gpointer (
		simple, context, (GDestroyNotify) async_context_free);

	g_simple_async_result_run_in_thread (
		simple, mail_folder_save_messages_thread,
		io_priority, cancellable);

	g_object_unref (simple);
}

gboolean
m_mail_folder_save_messages_finish (CamelFolder *folder,
                                    GAsyncResult *result,
                                    GError **error)
{
	GSimpleAsyncResult *simple;

	g_return_val_if_fail (
		g_simple_async_result_is_valid (
		result, G_OBJECT (folder),
		m_mail_folder_save_messages_in_maildir), FALSE);

	simple = G_SIMPLE_ASYNC_RESULT (result);

	/* Assume success unless a GError is set. */
	return !g_simple_async_result_propagate_error (simple, error);
}
