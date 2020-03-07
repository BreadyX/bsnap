src = ['./main.c',
	   './snap/snap-action.c', # debug
	   # TODO
	  ]

subdir('./cmd')

# Configure config.h
config_h = configuration_data()
config_h.set('name', meson.project_name())
config_h.set('version', meson.project_version())
configure_file(input: 'config.h.in',
				output: 'config.h',
				configuration: config_h)

# Setup includes
config_include = include_directories('.')
includes = [config_include]

#Compile
bsnap = executable(meson.project_name(), sources: src, include_directories: includes)

install_data(bsnap, install_dir: bin_dir, install_mode: 'rwxr-xr-x')
