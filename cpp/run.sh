# See LICENSE file for copyright and license details

OUTPUT=out

if [ "" == "$1" ]; then
	echo "Usage: run.sh [PROGRAM_NAME]"
	exit -1
fi
mkdir -p ${OUTPUT}
touch ${OUTPUT}/$1.txt
for i in {1..8}; do
	for j in {1..16}; do
		./build/$1/$1_O2 $j >> ${OUTPUT}/$1_$i.txt
	done
done
