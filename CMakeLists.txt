cmake_minimum_required(VERSION 4.1)
project(olympia-tag C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# =============================================================================
# Compiler-flag conventions (read before "tidying up" this file)
# =============================================================================
# The 32->64-bit modernization ladder is complete. Phases 1-10 were triaged
# to ZERO and locked as errors (the full ladder); none remain suppressed
# (Phase 6 = shorten-64-to-32 + sizeof-pointer-memaccess; Phase 7 =
# sign-conversion; Phase 8 = return-type + return-mismatch; Phase 9 = the full
# format class, incl. format-security; Phase 10 = implicit-int-conversion).
# See CLAUDE.md "Modernization status" and doc/modernization-64bit-plan.md.
#
# 1. Enforced classes are written as an explicit pair, one class per line:
#        -Wfoo -Werror=foo
#    The bare -Wfoo is technically redundant (-Werror=foo already enables it),
#    but the pair is deliberate: it records that the class was driven to zero
#    and locked. Do NOT collapse these to bare -Werror=foo.
#
# 2. -Wno-foo suppressions are legacy idioms this ~80K-line codebase still
#    relies on (no ladder class remains suppressed).
#
# The shared warning set is applied to the single olympia-tag target via
# olympia_compile_flags(), with optional sanitizers via
# olympia_enable_sanitizers() (the asan-ubsan preset sets OLYMPIA_SANITIZE=ON).
# Optimization level is owned by the build type / preset (debug -> -O0 -g,
# release -> -O3 -DNDEBUG, asan-ubsan -> -O1 -g via its preset override), NOT
# pinned per target.
# =============================================================================

option(OLYMPIA_SANITIZE "Enable sanitizer instrumentation" OFF)
set(OLYMPIA_SANITIZERS "address,undefined" CACHE STRING "Comma-separated sanitizers, e.g. address,undefined")

# All shared engine warning flags, applied to the olympia-tag target.
# Optimization is owned by the build type / preset (see the conventions block).
function(olympia_compile_flags tgt)
    target_compile_options(${tgt} PRIVATE
        # --- Suppressions: legacy idioms left as-is ---
        -Wno-comment
        -Wno-compare-distinct-pointer-types
        -Wno-dangling-else
        -Wno-deprecated-declarations
        -Wno-deprecated-non-prototype
        -Wno-extra-tokens
        -Wno-incompatible-library-redeclaration
        -Wno-logical-not-parentheses
        -Wno-macro-redefined
        -Wno-multichar
        -Wno-non-literal-null-conversion
        -Wno-parentheses
        -Wno-tautological-constant-out-of-range-compare

        # --- Modernization classes triaged to 0 and locked (CLAUDE.md) ---
        -Wint-to-pointer-cast -Werror=int-to-pointer-cast                # Phase 1
        -Wpointer-to-int-cast -Werror=pointer-to-int-cast                # Phase 1
        -Wincompatible-pointer-types -Werror=incompatible-pointer-types  # Phase 2
        -Wint-conversion -Werror=int-conversion                          # Phase 3
        -Wstrict-prototypes -Werror=strict-prototypes                    # Phase 4
        -Wmissing-prototypes -Werror=missing-prototypes                  # Phase 4
        -Wimplicit-function-declaration -Werror=implicit-function-declaration  # Phase 4
        -Wmissing-declarations -Werror=missing-declarations              # Phase 5
        # A sizeof(ptr) where sizeof(*ptr) is meant grows 4->8 bytes on LP64
        # (a real corruption risk). TAG never had G3's MD5 rnd.c and its
        # bzero(ctx, sizeof(ctx)) defect (the engine RNG was an inlined
        # drand48, now PCG32 in lib/prng.c), so this class was already 0;
        # locked at zero. Portable (GCC understands it too).
        -Wsizeof-pointer-memaccess -Werror=sizeof-pointer-memaccess      # Phase 6 (#17)
        # Implicit signed<->unsigned conversions: a negative int flowing into a
        # size_t arg is a real hazard. All sites were counts/lengths provably
        # >= 0, cast explicitly to (size_t). Portable (GCC understands it too).
        -Wsign-conversion -Werror=sign-conversion                        # Phase 7 (#18)
        # A non-void function that falls off the end leaves the caller reading a
        # garbage register -- on LP64 an 8-byte garbage long/pointer (the class
        # behind G2's `st -32` flicker). Fall-off is only a *warning* by default
        # on both Clang and GCC, so lock it. Portable (GCC understands it too).
        -Wreturn-type -Werror=return-type                                # Phase 8 (#19)
        # The format / vararg class. A bare %s/%d with no data argument reads a
        # garbage stack pointer (and %s then derefs it); a width mismatch
        # (%d <- long/size_t) silently truncates or corrupts on LP64; a
        # non-literal format string lets %-bearing data read the stack. The
        # printf wrappers carry __attribute__((format(printf,...))), so every
        # call site is checkable. Locks the FULL class: under Clang -Wformat
        # pulls in -Wformat-security, so dropping -Wno-format-security too leaves
        # no sub-suppression (the g2 model). Portable (GCC understands it too).
        -Wformat -Werror=format                                          # Phase 9 (#12)
        # Post-64-bit warning-ratchet promotion (not a ladder phase): a signed
        # char passed to a ctype macro (tolower etc.) indexes a table at a
        # negative offset on high-bit bytes -- latent OOB / UB. All sites cast
        # to (unsigned char); driven to zero and locked. Portable (GCC too).
        -Wchar-subscripts -Werror=char-subscripts                        # ratchet (#27)
        # A runtime (non-literal) printf-family format string. The security
        # case: player-controlled format = format-string injection (the EMOTE
        # bug, #25). Not part of -Wformat on Clang or GCC, so requested
        # explicitly. Driven to zero (#24): legitimate dynamic formats built
        # from trusted constants are pragma-annotated in-place; forwarded-format
        # wrappers carry __attribute__((format(printf,...))). Portable (GCC too).
        -Wformat-nonliteral -Werror=format-nonliteral                    # ratchet (#24/#25)
    )

    # Clang-only diagnostic spellings. GCC either folds each into a broader
    # class (-Wconversion, not enabled here) or enforces it by default, and
    # treats an unknown positive -W* as fatal -- so these must stay guarded to
    # keep GCC building.
    if (CMAKE_C_COMPILER_ID MATCHES "Clang")
        target_compile_options(${tgt} PRIVATE
            # The LP64 width-truncation class (long/size_t/ssize_t into int).
            -Wshorten-64-to-32 -Werror=shorten-64-to-32                  # Phase 6 (#17)
            # A bare `return;` in a non-void fn. Clang splits this out of
            # -Wreturn-type, so Clang needs it explicitly. GCC enforces the class
            # WITHOUT this flag (default-on permerror on >=14; folded into
            # -Wreturn-type on <=13, where the option was not yet split out and an
            # unknown positive -W* aborts the compile) -- so keep it Clang-only to
            # keep GCC <=13 building. No coverage lost.
            -Wreturn-mismatch -Werror=return-mismatch                    # Phase 8 (#19)
            # int narrowed into a short/schar/char entity field. Representation-preserving
            # (identical on ILP32/LP64), so a code-quality lock, not a 64-bit hazard. Made
            # explicit with (short)/(schar)/(char) casts. Clang-only spelling (GCC folds it
            # into -Wconversion); keep guarded so GCC keeps building.
            -Wimplicit-int-conversion -Werror=implicit-int-conversion        # Phase 10 (#20)
        )
    endif()
endfunction()

function(olympia_enable_sanitizers tgt)
    if(NOT OLYMPIA_SANITIZE)
        return()
    endif()
    if(NOT OLYMPIA_SANITIZERS)
        message(FATAL_ERROR "OLYMPIA_SANITIZE is ON but OLYMPIA_SANITIZERS is empty")
    endif()

    # Sanitizers are supported on Clang/GCC; MSVC uses different flags.
    if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        # Compile and Link flags must match!
        target_compile_options(${tgt} PRIVATE
            -fsanitize=${OLYMPIA_SANITIZERS}
            -fno-omit-frame-pointer
            -fno-optimize-sibling-calls
            -fno-sanitize-recover=all
            -fno-common
        )
        target_link_options(${tgt} PRIVATE
            -fsanitize=${OLYMPIA_SANITIZERS}
            -fno-omit-frame-pointer
            -fno-optimize-sibling-calls
            -fno-sanitize-recover=all
            -fno-common
        )
        if (CMAKE_C_COMPILER_ID MATCHES "Clang")
            # -fsanitize-address-use-after-scope is Clang-only
            target_compile_options(${tgt} PRIVATE
                -fsanitize-address-use-after-scope
            )
            target_link_options(${tgt} PRIVATE
                -fsanitize-address-use-after-scope
            )
        endif()
    else()
        message(WARNING "OLYMPIA_SANITIZE is ON but compiler is not Clang/GNU; sanitizers not enabled.")
    endif()
endfunction()

# ------------------------------------------------------------------------------
# 32-bit build support
# ------------------------------------------------------------------------------
# Enable 32-bit builds on 64-bit Linux systems for generating golden test files
# from the original legacy code before 64-bit refactoring.
#
# Usage:
#   mkdir build32 && cd build32
#   cmake -DBUILD_32BIT=ON ..
#   cmake --build .
#
option(BUILD_32BIT "Build 32-bit binaries (requires gcc-multilib on Linux)" OFF)

if (BUILD_32BIT)
    message(STATUS "32-bit build enabled (-g -m32)")
    add_compile_options(-g -m32)
    add_link_options(-g -m32)
endif()

# Platform-specific defines
if (LINUX)
    add_compile_definitions(OLYMPIA_CC_LINUX)
endif ()
if (WIN32)
    add_compile_definitions(OLYMPIA_CC_WINDOWS)
endif ()

# ------------------------------------------------------------------------------
# TAG island + mapgen (vendored from olympia-g3 @ 7b510b4, issue #3)
# ------------------------------------------------------------------------------
# Both draw from the shared PCG32 rnd() stream in lib/prng.c + lib/pcg.c
# (issue #31), seeded from a `randseed` file (load_seed/save_seed). The engine
# links the same two files, seeded from lib/system instead.
add_executable(island-tag)
target_sources(island-tag PRIVATE
    island/island.c lib/prng.c lib/pcg.c
)
target_include_directories(island-tag PRIVATE island/ lib/)
# Declare the C11 requirement at the target level (the project-level
# CMAKE_C_STANDARD above stays as the default for any future targets).
target_compile_features(island-tag PRIVATE c_std_11)
olympia_compile_flags(island-tag)

# Optional ASan/UBSan via the asan-ubsan preset (OLYMPIA_SANITIZE=ON).
olympia_enable_sanitizers(island-tag)

add_executable(mapgen-tag)
target_sources(mapgen-tag PRIVATE
    mapgen/mapgen.c mapgen/z.c
    lib/ilist.c lib/roads.c lib/tiles.c
    lib/checked_alloc.c lib/prng.c lib/pcg.c
)
target_sources(mapgen-tag PRIVATE
    FILE_SET HEADERS
    BASE_DIRS
        mapgen lib
    FILES
        mapgen/z.h
        lib/checked_alloc.h
        lib/prng.h
        lib/pcg.h
        lib/lists.h
)
target_include_directories(mapgen-tag PRIVATE mapgen/ lib/)
# Declare the C11 requirement at the target level (the project-level
# CMAKE_C_STANDARD above stays as the default for any future targets).
target_compile_features(mapgen-tag PRIVATE c_std_11)
olympia_compile_flags(mapgen-tag)

# Optional ASan/UBSan via the asan-ubsan preset (OLYMPIA_SANITIZE=ON).
olympia_enable_sanitizers(mapgen-tag)

# ------------------------------------------------------------------------------
# PCG32 known-answer test (lib/pcg.c, copied from olympia-g3 genesis/pcg.c)
# ------------------------------------------------------------------------------
# lib/pcg.c backs rnd() in every program (via lib/prng.c, issue #31); this
# target holds the generator itself to the reference vector and to G3's genesis
# output. Run via tests/rng/check.sh.
add_executable(pcg-check-tag)
target_sources(pcg-check-tag PRIVATE
    tests/rng/pcg_check.c lib/pcg.c
)
target_sources(pcg-check-tag PRIVATE
    FILE_SET HEADERS
    BASE_DIRS
        lib
    FILES
        lib/pcg.h
)
target_include_directories(pcg-check-tag PRIVATE lib/)
target_compile_features(pcg-check-tag PRIVATE c_std_11)
olympia_compile_flags(pcg-check-tag)

# Optional ASan/UBSan via the asan-ubsan preset (OLYMPIA_SANITIZE=ON).
olympia_enable_sanitizers(pcg-check-tag)

# ------------------------------------------------------------------------------
# TAG olympia (main game engine)
# ------------------------------------------------------------------------------
add_executable(olympia-tag)
target_sources(olympia-tag
    PRIVATE
        olympia/add.c olympia/adv.c olympia/alchem.c olympia/art.c
        olympia/artifacts.c olympia/basic.c olympia/beast.c olympia/build.c
        olympia/buy.c olympia/c1.c olympia/c2.c olympia/check.c olympia/cloud.c
        olympia/code.c olympia/combat.c olympia/day.c olympia/dir.c
        olympia/display.c olympia/eat.c olympia/effect.c olympia/faery.c
        olympia/garr.c olympia/gate.c olympia/glob.c olympia/gm.c olympia/hades.c
        olympia/hero.c olympia/immed.c olympia/input.c olympia/io.c
        olympia/kill.c olympia/loc.c olympia/lore.c olympia/main.c olympia/make.c
        olympia/map.c olympia/mining.c olympia/move.c olympia/necro.c
        olympia/npc.c olympia/order.c olympia/perm.c olympia/produce.c
        olympia/quest.c olympia/ranger.c olympia/relig.c olympia/report.c
        olympia/savage.c olympia/scry.c olympia/seed.c olympia/shipcraft.c
        olympia/sout.c olympia/stack.c olympia/stealth.c olympia/storm.c
        olympia/summary.c olympia/swear.c olympia/times.c olympia/trading.c
        olympia/u.c olympia/use.c olympia/win.c
        olympia/z.c
        lib/checked_alloc.c
        lib/accept_ents.c lib/admits.c lib/cstrings.c lib/effects.c
        lib/entity_builds.c lib/exit_views.c lib/fights.c lib/flag_ents.c
        lib/item_ents.c lib/ilist.c lib/orders.c lib/req_ents.c
        lib/roads.c lib/skill_ents.c lib/tiles.c lib/trades.c lib/wait_args.c
        lib/prng.c lib/pcg.c
)
target_sources(olympia-tag
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS
        olympia lib
    FILES
        olympia/code.h olympia/combat.h olympia/dir.h olympia/display.h
        olympia/eat.h olympia/effect.h olympia/etc.h olympia/garr.h
        olympia/kill.h olympia/legacy.h olympia/loc.h olympia/loop.h
        olympia/oly.h olympia/oly3.h olympia/order.h olympia/quest.h
        olympia/relig.h olympia/sout.h olympia/stack.h olympia/swear.h
        olympia/u.h olympia/use.h olympia/z.h
        lib/checked_alloc.h lib/lists.h lib/pcg.h lib/prng.h
)
target_include_directories(olympia-tag PRIVATE olympia/ lib/)
# Declare the C11 requirement at the target level (the project-level
# CMAKE_C_STANDARD above stays as the default for any future targets).
target_compile_features(olympia-tag PRIVATE c_std_11)
# Engine-specific defines (optimization/-g is owned by the build type / preset).
target_compile_options(olympia-tag PRIVATE -DLINUX)
olympia_compile_flags(olympia-tag)

# Optional ASan/UBSan via the asan-ubsan preset (OLYMPIA_SANITIZE=ON).
olympia_enable_sanitizers(olympia-tag)
