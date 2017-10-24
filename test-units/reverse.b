declblock{
	int num,test,ans = 0,i,tmp,pos,in[100],out[100];
}
codeblock{
	
	read test;
	tmp = test;
	while test>0 {
		read num;
		pos = tmp-test;
		in[pos] = num;
		ans = 0;
		while num>0 {
			ans = ans * 10;
			ans = ans + num%10;
			num = num / 10;
		}
		out[pos] = ans;
		test = test - 1;
	}
	tmp = tmp-1;
	for i=0,tmp {
		println "reversed string of ",in[i]," is -> ", out[i];
	} 	

}
