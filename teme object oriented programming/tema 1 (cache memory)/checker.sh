#!/bin/bash

make clean &> /dev/null
make build &> /dev/null

no_tests=10
no_bonus=2

grade=0
bonus=0

for i in `seq 0 $(($no_tests - 1))`; do
	cp "input/test"$i".in" "configuratie"$i.in
	cp "ref/out"$i".out" "ref"$i".out"
	
	make INPUT_PATH="$(pwd)/configuratie"$i".in" OUTPUT_PATH="$(pwd)/out"$i".out" run &> /dev/null

	diff "out"$i".out" "ref"$i".out" &> /dev/null

	if [  $? -eq 0 ]
 	then 
		grade=$(($grade+10));
		echo "test"$i".............10"
	else
		echo "test"$i"............. 0"
		# diff "output/test"$i".out" "ref/test"$i".ref"
	fi
	
	rm -rf "configuratie"$i".in" "out"$i".out" "ref"$i".out" &> /dev/null
done

for i in `seq 0 $(($no_bonus - 1))`; do
	cp "input/bonus"$i".in" "configuratie"$i.in
	cp "ref/bonus"$i".out" "ref"$i".out"

	make INPUT_PATH="$(pwd)/configuratie"$i".in" OUTPUT_PATH="$(pwd)/out"$i".out" run &> /dev/null
	
	diff "out"$i".out" "ref"$i".out" &> /dev/null

	if [  $? -eq 0 ]
 	then 
		bonus=$(($bonus+10));
		echo "bonus"$i".............10"
	else
		echo "bonus"$i"............. 0"
		# diff "output/test"$i".out" "ref/test"$i".ref"
	fi
	
	rm -rf "configuratie"$i".in" "out"$i".out" "ref"$i".out" &> /dev/null
done
echo ""


echo "GRADE.................................$grade/100";
echo "BONUS.................................$bonus/20";


