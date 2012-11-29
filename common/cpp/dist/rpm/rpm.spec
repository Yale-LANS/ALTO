%define name p4p-common-cpp

Summary:	P4P Common Libraries
Name:		%{name}
Version:	%{version}
Release:	%{release}
BuildRequires:	cmake >= 2.6.0, libcurl-devel, doxygen, graphviz
Requires:	curl
Source:		%{name}-%{version}.tar.gz
Vendor:		Yale University
URL:		http://p4p.cs.yale.edu/
License:	BSD
Group:		Networking
Prefix:		%{_prefix}
BuildRoot:	/tmp/%{name}-%{version}-build

%description
P4P Common Libraries

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

# Copy built docs back to source temporarily
cp -r "$RPM_BUILD_ROOT/%{_prefix}/share/doc/p4p_common_cpp/html" ../html

# Remove installed docs
rm -rf "$RPM_BUILD_ROOT/%{_prefix}/share/doc/p4p_common_cpp"

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
%doc LIBPATRICIA-COPYRIGHT
%doc LIBPATRICIA-CREDITS
%doc LIBPATRICIA-README
%doc html

%{_prefix}/lib/*
%{_prefix}/include/*

