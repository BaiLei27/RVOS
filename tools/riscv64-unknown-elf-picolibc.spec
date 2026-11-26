%global pkgvers 0
%global scdate0 20261119
%global branch0 main
%global debug_package %{nil}
%global picolibc_url https://github.com/picolibc/picolibc/releases/download/%{version}/picolibc-%{version}.tar.xz
Source0: %{picolibc_url}
# Source0: picolibc-%{version}.tar.xz

%define sysarch unknown-elf
%define target riscv64
%define target_full %{target}-%{sysarch}

Name:           cross-%{sysarch}-picolibc
Version:        1.8.12
Release:        %{scdate0}.%{pkgvers}%{?dist}
Summary:        C library designed for embedded
License:        BSD and MIT and LGPLv2+ and ISC
BuildArch:      x86_64

URL:            https://keithp.com/picolibc

BuildRequires:  git gcc-c++ meson ninja-build %{target_full}-gcc %{target_full}-binutils curl

%undefine _auto_set_build_flags
%global __os_install_post /usr/lib/rpm/brp-compress
%global _binaries_in_noarch_packages_terminate_build 0
%global __provides_exclude_from ^(%{_prefix}/.*-%{sysarch}/.*$)
%global __requires_exclude_from ^(%{_prefix}/.*-%{sysarch}/.*$)

%description
Picolibc is a C library intended for use on embedded systems.

%package -n %{target_full}-picolibc

Summary: C library intended for use on %{target} embedded systems
Provides:  %{target}-picolibc
Provides:  %{target}-libc

%description -n %{target_full}-picolibc
Picolibc is a C library intended for use on %{target} embedded systems.

%prep
SRC=%{_sourcedir}/picolibc-%{version}.tar.xz
if [ ! -s "$SRC" ]; then
    mkdir -p %{_sourcedir}
    URL="%{picolibc_url}"
    echo "+++ Downloading $URL"
    ok=0
    if command -v curl >/dev/null 2>&1; then
        curl -fL --retry 3 --connect-timeout 30 -o "$SRC.part" "$URL" && ok=1
    elif command -v wget >/dev/null 2>&1; then
        wget -c -O "$SRC.part" "$URL" && ok=1
    else
        echo "ERROR: neither curl nor wget is available" >&2
        exit 1
    fi
    if [ "$ok" != 1 ]; then
        rm -f "$SRC.part"
        echo "ERROR: failed to download picolibc-%{version}.tar.xz" >&2
        exit 1
    fi
    mv -f "$SRC.part" "$SRC"
fi
%setup -q -n picolibc-%{version}

%build
endian="little"
echo "Building for [%{target}]"
mkdir -p build-%{target_full}
pushd build-%{target_full}

meson setup .. \
  --buildtype=release \
  --cross-file ../scripts/cross-%{target_full}.txt \
  -Dprefix='/usr/%{target_full}' \
  -Dspecsdir=lib

ninja %{?_smp_mflags}
popd

%install
echo "Installing for [%{target_full}]"
pushd build-%{target_full}
  DESTDIR=%{buildroot} ninja install
  rm -rf %{buildroot}%{_infodir}
popd

%files -n %{target_full}-picolibc
%doc README.md
%license COPYING*
%dir %{_prefix}/%{target_full}
%dir %{_prefix}/%{target_full}/include
%{_prefix}/%{target_full}/include/*
%dir %{_prefix}/%{target_full}/lib
%{_prefix}/%{target_full}/lib/*
#%{_prefix}/%{target_full}/lib/*.specs

%changelog
* Mon Apr 28 2025 Bailei27 <SwanBaiLei2Seven@foxmail.com>
- Restrict to riscv64 only

* Fri May 27 2022 Cristian Balint <cristian.balint@gmail.com>
- github update releases
