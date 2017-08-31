declblock{
	int data[100] ;
	int i, sum ;
}

codeblock{
	for i = 1, 100 {
		data[0] = i ; 
	}
	

	sum = 0;
	for i = 1, 100 {
		sum = sum + data[0] ;
 	}

	print "Sum value: ", sum ;
}

