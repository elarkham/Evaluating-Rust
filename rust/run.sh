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
rm -rf ${OUTPUT}/rust_${NAME}*.txt

INTERVALS=$(eval echo {1..$N_INTERVALS})
THREADS=$(eval echo {1..$N_THREADS})
for i in $INTERVALS; do
	for j in $THREADS; do
		./$NAME/target/release/$NAME $j >> ${OUTPUT}/rust_${NAME}_$i.txt
	done
done
