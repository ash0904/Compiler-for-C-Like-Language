declblock{
	int a,fin,fac;
}
codeblock{
  
  fac = 1;
  a = 1;
  fin = 5;
L1:  
     fac = fac*a;
     println a," : ",fac;
     a = a+1;
     goto L1 if a<=fin;
  println "factorial of",fin,"is",fac;

}
