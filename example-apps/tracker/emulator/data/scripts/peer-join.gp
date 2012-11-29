################
set terminal postscript enhanced "Times-Roman" 24
set size 1.0,0.8
set output 'peer-join-time.eps'
set key right top

set xlabel "# Peer Joins (Thousands)"
set ylabel "Per-Peer Join + Selection Time (us)"

plot '< cat native.out              | grep show_diff | awk "{ print \$3 / \$7 }"' title "Native", \
     '< cat filesharing-generic.out | grep show_diff | awk "{ print \$3 / \$7 }"' title "P4P"

reset
################

################
set terminal postscript enhanced "Times-Roman" 24
set size 1.0,0.8
set output 'peer-memory.eps'
set key right top

set yrange [0:500]

set xlabel "# Peer Joins (Thousands)"
set ylabel "Per-Peer Memory (Bytes)"

plot '< cat native.out              | grep show_diff | awk "{ print \$11 / \$7 }"' title "Native", \
     '< cat filesharing-generic.out | grep show_diff | awk "{ print \$11 / \$7 }"' title "P4P"

reset
################

