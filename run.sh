#!/bin/bash

COUNT=0
while [[ true ]]; do
	rm -rf results
	./tone -t target -w words

	if [[ -e ./results ]]; then
		echo "GOT IT"
		rm -rf ./savedlow
		tail -2 ./results > ./savedlow
	fi
	COUNT=`./addr.py 1 $COUNT`
	if [[ $COUNT -eq 2 ]]; then
		break
	fi
done
