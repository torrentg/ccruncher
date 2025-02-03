Name: ccruncher
Version: 2.6.1
Release: 1
Summary: A portfolio credit risk modeling program

License: GPLv2
URL: http://www.ccruncher.net/
Source: ccruncher-%{version}_src.tgz

BuildRequires: gsl-devel >= 2.1
BuildRequires: expat-devel >= 2.1.1
BuildRequires: zlib-ng-compat-devel >= 1.2.8
BuildRequires: qt5-qtbase-devel >= 5.6.1
BuildRequires: qwt-qt5-devel >= 6.1.2
BuildRequires: libconfig-devel >= 1.5

Requires: gsl >= 2.1
Requires: expat >= 2.1.1
Requires: zlib-ng-compat >= 1.2.8
Requires: qt5-qtbase >= 5.6.1
Requires: qwt-qt5 >= 6.1.2
Requires: libconfig >= 1.5

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
* Mon Feb 03 2025 Gerard Torrent <gtorrent@ccruncher.net> - 2.6.1-1
- Updated Windows build + removed warnings.
* Mon Mar 06 2023 Gerard Torrent <gtorrent@ccruncher.net> - 2.6.0-1
- Project migrated to github.
* Tue Jun 12 2018 Gerard Torrent <gtorrent@ccruncher.net> - 2.5.1-1
- Solved floating point comparison error.
* Thu Jul 20 2017 Gerard Torrent <gtorrent@ccruncher.net> - 2.5.0-1
- Initial RPM release.


#https://fedoraproject.org/wiki/How_to_create_an_RPM_package
