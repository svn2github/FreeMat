Summary:	FreeMat is an environment for rapid engineering and scientific processing.
Name:		FreeMat
Version:	1.06
Release:	1
URL:		http://sourceforge.net/projects/freemat/
Source0:	http://prdownloads.sourceforge.net/freemat/FreeMat-1.06.tar.gz
License:	MIT
Group:		Applications/Engineering
BuildRoot:	%{_tmppath}/%{name}-%(id -un)
#Requires:	

%description
FreeMat is an environment for rapid engineering and scientific processing.  It
is similar to commercial systems such as MATLAB from Mathworks and IDL from
Research Systems, but is Open Source.  It is free as in speech and free as in
beer.

%prep
%setup -q

%build
./configure --prefix=%{_prefix}
# --bindir=%{_bindir} \
#	--mandir=%{_mandir}
CFLAGS="$RPM_OPT_FLAGS" make

%install
rm -rf $RPM_BUILD_ROOT

make prefix=$RPM_BUILD_ROOT%{_prefix} bindir=$RPM_BUILD_ROOT%{_bindir} \
	mandir=$RPM_BUILD_ROOT%{_mandir} libdir=$RPM_BUILD_ROOT%{_libdir} \
	localstatedir=$RPM_BUILD_ROOT%{_localstatedir} \
	datadir=$RPM_BUILD_ROOT%{_datadir} \
	includedir=$RPM_BUILD_ROOT%{_includedir} \
	sysconfdir=$RPM_BUILD_ROOT%{_sysconfdir} install

%clean
[ %{buildroot} != "/" ] && rm -rf %{buildroot}


%files
%defattr(-,root,root)
%doc AUTHORS COPYING COPYRIGHT.GPL INSTALL PORTS README 
%{_bindir}/FreeMat
%{_datadir}/FreeMat

%changelog
* Thu Jul 8 2004 <jeremy.gill@med.ge.com>
- Initial Redhat 8.0 build.
