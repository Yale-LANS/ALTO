#/bin/bash

portalshell=p4p_portal_shell
geovspdistanalysis=p4p_portal_config_geovspdist.pl

if [ $# -le 1 ]
then
	echo "Usage: $0 <p4p portal server>:<port> <output (e.g., ISP name)>"
	echo "Output:"
	echo "       pidmap.<output>: PID map visible to applications"
	echo "       pdistance.<output>: pDistances visible to applications"
        echo "       geovspdist.<output>: geographical distance v.s. pDistance (of PID pairs)"
        echo "       geovspdist.<output>.eps: visualization"
	exit 1
fi

server=$1; shift
isp=$1; shift

path=`dirname $0`
if [ -n "$path" ]
then
	path="$path/"
fi
commands=`mktemp`
results=`mktemp`


# GetPIDMap 
echo "show pidmap" > $commands
echo "exit" >> $commands

$portalshell $server $commands >$results 2>/dev/null
cp -f $results pidmap.$isp


# GetpDistance
echo "show pdistance" > $commands
echo "exit" >> $commands

$portalshell $server $commands >$results 2>/dev/null
cp -f $results pdistance.$isp


# Analysis: geographical distance vs pDistance
perl "${path}${geovspdistanalysis}" pidmap.$isp pdistance.$isp > geovspdist

gnuplot ${path}p4p_portal_config_geovspdist.gp

mv geovspdist geovspdist.$isp
mv geovspdist.eps geovspdist.$isp.eps

rm -f $commands
rm -f $results


# Internal functions
function dirname()
{
	local dir="${1%${1##*/}}"
	"${dir:=./}" != "/" && dir="${dir%?}"
	echo "$dir"
}

