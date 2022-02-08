all: main main2

main:
	@echo " Compile travelMonitor ...";
	
	gcc -I ./include -o build/travelMonitor examples/main.c src/Functions_travelmonitor.c src/newfunctions_travelmonitor.c src/travel_main_questions.c src/bloomFunctions.c

main2:
	@echo " Compile monitor ...";
	
	gcc -I ./include -o build/monitor examples/main2.c src/StartersAndHelpFunctions.c src/BST.c src/bloomfilter.c src/bloomFunctions.c src/skipListFunctions.c src/monitornewfunc.c src/monitor_main_questions.c