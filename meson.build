project('evolution-offline-store', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

add_project_arguments('-DHAVE_CONFIG_H', language:'c')
add_project_arguments('-I.', language:'c')

# Dependecies so far:
evolutionshell        = dependency('evolution-shell-3.0',       version: '>=3.6.4')
gtk                   = dependency('gtk+-3.0',                  version: '>=3.6.0')
glib                  = dependency('glib-2.0')
evolutionmail         = dependency('evolution-mail-3.0',        version: '>=3.6.0')
libemailengine        = dependency('libemail-engine',           version: '>=3.6.0')

# Directories
LIB_INSTALL_DIR      = join_paths(get_option('prefix'), 'lib')
SHARE_INSTALL_PREFIX = join_paths(get_option('prefix'), 'share')
plugindir = get_option('plugin-install-dir')

if plugindir == ''
   	plugindir = evolutionshell.get_variable(pkgconfig: 'plugindir', default_value: '/usr/lib/evolution/plugins')
endif

if plugindir == ''
	plugindir = join_paths(evolutionshell.get_variable(pkgconfig: 'privlibdir'), 'plugins')
endif


moduledir = get_option('module-install-dir')

if moduledir == ''
   	moduledir = evolutionshell.get_variable(pkgconfig: 'moduledir', default_value: '/usr/lib/evolution/modules')
endif

if moduledir == ''
	moduledir = join_paths(evolutionshell.get_variable(pkgconfig: 'privlibdir'), 'modules')
endif






debugoption = get_option('debugbuild')
if debugoption == true
	add_project_arguments('-DDEBUG', language:'c')
endif

# Generate the config.h file
conf_data = configuration_data()

# Main project information
conf_data.set_quoted('PROJECT_NAME', meson.project_name())
conf_data.set('VERSION', meson.project_version())

# We dont want deprecated functions from evolution data server
conf_data.set('EDS_DISABLE_DEPRECATED', true)

# We dont use the old version anyway. If you do, good luck.
evoversion = evolutionshell.version()
evoversion = evoversion.replace('.','')
evoversion_int = evoversion.to_int()
evoversion_int = evoversion_int * 10

conf_data.set('EVOLUTION_VERSION', evoversion_int)

conf_data.set('PLUGIN_INSTALL_DIR', plugindir)
conf_data.set_quoted('GETTEXT_PACKAGE', meson.project_name())
conf_data.set('LOCALEDIR', join_paths(SHARE_INSTALL_PREFIX, 'locale'))

configure_file(
	output: 'config.h',
	configuration: conf_data,
)

subdir('src')
