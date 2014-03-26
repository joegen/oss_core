set data style lines
#set title "Efficacy of verification system under increasing network size, \n under ideal conditions, with 20% malice and malicetype 1"
set xlabel "Message id"
set ylabel "%of messages reached"
set output "trial1.eps"
set key bottom right
load 'style.gnu'

plot "mon-justkill.dat" using 1:2 
