#ifndef M_MAIL_FOLDER_UTILS_H
#define M_MAIL_FOLDER_UTILS_H

/* CamelFolder wrappers with Evolution-specific policies. */

#include <camel/camel.h>

G_BEGIN_DECLS

gboolean	m_mail_folder_save_messages_sync
						(CamelFolder *folder,
						 GPtrArray *message_uids,
						 GFile *destination,
						 GCancellable *cancellable,
						 GError **error);
void		m_mail_folder_save_messages	(CamelFolder *folder,
						 GPtrArray *message_uids,
						 GFile *destination,
						 gint io_priority,
						 GCancellable *cancellable,
						 GAsyncReadyCallback callback,
						 gpointer user_data);
gboolean	m_mail_folder_save_messages_finish
						(CamelFolder *folder,
						 GAsyncResult *result,
						 GError **error);

G_END_DECLS

#endif /* M_MAIL_FOLDER_UTILS_H */
