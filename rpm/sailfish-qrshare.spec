Name:       sailfish-qrshare
Version:    1.0.11
Release:    1
License:    BSD
Summary:    Share plugin for generating QR and Aztec codes
Group:      System/Libraries
URL:        https://github.com/monich/sailfish-qrshare
Source0:    %{name}-%{version}.tar.gz

BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Gui)
BuildRequires: pkgconfig(Qt5Svg)
BuildRequires: pkgconfig(Qt5Qml)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(nemotransferengine-qt5)
BuildRequires: pkgconfig(rpm)
BuildRequires: pkgconfig(popt)
BuildRequires: qt5-qttools
BuildRequires: qt5-qttools-linguist

%define qrshare_datadir  %{_datadir}/%{name}
%define qrshare_svg %{qrshare_datadir}/qrshare.svg
%define qrshare_png %{qrshare_datadir}/qrshare.png
%define qrshare_qmlplugindir %{_libdir}/qt5/qml/com/monich/qrshare
%define mkicon %{qrshare_qmlplugindir}/mkicon

%description
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

%post
%{mkicon} %{qrshare_svg} || :
rm -f %{mkicon}

%preun
if [ "$1" == 0 ] ; then rm -f %{qrshare_png}; fi

%files
%defattr(-,root,root,-)
%{_libdir}/nemo-transferengine/plugins/libqrshareplugin.so
%{qrshare_datadir}/translations/%{name}*.qm
%{qrshare_datadir}/unhappy.svg
%{qrshare_datadir}/*.qml
%{qrshare_qmlplugindir}
%{qrshare_svg}
%{mkicon}
