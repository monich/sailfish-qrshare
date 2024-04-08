Name:       sailfish-qrshare

Version:    1.1.2
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
BuildRequires: qt5-qttools
BuildRequires: qt5-qttools-linguist

%define qrshare_datadir_old /usr/share/sailfish-qrshare/
%define qrshare_datadir /usr/share/nemo-transferengine/openrepos-qrshare
%define qrshare_svg %{qrshare_datadir}/qrshare.svg
%define qrshare_png %{qrshare_datadir}/qrshare.png
%define qrshare_qmlplugindir %{_libdir}/qt5/qml/com/monich/qrshare
%define transferengine_plugindir %{_libdir}/nemo-transferengine/plugins
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

%posttrans
rm -f %{qrshare_datadir_old}/qrshare.png
if  [ -d %{qrshare_datadir_old}/translations ] ; then rmdir %{qrshare_datadir_old}/translations || : ; fi
if  [ -d %{qrshare_datadir_old} ] ; then rmdir %{qrshare_datadir_old} || : ; fi

%files
%defattr(-,root,root,-)
%dir %{qrshare_datadir}
%{transferengine_plugindir}/libqrshareplugin.so
%{transferengine_plugindir}/sharing/libqrshareplugin2.so
%{qrshare_datadir}/translations
%{qrshare_datadir}/unhappy.svg
%{qrshare_datadir}/*.qml
%{qrshare_qmlplugindir}
%{qrshare_svg}
%{mkicon}
