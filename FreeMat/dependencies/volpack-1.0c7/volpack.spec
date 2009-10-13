Name: volpack
Version: 1.0c7
Release: 1
Summary: portable library for fast volume rendering

Group: Applications/Engineering
License: BSD
URL: http://amide.sourceforge.net
Source0: %{name}-%{version}.tgz
Buildroot: %{_tmppath}/%{name}-%{version}-root

%description 
VolPack is a portable library of fast volume rendering algorithms that
produce high-quality images.  It was written by Phil Lacroute.

%package devel
Summary: Static libraries and header file for development using volpack
Group: Development/Libraries
Requires: volpack = %{version}

%description devel
The volpack-devel package contains the header files and static libraries
necessary for developing programs using the volpack volume rendering 
library.

%prep
%setup -n %{name}-%{version}

%build
%configure
make

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

#chmod a+x $RPM_BUILD_ROOT/%{prefix}/lib/*
%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog README
%{_libdir}/*so.*
%{_libdir}/*.la

%files devel
%defattr(-,root,root)
%doc AUTHORS COPYING
%{_libdir}/*.a
%{_libdir}/*.so
%{_includedir}/*
%{_mandir}/*

%changelog
* Wed Nov 10 2004 Andy Loening <loening@alum.mit.edu>
- updates for multilib support (x86_64)
- split off devel package

* Sun Mar 18 2001 Andy Loening <loening@alum.mit.edu>
- wrote this fool thing
