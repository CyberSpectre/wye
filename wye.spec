Name:		wye
Version:	0.04
Release:	1%{?dist}
Summary:	Stream processing framework
Group:		Applications/Internet
License:	GPLv3
URL:		http://wye.sourceforge.net
Source:		%{name}-%{version}.tar.gz
BuildRequires: systemd
%{?systemd_requires}
#Requires:	

# Compression disabler?

%description
Wye is a stream processing framework for medium-to-low latency processing of
data.  Wye is a much simpler framework than other big data frameworks, which
makes it much simpler to get stream processing up and running.

It currently operates a single host framework, although there are ambitious
plans to expand to large clusters.

Please see documentation in /usr/share/doc/wye.
%prep
%autosetup

%build
%configure
make %{?_smp_mflags}

%install
%make_install

%post
%systemd_post wye.service

%preun
%systemd_preun wye.service

%postun
%systemd_postun_with_restart wye.service

%clean
rm -rf $RPM_BUILD_ROOT

%files
#%exclude /usr/share/info/dir
%defattr(-,root,root) 
%doc /usr/share/doc/wye
#%doc /usr/share/info/
#%doc /usr/share/man
/usr/bin/*
/usr/lib/python2.7/site-packages/wye
#%{_libdir}/lib*
/usr/lib/systemd/system/wye.service

%changelog
