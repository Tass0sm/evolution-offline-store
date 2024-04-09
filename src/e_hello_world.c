#include "e_hello_world.h"

struct _EHelloWorld {
  EExtension parent;
};

struct _EHelloWorldClass {
  EExtensionClass parent_class;
};

G_DEFINE_DYNAMIC_TYPE(EHelloWorld, e_hello_world, E_TYPE_EXTENSION)

static void
e_hello_world_constructed (GObject *object)
{
  EExtensible *extensible;

  /* This retrieves the EShell instance we're extending. */
  extensible = e_extension_get_extensible (E_EXTENSION (object));

  /* This prints "Hello world from EShell!" */
  g_print ("Hello world from %s!\n", G_OBJECT_TYPE_NAME (extensible));
}

static void
e_hello_world_finalize (GObject *object)
{
  g_print ("Goodbye cruel world!\n");

  /* Chain up to parent's finalize() method. */
  G_OBJECT_CLASS (e_hello_world_parent_class)->finalize (object);
}

static void
e_hello_world_class_init (EHelloWorldClass *class)
{
  GObjectClass *object_class;
  EExtensionClass *extension_class;

  object_class = G_OBJECT_CLASS (class);
  object_class->constructed = e_hello_world_constructed;
  object_class->finalize = e_hello_world_finalize;

  /* Specify the GType of the class we're extending.
   * The class must implement the EExtensible interface. */
  extension_class = E_EXTENSION_CLASS (class);
  extension_class->extensible_type = E_TYPE_SHELL;
}

static void
e_hello_world_class_finalize (EHelloWorldClass *class)
{
  /* This function is usually left empty. */
}

static void
e_hello_world_init (EHelloWorld *extension)
{
  /* The EShell object we're extending is not available yet,
   * but we could still do some early initialization here. */
}

void
e_hello_world_type_register (GTypeModule *type_module)
{
	e_hello_world_register_type (type_module);
}
