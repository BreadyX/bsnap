project('bsnap', 'c', version: '0.0.1')

src = []
includes = [include_directories('include')]

subdir('include')
subdir('src')

# Compile
bsnap = executable(meson.project_name(),
				   sources: src,
				   include_directories: includes,
				   install: true,
				   install_dir: get_option('bindir'))

subdir('tests')
