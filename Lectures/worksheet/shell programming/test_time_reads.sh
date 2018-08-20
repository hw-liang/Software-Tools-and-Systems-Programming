times=$1
filename=$2    
total=0
for i in `seq $times`
do
      #echo running for time $i
      # quotations needed around $filename to handle filenames with spaces
	num_reads=`./time_reads 2 "$filename" 2> /dev/null | cut -f 1 -d " "`
      #echo $num_reads
	total=`expr $total + $num_reads`
done
echo Average is `expr $total / $times`

