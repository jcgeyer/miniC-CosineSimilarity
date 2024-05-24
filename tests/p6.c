extern void print(int);
extern int read();

int func(int n){
	int max;
	int i;
	int a;
	int b;
	i = 0;
	max = 0;
	
	while (i < n){ 
		b = read();
		if (b > max)
			max = b;
		i = i + 1;
	}
	
	return max;
}
