#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/*
red: 2
corroded: triangle
shiny: pentagon
concave: 7
blue: 9

_ + _ * _^2 + _^3 - _ = 399

ANSWER: 9 2 5 7 3
*/

int * arr = NULL;

void printArray(int * array,int length){
	int idx = 0;
	while(idx < length){
		printf("%d ",arr[idx++]);
	}
	printf("\n");
}

void swap(int * e1,int * e2){
	int tmp = *e1;
	*e1 = *e2;
	*e2 = tmp;
}


int check(){
	return((arr[0] + (arr[1]*pow(arr[2],2)) + (pow(arr[3],3)) - arr[4]) == 399);
}

void permute(int *array,int i,int length) { 
  if (length == i){
     if(check()) printArray(array,length);
     return;
  }
  int j = i;
  for (j = i; j < length; j++) { 
     swap(array+i,array+j);
     permute(array,i+1,length);
     swap(array+i,array+j);
  }
  return;
}


int main(){
	arr = calloc(5,sizeof(int));
	arr[0] = 2;
	arr[1] = 3;
	arr[2] = 5;
	arr[3] = 7;
	arr[4] = 9;

	permute(arr,0,5);

	free(arr);
	return 0;
}