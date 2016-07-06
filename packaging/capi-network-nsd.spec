Name:       capi-network-nsd
Summary:    A Network Service Discovery libraries in Native API
Version:    0.0.5
Release:    1
Group:      System/Network
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001:    capi-network-nsd.manifest
Source1002:    libnsd-dns-sd.manifest
Source1003:    libnsd-ssdp.manifest
Source1004:    nsd-tests.manifest
BuildRequires:  cmake
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dns_sd)
BuildRequires:  pkgconfig(gssdp-1.0)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
NSD library in Tizen C API This package provides both SSDP and
DNS-SD Tizen C API

%package devel
Summary:  A NSD library in Native API (Development)
Requires:   libnsd-dns-sd = %{version}-%{release}
Requires:   libnsd-ssdp = %{version}-%{release}

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

%package -n libnsd-ssdp
Summary:    NSD - SSDP library in Tizen C API

%description -n libnsd-ssdp
Network Service Discovery library for SSDP

%prep
%setup -q
chmod 644 %{SOURCE1001}
chmod 644 %{SOURCE1002}
chmod 644 %{SOURCE1003}
cp -a %{SOURCE1001} .
cp -a %{SOURCE1002} .
cp -a %{SOURCE1003} .
cp -a %{SOURCE1004} .

%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"

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

%post -n libnsd-ssdp -p /sbin/ldconfig

%postun -n libnsd-ssdp -p /sbin/ldconfig

%files
%manifest capi-network-nsd.manifest
%license LICENSE

%files devel
%{_includedir}/nsd/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/*.so

%files tests
%manifest nsd-tests.manifest
%attr(755,root,root) %{_bindir}/dns-sd-test
%attr(755,root,root) %{_bindir}/ssdp-test

%files -n libnsd-dns-sd
%manifest libnsd-dns-sd.manifest
%license LICENSE
%{_libdir}/libnsd-dns-sd.so.*

%files -n libnsd-ssdp
%manifest libnsd-ssdp.manifest
%license LICENSE
%{_libdir}/libnsd-ssdp.so.*
