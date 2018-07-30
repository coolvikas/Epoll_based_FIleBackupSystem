for i in {101..200..10}
do
	awk -f average.awk $i >> finalresult
done
