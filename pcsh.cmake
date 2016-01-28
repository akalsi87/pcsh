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
    ${pcsh_inc}/version.hpp;
    ${pcsh_inc}/parser.hpp)

# internal
set(pcsh_int_hdr
    )

# -- Sources
set(pcsh_src
    ${pcsh_src}/version.cpp;
    ${pcsh_src}/parser.cpp)

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
