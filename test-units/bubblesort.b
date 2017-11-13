declblock{
	int  i, j, arr[100],temp,next;
}
codeblock{
arr[0]=3;
arr[1]=2;
arr[2]=5;
arr[3]=7;
arr[4]=1;
i=0;
  while i<5
  {
	print arr[i]," ";
	i = i + 1;
  }
println "original array";

i=0;
  while i<5
  {
      j=0;
      while j<4
      {
        next=j+1;
        if arr[j]>arr[next]
          {
            temp=arr[j];
            arr[j]=arr[next];
            arr[next]=temp;

          }
        j=j+1;
      }
  i=i+1;
  }
i=0;
  while i<5
  {
	print arr[i]," ";
	i = i + 1;
  }
println "Sorted array";
}	
