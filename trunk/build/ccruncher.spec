Name:           ccruncher
Version:        2.5.0
Release:        1%{?dist}
Summary:        A portfolio credit risk modeling program

License:        GPLv2
URL:            http://www.ccruncher.net/
Source0:        http://sourceforge.net/projects/ccruncher/files/ccruncher/2.5/ccruncher-2.5.0.tgz

BuildRequires:  gsl-devel >= 2.1
BuildRequires:  expat-devel >= 2.1.1
BuildRequires:  zlib-devel >= 1.2.8
BuildRequires:  qt-devel >= 5.6.1
BuildRequires:  qwt-qt5-devel >= 6.1.2

Requires:  gsl >= 2.1
Requires:  expat >= 2.1.1
Requires:  zlib >= 1.2.8
Requires:  qt >= 5.6.1
Requires:  qwt-qt5 >= 6.1.2 

%description
CCruncher is a project for quantifying portfolio credit risk using the copula 
approach. CCruncher evaluates the portfolio credit risk by sampling the 
portfolio loss distribution and computing the Expected Loss (EL), Value at 
Risk (VaR) and Expected Shortfall (ES) statistics. The portfolio losses are 
obtained simulating the default times of obligors and simulating the EADs and 
LGDs of their assets. 

%prep
%setup -q

%build
qmake-qt5 ccruncher-cmd.pro
make %{?_smp_mflags}
qmake-qt5 ccruncher-gui.pro
make %{?_smp_mflags}
qmake-qt5 ccruncher-inf.pro
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{_bindir}
install -m 755 build/ccruncher-cmd $RPM_BUILD_ROOT%{_bindir}/ccruncher-cmd
install -m 755 build/ccruncher-gui $RPM_BUILD_ROOT%{_bindir}/ccruncher-gui
install -m 755 build/ccruncher-inf $RPM_BUILD_ROOT%{_bindir}/ccruncher-inf
mkdir -p $RPM_BUILD_ROOT%{_mandir}/man1
install -m 644 doc/ccruncher-cmd.1 $RPM_BUILD_ROOT%{_mandir}/man1/ccruncher-cmd.1
install -m 644 doc/ccruncher-gui.1 $RPM_BUILD_ROOT%{_mandir}/man1/ccruncher-gui.1
install -m 644 doc/ccruncher-inf.1 $RPM_BUILD_ROOT%{_mandir}/man1/ccruncher-inf.1

%files
%doc doc/AUTHORS doc/ChangeLog doc/COPYING doc/README doc/TODO 
%doc doc/ccruncher.pdf 
%doc doc/index.html doc/html
%doc samples
%{_mandir}/man1/ccruncher-cmd.1.gz
%{_mandir}/man1/ccruncher-gui.1.gz
%{_mandir}/man1/ccruncher-inf.1.gz
%{_bindir}/ccruncher-cmd
%{_bindir}/ccruncher-gui
%{_bindir}/ccruncher-inf

%changelog
* Sun Jan  9 2015 Gerard Torrent <gtorrent@ccruncher.net> - 2.5.0-1
- Initial RPM release


#http://www.rpm.org/max-rpm/s1-rpm-build-creating-spec-file.html
#https://fedoraproject.org/wiki/How_to_create_an_RPM_package#.25prep_section:_.25autosetup_command
