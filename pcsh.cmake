# library : pcsh
# -- Version
set(pcsh_maj_ver 0)
set(pcsh_min_ver 0)
set(pcsh_pat_ver 1)

set(pcsh_lib_ver
    "${pcsh_maj_ver}.${pcsh_min_ver}.${pcsh_pat_ver}")

set(pcsh_lib_compat_ver
    "${pcsh_maj_ver}.${pcsh_min_ver}")


set(hdr_dir include/pcsh)
set(src_dir src/pcsh)

# -- Headers
# export
set(pcsh_hdr
    ${hdr_dir}/assert.hpp;
    ${hdr_dir}/arena.hpp;
    ${hdr_dir}/ir.hpp;
    ${hdr_dir}/ir_operations.hpp;
    ${hdr_dir}/noncopyable.hpp;
    ${hdr_dir}/ostream.hpp;
    ${hdr_dir}/parser.hpp;
    ${hdr_dir}/result_type.hpp;
    ${hdr_dir}/types.hpp;
    ${hdr_dir}/version.hpp;
)

# internal
set(pcsh_int_hdr
    ${src_dir}/ir/execution/executor.hpp;
    ${src_dir}/ir/nodes.hpp;
    ${src_dir}/ir/nodes_fwd.hpp;
    ${src_dir}/ir/ops/printer.hpp;
    ${src_dir}/ir/ops/tree_cloner.hpp;
    ${src_dir}/ir/ops/variable_printer.hpp;
    ${src_dir}/ir/passes/populate_symbol_table.hpp;
    ${src_dir}/ir/passes/type_checker.hpp;
    ${src_dir}/ir/symbol_table.hpp;
    ${src_dir}/ir/tree_validation.hpp;
    ${src_dir}/ir/visitor.hpp;
)

# -- Sources
set(pcsh_src
    ${src_dir}/assert.cpp;
    ${src_dir}/arena.cpp;
    ${src_dir}/ir/execution/executor.cpp;
    ${src_dir}/ir/operations.cpp;
    ${src_dir}/ir/ops/printer.cpp;
    ${src_dir}/ir/ops/tree_cloner.cpp;
    ${src_dir}/ir/ops/variable_printer.cpp;
    ${src_dir}/ir/visitor.cpp;
    ${src_dir}/ir/passes/populate_symbol_table.cpp;
    ${src_dir}/ir/passes/type_checker.cpp;
    ${src_dir}/ir/symbol_table.cpp;
    ${src_dir}/ir/tree_validation.cpp;
    ${src_dir}/parser.cpp;
    ${src_dir}/version.cpp;
)

# -- Targets
# --- shared
add_lib(libpcsh SHARED ${pcsh_src} ${pcsh_int_hdr} ${pcsh_hdr})
add_lib_build_def(libpcsh include/pcsh/exportsym.h PCSH)
add_inc_dir(libpcsh ${CMAKE_CURRENT_SOURCE_DIR}/${src_dir})
add_comp_def(libpcsh PCSH_MAJ=${pcsh_maj_ver})
add_comp_def(libpcsh PCSH_MIN=${pcsh_min_ver})
add_comp_def(libpcsh PCSH_PAT=${pcsh_pat_ver})
link_libs(libpcsh )
set_tgt_ver(libpcsh ${pcsh_lib_ver} ${pcsh_lib_compat_ver})

set_target_properties(libpcsh PROPERTIES PREFIX "")

# --- main executable
set(pcsh_main_src
    ${src_dir}/main/linebufistream.hpp;
    ${src_dir}/main/main.cpp;
)

add_exe(pcsh ${pcsh_main_src})
link_libs(pcsh libpcsh)

# add fake dependency of check on pcsh so we enforce its build
add_dependencies(check pcsh)

# -- Install!
install_hdr(${pcsh_hdr})
install_tgt(libpcsh)
install_tgt(pcsh)
