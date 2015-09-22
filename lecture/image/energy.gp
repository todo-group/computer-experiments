set log x
set xrange [10:]
set yrange [-100:300]
set xlabel "MCS"
set ylabel "Energy"
set key at 10000,-30
plot "steepest_decent.dat" u 1:3 w l title "steepest decent" lt 7, "simulated_annealing.dat" u 1:4 w l title "simulated annealing" lt 6
