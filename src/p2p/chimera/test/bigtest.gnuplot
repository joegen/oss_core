set data style lines
#set title "Efficacy of verification system under increasing network size, \n under ideal conditions, with 20% malice and malicetype 1"
set xlabel "Message id"
set ylabel "Number of hops taken"
set output "bigtest-16.eps"
set key bottom right
load 'style.gnu'

plot "bigtest-base16.dat" using 1:2  with points
