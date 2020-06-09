OUTPUT_NAME=$1;
EXE_NAME=$2;
STM_TYPE=$3
echo "" > $OUTPUT_NAME;

for i in {1..4} ; do
    for j in {1..32} ; do
        echo "head,$STM_TYPE,$i,$j" >> $OUTPUT_NAME;
        $EXE_NAME $STM_TYPE $i $j >> $OUTPUT_NAME;
    done
done
