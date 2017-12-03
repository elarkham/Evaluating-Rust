# See LICENSE file for copyright and license details

OUTPUT=out

if [ "" == "$1" ]; then
	echo "Usage: run.sh [PROGRAM_NAME]"
	exit -1
fi
mkdir -p ${OUTPUT}
touch ${OUTPUT}/$1.txt
for i in {1..24}; do
	./$1/target/release/$1 $i >> ${OUTPUT}/$1.txt
done
