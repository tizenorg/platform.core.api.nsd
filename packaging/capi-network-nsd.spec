Name:       capi-network-nsd
Summary:    A Network Service Discovery libraries in Native API
Version:    0.0.1
Release:    1
Group:      System/Network
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(mDNSResponder)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
NSD library in Tizen C API This package provides both SSDP and
DNS-SD Tizen C API

%package devel
Summary:  A NSD library in Native API (Development)
Requires:   libnsd-dns-sd = %{version}-%{release}

%description devel
Netowrk Service Discovery library in Tizen C API development files

%package tests
Summary:    NSD - NSD test binaries

%description tests
NSD tests for both DNS-SD and SSDP Tizen C API

%package -n libnsd-dns-sd
Summary:    NSD - DNS-SD library in Tizen C API

%description -n libnsd-dns-sd
Network Service Discovery library for DNS-SD

%prep
%setup -q

%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
%endif


MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . \
        -DCMAKE_BUILD_TYPE=%{?build_type} \
        -DCMAKE_VERBOSE_MAKEFILE=ON \
        -DLIB_DIR:PATH=%{_libdir} \
        -DBIN_DIR:PATH=%{_bindir} \
        -DINCLUDE_DIR:PATH=%{_includedir} \
        -DLOCAL_STATE_DIR:PATH=%{_localstatedir} \
        -DDATA_ROOT_DIR:PATH=%{_datadir} \
        -DFULLVER=%{version} \
        -DMAJORVER=${MAJORVER}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post -n libnsd-dns-sd -p /sbin/ldconfig

%postun -n libnsd-dns-sd -p /sbin/ldconfig

%files
%license LICENSE

%files devel
%{_includedir}/nsd/*.h

%files -n libnsd-dns-sd
%license LICENSE
