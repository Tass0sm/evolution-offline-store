#ifndef SAVE_AS_MAILDIR_EXTENSION_H
#define SAVE_AS_MAILDIR_EXTENSION_H

#include <glib-object.h>
#include <libebackend/libebackend.h>
#include <gtk/gtk.h>

/* Standard GObject macros */
#define M_TYPE_SHELL_VIEW_EXTENSION \
	(m_shell_view_extension_get_type ())
#define M_SHELL_VIEW_EXTENSION(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST \
	((obj), M_TYPE_SHELL_VIEW_EXTENSION, MShellViewExtension))
#define M_SHELL_VIEW_EXTENSION_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_CAST \
	((cls), M_TYPE_SHELL_VIEW_EXTENSION, MShellViewExtensionClass))
#define M_IS_SHELL_VIEW_EXTENSION(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE \
	((obj), M_TYPE_SHELL_VIEW_EXTENSION))
#define M_IS_SHELL_VIEW_EXTENSION_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_TYPE \
	((cls), M_TYPE_SHELL_VIEW_EXTENSION))
#define M_SHELL_VIEW_EXTENSION_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS \
	((obj), M_TYPE_SHELL_VIEW_EXTENSION, MShellViewExtensionClass))

G_BEGIN_DECLS

typedef struct _MShellViewExtension MShellViewExtension;
typedef struct _MShellViewExtensionClass MShellViewExtensionClass;
typedef struct _MShellViewExtensionPrivate MShellViewExtensionPrivate;

struct _MShellViewExtension
{
	EExtension parent;

	MShellViewExtensionPrivate *priv;
};

struct _MShellViewExtensionClass
{
	EExtensionClass parent;
};

GType	m_shell_view_extension_get_type		(void);
void	m_shell_view_extension_type_register	(GTypeModule *type_module);

G_END_DECLS

#endif /* SAVE_AS_MAILDIR_EXTENSION_H */
