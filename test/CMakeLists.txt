# TEST: sub module

add_test_exe    (tpcshver ./tpcshver.cpp)
test_link_libs  (tpcshver pcsh)
create_test     (tpcshver)
