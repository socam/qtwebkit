# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.26
# 

Name:       qtwebkit-socam

# >> macros
# << macros

Summary:    QtWebkit for SOCAM
Version:    4.9.4
Release:    1
Group:      Applications/Internet
License:    TBD
Source0:    %{name}-%{version}.tar.gz
Provides:   qtwebkit = 4.9.4

%description
QtWebkit for SOCAM

%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
# >> build pre
# << build pre

#make %{?jobs:-j%jobs}

# >> build post
# << build post

%install
rm -rf %{buildroot}/*
# >> install pre
# << install pre

cp %{_builddir}/%{name}-%{version}/* %{buildroot} -r

# >> install post
# << install post

%post
# >> post

# << post

%files
%defattr(-,root,root,-)

/usr/include/qt4
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri
/usr/lib/pkgconfig/QtWebKit.pc
/usr/lib/qt4/imports/QtWebKit/libqmlwebkitplugin.so
/usr/lib/qt4/imports/QtWebKit/qmldir
/usr/lib/libQtWebKit.so.4.9.4
/usr/lib/libQtWebKit.so.4.9
/usr/lib/libQtWebKit.so.4
/usr/lib/libQtWebKit.prl
/usr/lib/libQtWebKit.so



# >> files
# << files
