# TEST: sub module

add_test_exe    (tpcshver tpcshver.cpp)
test_link_libs  (tpcshver pcsh)
create_test     (tpcshver)

add_test_exe    (tarena tarena.cpp)
test_link_libs  (tarena pcsh)
create_test     (tarena)

add_test_exe    (tparser tparser.cpp)
test_link_libs  (tparser pcsh)
create_test     (tparser)
