#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "save_as_maildir_extension.h"

/* Module Entry Points */
void e_module_load (GTypeModule *type_module);
void e_module_unload (GTypeModule *type_module);

G_MODULE_EXPORT void
e_module_load (GTypeModule *type_module)
{
  /* This registers our EShell extension class with the GObject
   * type system.  An instance of our extension class is paired
   * with each instance of the class we're extending. */
  m_shell_view_extension_type_register (type_module);
}

G_MODULE_EXPORT void
e_module_unload (GTypeModule *type_module)
{
  /* This function is usually left empty. */
}
