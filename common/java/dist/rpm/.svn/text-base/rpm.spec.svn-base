%define name p4p-common-java

Summary:	P4P Common Libraries (Java)
Name:		%{name}
Version:	%{version}
Release:	%{release}
BuildRequires:	jpackage-utils, java-devel, ant
Requires:	jpackage-utils, java
Source:		%{name}-%{version}.tar.gz
Vendor:		Yale University
URL:		http://p4p.cs.yale.edu/
License:	BSD
Group:		Networking
Prefix:		%{_prefix}
BuildRoot:	/tmp/%{name}-%{version}-build

%description
P4P Common Libraries (Java)

%prep
%setup

%build
ant

%install
rm -rf $RPM_BUILD_ROOT

mkdir -p $RPM_BUILD_ROOT/%{_javadir}
cp -p p4p_common.jar $RPM_BUILD_ROOT/%{_javadir}/%{name}.jar

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

%{_javadir}/*

