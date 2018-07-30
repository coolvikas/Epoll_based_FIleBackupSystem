BEGIN { 
	th = 0;
	rt = 0;
}
{
         th += $1
         rt += $2	


}
END{
	print FILENAME" "th/NR" "rt/NR
}
