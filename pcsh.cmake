# library : pcsh
# -- Version
set(pcsh_maj_ver 0)
set(pcsh_min_ver 0)
set(pcsh_pat_ver 1)

set(pcsh_lib_ver
    "${pcsh_maj_ver}.${pcsh_min_ver}.${pcsh_pat_ver}")

set(pcsh_lib_compat_ver
    "${pcsh_maj_ver}.${pcsh_min_ver}")


set(pcsh_inc include/pcsh)
set(pcsh_src src/pcsh)

# -- Headers
# export
set(pcsh_exp_hdr
    ${pcsh_inc}/assert.hpp;
    ${pcsh_inc}/arena.hpp;
    ${pcsh_inc}/ir.hpp;
    ${pcsh_inc}/noncopyable.hpp;
    ${pcsh_inc}/ostream.hpp;
    ${pcsh_inc}/parser.hpp;
    ${pcsh_inc}/types.hpp;
    ${pcsh_inc}/version.hpp;
)

# internal
set(pcsh_int_hdr
    ${pcsh_src}/ir_nodes.hpp;
    ${pcsh_src}/ir_nodes_fwd.hpp;
    ${pcsh_src}/ir_printer.hpp;
    ${pcsh_src}/ir_visitor.hpp;
    ${pcsh_src}/populate_symbol_table.hpp;
    ${pcsh_src}/symbol_table.hpp;
)

# -- Sources
set(pcsh_src
    ${pcsh_src}/assert.cpp;
    ${pcsh_src}/arena.cpp;
    ${pcsh_src}/ir.cpp;
    ${pcsh_src}/ir_printer.cpp;
    ${pcsh_src}/ir_visitor.cpp;
    ${pcsh_src}/parser.cpp;
    ${pcsh_src}/populate_symbol_table.cpp;
    ${pcsh_src}/symbol_table.cpp;
    ${pcsh_src}/version.cpp;
)

# -- Targets
# --- shared
add_lib(pcsh SHARED ${pcsh_src} ${pcsh_int_hdr} ${pcsh_exp_hdr})
add_lib_build_def(pcsh include/pcsh/exportsym.h PCSH)
add_comp_def(pcsh PCSH_MAJ=${pcsh_maj_ver})
add_comp_def(pcsh PCSH_MIN=${pcsh_min_ver})
add_comp_def(pcsh PCSH_PAT=${pcsh_pat_ver})
link_libs(pcsh )
set_tgt_ver(pcsh ${pcsh_lib_ver} ${pcsh_lib_compat_ver})

# -- Install!
install_hdr(${pcsh_exp_hdr})
install_tgt(pcsh)
