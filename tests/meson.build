test_includes = [include_directories('..' / 'src')]

tests = {
  'test blist':      [['test-list.c'], []],
  'test blist_iter': [['test-list-iter.c'], []],
  'test bmap':       [['test-map.c'], []],
  'test bmap_iter':  [['test-map-iter.c'], []],
}

foreach test_case, test_data : tests
  test_exe = executable(test_case.underscorify(),
                        sources: test_data[0],
                        include_directories: includes + test_includes)
  test(test_case, test_exe, args: test_data[1])
endforeach

