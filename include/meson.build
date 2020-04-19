# Configure config.h
config_h = configuration_data()
config_h.set('name', meson.project_name())
config_h.set('version', meson.project_version())
configure_file(input: 'config.h.in',
  output: 'config.h',
  configuration: config_h)
