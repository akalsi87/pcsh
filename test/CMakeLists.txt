# TEST: sub module

add_test_exe    (tpcshver tpcshver.cpp)
test_link_libs  (tpcshver libpcsh)
create_test     (tpcshver)

add_test_exe    (tarena tarena.cpp)
test_link_libs  (tarena libpcsh)
create_test     (tarena)

add_test_exe    (tparser tparser.cpp)
test_link_libs  (tparser libpcsh)
create_test     (tparser)
