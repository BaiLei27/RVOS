%global pkgvers 0
%global scdate0 20260807
%global debug_package %{nil}
%global gcc_mirror   https://mirrors.ustc.edu.cn/gnu/gcc
%global gcc_fallback https://gcc.gnu.org/pub/gcc/releases

Source0: %{gcc_mirror}/gcc-%{version}/gcc-%{version}.tar.xz
# Source0: gcc-%{version}.tar.xz

%global bootstrap 0
%define sysarch unknown-elf
%define targets riscv64

Name:           cross-%{sysarch}-gcc
Version:        16.2.0
Release:        %{scdate0}.%{pkgvers}%{?dist}
Summary:        GNU GCC for cross-compilation to RISC-V bare-metal targets
License:        GPLv2+ and GPLv3+ and LGPLv2+ and BSD
URL:            https://gcc.gnu.org

%define maj_ver %(echo "%{version}" | cut -d'.' -f1)
%define cpulist %(echo "%{targets}" | sed "s|%{_arch}||g")

BuildRequires:  git gcc-c++ flex bison texinfo mc dnf curl
BuildRequires:  zlib-devel libzstd-devel libunwind-devel gmp-devel mpfr-devel libmpc-devel
%if ! %{bootstrap}
BuildRequires:  isl-devel
%endif
%if 0%{?fedora}
BuildRequires:  autogen
%endif

BuildRequires: riscv64-unknown-elf-binutils
%if ! %{bootstrap}
BuildRequires: riscv64-picolibc
Suggests:      riscv64-picolibc
%endif

%global __brp_strip_lto %{nil}
%global __os_install_post /usr/lib/rpm/brp-compress
%global __provides_exclude_from ^(%{_prefix}/.*-%{sysarch}/.*$|%{_prefix}/lib/gcc/.*-%{sysarch}/.*$)
%global __requires_exclude_from ^(%{_prefix}/.*-%{sysarch}/.*$|%{_prefix}/lib/gcc/.*-%{sysarch}/.*$)
%global optflags %(echo "%{optflags} -fPIE"| sed 's|-Werror=format-security||')

%description
This is a Cross Compiling version of GNU GCC, which can be used to
compile for the RISC-V bare-metal platform using picolibc.

%package -n riscv64-%{sysarch}-gcc
Summary: GNU GCC for cross-compilation to riscv64 target
Requires: riscv64-unknown-elf-binutils
%if ! %{bootstrap}
Requires: riscv64-picolibc
Suggests: riscv64-picolibc
%endif
Provides: riscv64-gcc = %{version}

%description -n riscv64-%{sysarch}-gcc
GNU GCC for cross-compilation to riscv64 target with picolibc

%if ! %{bootstrap}
%package -n riscv64-%{sysarch}-gcc-c++
Summary: GNU GCC C++ for cross-compilation to riscv64 target
Requires: riscv64-gcc
Provides: riscv64-gcc-c++ = %{version}

%description -n riscv64-%{sysarch}-gcc-c++
GNU GCC C++ for cross-compilation to riscv64 target with picolibc
%endif

%prep
SRC=%{_sourcedir}/gcc-%{version}.tar.xz
if [ ! -s "$SRC" ]; then
    mkdir -p %{_sourcedir}
    URLS="
%{gcc_mirror}/gcc-%{version}/gcc-%{version}.tar.xz
%{gcc_fallback}/gcc-%{version}/gcc-%{version}.tar.xz
"
    ok=0
    for u in $URLS; do
        echo "+++ Downloading $u"
        if command -v curl >/dev/null 2>&1; then
            curl -fL --retry 3 --connect-timeout 30 -o "$SRC.part" "$u" && ok=1
        elif command -v wget >/dev/null 2>&1; then
            wget -c -O "$SRC.part" "$u" && ok=1
        else
            echo "ERROR: neither curl nor wget available" >&2
            exit 1
        fi
        if [ "$ok" = 1 ]; then
            mv -f "$SRC.part" "$SRC"
            break
        fi
        rm -f "$SRC.part"
    done
    [ "$ok" = 1 ] || { echo "ERROR: failed to download gcc-%{version}.tar.xz" >&2; exit 1; }
fi
%setup -q -n gcc-%{version}

%build
export TMPDIR=%{_builddir}/tmp
mkdir -p "$TMPDIR"

export CFLAGS_FOR_BUILD="$(echo %{optflags} | sed 's/-ffat-lto-objects//') -g0"
export CXXFLAGS_FOR_BUILD="$CFLAGS_FOR_BUILD"

contrib/gcc_update --touch
target=riscv64
echo "Building for [$target]"
mkdir -p build-$target-%{sysarch}
pushd build-$target-%{sysarch}

export CFLAGS_FOR_TARGET="-O3 -mcmodel=medany -ffunction-sections -fdata-sections -funroll-loops"
export CXXFLAGS_FOR_TARGET="-O3 -mcmodel=medany -ffunction-sections -fdata-sections -funroll-loops -fno-exceptions -fno-rtti"

