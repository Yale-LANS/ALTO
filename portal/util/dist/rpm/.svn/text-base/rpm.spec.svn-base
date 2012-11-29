%define name p4p-portal-utils

Summary:	P4P Portal Utilities
Name:		%{name}
Version:	%{version}
Release:	%{release}
BuildRequires:	cmake >= 2.6.0, boost-devel >= 1.35, p4p-common-cpp
Requires:	boost >= 1.35, p4p-common-cpp
Source:		%{name}-%{version}.tar.gz
Vendor:		Yale University
URL:		http://p4p.cs.yale.edu/
License:	BSD
Group:		Networking
Prefix:		%{_prefix}
BuildRoot:	/tmp/%{name}-%{version}-build

%description
P4P Portal Utilities

%prep
%setup
mkdir build-temp
cd build-temp
cmake							\
	-D CMAKE_BUILD_TYPE:STRING=Release		\
	-D CMAKE_INSTALL_PREFIX:STRING=%{_prefix}	\
	..

%build
cd build-temp
make

%install
rm -rf $RPM_BUILD_ROOT
cd build-temp
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-,root,root)

%doc README
%doc LICENSE
%doc AUTHORS

%{_prefix}/bin/*

