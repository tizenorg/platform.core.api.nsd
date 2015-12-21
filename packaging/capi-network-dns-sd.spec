Name:          capi-network-dns-sd
Summary:       Dns-sd library in Native API
Version:       0.0.1
Release:       1
Source:       %{name}-%{version}.tar.gz
License:       Apache-2.0
Group:         System/Network
BuildRequires:	cmake
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:	pkgconfig(mDNSResponder)
Requires(post):	/sbin/ldconfig
Requires(postun):	/sbin/ldconfig

%description
dns-sd library in Tizen Native C API

%package devel
Summary:  Dns-sd library in Native API (Development)
Group:    System/Network
Requires: %{name} = %{version}-%{release}

%description devel
dns-sd library in Tizen Native C API (Development)

%prep
%setup -q

%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
cmake -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER}

make %{?_smp_mflags}

%install
%make_install

#License
mkdir -p %{buildroot}%{_datadir}/license
cp LICENSE.APLv2 %{buildroot}%{_datadir}/license/capi-network-dns-sd

%post -p /sbin/ldconfig
%post devel -p /sbin/ldconfig

%postun -p /sbin/ldconfig
%postun devel -p /sbin/ldconfig

%files
%{_libdir}/libcapi-network-dns-sd.so*
%{_bindir}/dns-sd-test
%{_datadir}/license/capi-network-dns-sd
%manifest capi-network-dns-sd.manifest


%files devel
%{_includedir}/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-network-dns-sd.so*