CC="%{__cc} %{optflags} -fno-stack-protector" \
CXX="%{__cxx} %{optflags} -fno-stack-protector" \
../configure  \
    --prefix=%{_prefix} --mandir=%{_mandir} \
    --libdir=%{_prefix}/lib --includedir=%{_includedir} \
    --with-pkgversion="RedHat %{version}-%{release}" \
    --infodir=%{_infodir} --target=$target-%{sysarch} \
    --with-ld=%{_bindir}/$target-%{sysarch}-ld \
    --with-sysroot=%{_prefix}/$target-%{sysarch} \
    --with-build-sysroot=%{_prefix}/$target-%{sysarch} \
    --with-local-prefix=%{_prefix}/$target-%{sysarch} \
    --with-python-dir=share/$target-%{sysarch}/gcc-%{maj_ver}/python \
    --with-gcc-major-version-only \
    --with-zstd \
    --with-system-zlib \
    --with-system-libunwind \
    --with-tune=rocket \
    --with-arch=rv64gc \
    --with-abi=lp64d \
    --with-cmodel=medany \
    --with-isl \
    --enable-strip \
    --enable-tls \
    --enable-lto \
    --enable-plugins \
    --enable-multilib \
    --enable-checking=release \
    --disable-gold \
    --disable-libunwind-exceptions \
    --disable-silent-rules \
    --disable-sjlj-exceptions \
    --disable-libssp \
    --disable-libgomp \
    --disable-nls \
    --disable-threads \
    --disable-libquadmath \
    --disable-libstdcxx-pch \
    --disable-fixincludes \
%if %{bootstrap}
    --with-newlib \
    --enable-shared \
    --enable-languages=c,lto \
%else
    --with-newlib \
    --disable-shared  \
    --enable-languages=c,c++,lto \
%endif
    --with-multilib-generator="rv32imafc-ilp32f--;rv32imafdc-ilp32d--;rv32gc-ilp32d--;rv32gcv-ilp32d--;rv64imafc-lp64f--;rv64imafdc-lp64d--;rv64gc-lp64d--;rv64gcv-lp64d--"

%if %{bootstrap}
make all-gcc all-target-libgcc %{?_smp_mflags}
%else
make %{?_smp_mflags}
%endif
popd


%install
target=riscv64
echo "Installing for [$target]"
pushd build-$target-%{sysarch}
make install-strip DESTDIR=%{buildroot}
popd

rm -rf %{buildroot}%{_infodir}
rm -rf %{buildroot}%{_mandir}/man7
rm -f %{buildroot}%{_prefix}/lib/libiberty.a
rm -f %{buildroot}%{_libdir}/libcc1*
rm -rf %{buildroot}%{_prefix}/$target-%{sysarch}/share
find %{buildroot}/%{_prefix}/$target-%{sysarch} -name '*.la' -delete
rm -rf %{buildroot}%{_prefix}/lib*/gcc/$target-%{sysarch}/%{maj_ver}/install-tools
rm -rf %{buildroot}%{_libexecdir}/gcc/$target-%{sysarch}/%{maj_ver}/install-tools
rm -f %{buildroot}%{_libexecdir}/gcc/$target-%{sysarch}/%{maj_ver}/*.la

if [ -d %{buildroot}%{_prefix}/lib/gcc/riscv64-%{sysarch}/%{maj_ver} ]; then
    find %{buildroot}%{_prefix}/lib/gcc/riscv64-%{sysarch}/%{maj_ver} \
         \( -name '*.a' -o -name '*.o' \) \
         -exec riscv64-%{sysarch}-strip --strip-debug {} + 2>/dev/null || true
fi


%files -n riscv64-%{sysarch}-gcc
%license COPYING*
%doc README
%{_bindir}/riscv64-%{sysarch}-*
%exclude %{_bindir}/riscv64-%{sysarch}-?++
%dir %{_prefix}/lib/gcc/riscv64-%{sysarch}
%{_prefix}/lib/gcc/riscv64-%{sysarch}/%{maj_ver}
%dir %{_libexecdir}/gcc/riscv64-%{sysarch}
%{_libexecdir}/gcc/riscv64-%{sysarch}/%{maj_ver}
%exclude %{_libexecdir}/gcc/riscv64-%{sysarch}/%{maj_ver}/cc1plus
%{_mandir}/man1/riscv64-*.1.gz
%exclude %{_mandir}/man1/riscv64-%{sysarch}-g++.1.gz
%dir %{_datadir}/riscv64-%{sysarch}/gcc-%{maj_ver}
%{_datadir}/riscv64-%{sysarch}/gcc-%{maj_ver}/python

%if ! %{bootstrap}
%files -n riscv64-%{sysarch}-gcc-c++
%{_bindir}/riscv64-%{sysarch}-?++
%{_prefix}/riscv64-%{sysarch}/include/c++
%{_libexecdir}/gcc/riscv64-%{sysarch}/%{maj_ver}/cc1plus
%{_mandir}/man1/riscv64-%{sysarch}-g++.1.gz
%endif

%changelog
* Mon Apr 28 2025 Bailei27 <SwanBaiLei2Seven@foxmail.com>
- Restrict to riscv64 only
- Use picolibc instead of newlib
- Set -mcmodel=medany
- Add full multilib support as requested

* Mon Jul 02 2018 Cristian Balint <cristian.balint@gmail.com>
- git update releases
