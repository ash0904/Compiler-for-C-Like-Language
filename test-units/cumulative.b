declblock{
	int sum,cumsum[100],arr[100],temp;
  int i,j,prevval;
}
codeblock{
  sum=0;
  i=0;
  while i<=10{
    arr[i]=i;
    println i," : ",arr[i];
    i=i+1;
  }
  i=0;
  println "Cummulative Array is :- ";
  while i<=10
  {
    prevval=i-1;
    cumsum[i]=cumsum[prevval]+arr[i];
    println i," : ",cumsum[i];
    i=i+1;
  }
}
