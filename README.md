# System-Programming-P2
Project implemented as part of the course Syspro K24 - 2021 - DIT
Based on project 1, added fork/exec, signal handlers and named pipes.

Mark: 88/100 Final mark of k24: 10/10

How to run:

make

cd build

./create_infiles.sh inputfile input_dir 3 // 3 = numFilesPerDirectory

./travelMonitor -m 30 -b 5 -s 100000 -i input_dir //m = numMonitors b = bufferSize s = sizeOfBloom
