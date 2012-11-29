%define name p4p-portal-docs

Summary:	P4P Portal Documentation
Name:		%{name}
Version:	%{version}
Release:	%{release}
BuildRequires:	cmake >= 2.6.0, texlive, latex2html, ImageMagick
Source:		%{name}-%{version}.tar.gz
Vendor:		Yale University
URL:		http://p4p.cs.yale.edu/
License:	BSD
Group:		Networking
Prefix:		%{_prefix}
BuildRoot:	/tmp/%{name}-%{version}-build

%description
P4P Portal Documentation

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
rm -rf "$RPM_BUILD_ROOT"
cd build-temp
make DESTDIR="$RPM_BUILD_ROOT" install

# Copy built docs back to source temporarily
cp "$RPM_BUILD_ROOT/%{_prefix}/share/doc/p4p_portal_manual.pdf" ../config-manual

# Remove installed files
rm -rf "$RPM_BUILD_ROOT"/*

%clean
rm -rf "$RPM_BUILD_ROOT"

%post

%postun

%files
%defattr(-,root,root)

%doc README
%doc LICENSE
%doc AUTHORS

%doc shell-spec/portal-shell-v2.1-spec.txt
%doc shell-spec/portal-shell-v2.1-commands.txt
%doc protocol-spec/portal-protocol-spec-v1.0.txt
%doc config-manual/p4p_portal_manual.pdf
%doc config-examples/abilene.conf

