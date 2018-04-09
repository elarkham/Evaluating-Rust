# See LICENSE file for copyright and license details

OUTPUT=out
NAME="$1"
N_INTERVALS="$2"
N_THREADS="$3"

if [ "" == "$1" ]; then
	echo "Usage: run.sh [PROGRAM_NAME]"
	exit -1
fi
mkdir -p ${OUTPUT}
rm -rf ${OUTPUT}/cpp_$NAME_*.txt

INTERVALS=$(eval echo {1..$N_INTERVALS})
THREADS=$(eval echo {1..$N_THREADS})

if [ "$NAME" == "wordfind" ]; then
  for i in $INTERVALS; do
  	for j in $THREADS; do
      ./build/$NAME/${NAME}_O2 $(eval echo ../data/wordfind/alice_{1..$j}.txt) >> ${OUTPUT}/cpp_${NAME}_$i.txt
  	done
  done
  exit;
fi

for i in $INTERVALS; do
	for j in $THREADS; do
		./build/$NAME/${NAME}_O2 $j >> ${OUTPUT}/cpp_${NAME}_$i.txt
	done
done
