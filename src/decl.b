declblock{
 int ayush=1,meta;
 int sum,i=0;
}
codeblock{
	sum = 0;
	if i == 1
  	{
		sum = ayush;
		println i," is equal to 1";
 	}
  	else
  	{
    		meta = ayush;
		println i," is equal not to 1";
  	}
	for i = ayush - 1,5 {
		sum = sum + i;
	}
	read meta; 
	println "you entered meta as: ",meta, " but sum is: ",sum; 
	while sum >= 2 {
		println sum;
		sum = sum -1;
	}
	println "done";


}
