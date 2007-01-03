Summary: Freemat is an intepreted, matrix-oriented development environment for engineering and scientific applications, similar to the commercial package MATLAB. Freemat provides visualization, image manipulation, and plotting as well as parallel programming.
Name: freemat
Version: 3.0
Release: 1
License: GPL
Group: Engineering
URL: http://freemat.sourceforge.net
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description

%prep
%setup -q

%build
./configure LDFLAGS=-L/usr/lib/atlas CPPFLAGS=-I/usr/include/ufsparse
make
%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install
%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
/usr/local/bin/FreeMat
/usr/local/share/%{name}-%{version}
%doc COPYING AUTHORS README NEWS


%changelog
* Mon Jan  1 2007 Samit Basu <basu@localhost.localdomain> - 
- Initial build.

