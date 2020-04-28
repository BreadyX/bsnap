test_includes = [include_directories('..' / 'src')]

tests = {
  'blist':      [['test-list.c'], []],
  'blist_iter': [['test-list-iter.c'], []],
  'bmap':       [['test-map.c'], []],
  'bmap_iter':  [['test-map-iter.c'], []],
  'parse_ini':  [['test-parse-ini.c'], ['./test-data/ini.conf']],
}

foreach test_case, test_data : tests
  test_exe = executable(test_case.underscorify(),
						c_args: ['-g'],
                        sources: test_data[0],
                        include_directories: includes + test_includes)
  test(test_case, test_exe, args: test_data[1])
endforeach

