################
set terminal postscript eps color enhanced "Times-Roman" 24
set size 1.0,0.8
set output 'geovspdist.eps'

# set xrange [0:100]
set xlabel "pDistance"
set ylabel "Geographical Distance (km)"

plot 'geovspdist' using 1:2 notitle

set terminal x11
replot
pause -1

################

