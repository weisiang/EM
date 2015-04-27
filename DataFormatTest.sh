rm CountRlt
cat InputFile | while read line
do
rm Test*
./TrainEM_part.sh -i $line -n 1 -t 5 -f Test &>> Error
done
rm Test* Error
