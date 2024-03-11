#include <shell/e-shell.h>
#include <libebackend/libebackend.h>

typedef struct _EHelloWorld EHelloWorld;
typedef struct _EHelloWorldClass EHelloWorldClass;

struct _EHelloWorld;
struct _EHelloWorldClass;

GType e_hello_world_get_type (void);
void  e_hello_world_type_register (GTypeModule *type_module);
