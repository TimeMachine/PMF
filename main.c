#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<string.h>
#define SWAP(x, y) tmp = x; x = y; y = tmp;
#define sourceLength 14
#define Number_find_pollution 6
#define Number_match 2

double data[100][30]; //output3fac_profiles
int dataSort[100][30];
double sourcePro[54][100]; //source
int sourcePro_sort[54][100] = {0};
int count;
double tmp;

int match_index[30] = {0};  // maximal number of match
double match_data[30][100][2]; // 0 is index of source , 1 is square 
int max_match_pollution[30][100];

int square_min_index[30];
double square_min_in_factor[30][100][2]; //index of source

int match_redundant_index[30];
double match_redundant_data[30][100][2]; //source only have the maximal pollution and sort by square

double calculateSquare(int factor,int source,int method){
	//method 0 is factor最大物種的平方和大小 , method 1 is 所有物種之平方和平均
	double value = 0;
	if(method == 0){
		int pollution = dataSort[max_match_pollution[factor][source]][factor];
		double r = data[pollution][factor] - sourcePro[pollution][source];
		r *= r; 
		value = r;
	}
	else if(method == 1){
		int avg_count = 0;
		int i = 0;
		double sum = 0;
		for(i=0;i<54;i++)
			if(sourcePro[i][source]!=-999){
				double r = data[i][factor] - sourcePro[i][source];
				r*=r;
				sum += r;
				avg_count++;
			}
		value = sum / avg_count;
	}
	return value;
}

void find_source(int factor,int index,int type,int answer[30],int method){
	int i = 0,find = 0;
	int source;
	if(type == 0){
		source = match_data[factor][index][0];
		//printf("%d , %d , %d , %d , %lf\n",factor,index,type,source,match_data[factor][index][1]);
	}
	else{
		source = match_redundant_data[factor][index][0];
		//printf("%d , %d , %d , %d , %lf\n",factor,index,type,source,match_redundant_data[factor][index][1]);
	}
	for(;i < count;i++){
		if(source == answer[i]){
			find = 1;
			if( calculateSquare(factor,source,method) >= calculateSquare(i,source,method) ){
				if(type == 0 && index+1 < match_index[factor])
					find_source(factor,index+1,type,answer,method);
				else if(type == 0)
					find_source(factor,0,1,answer,method);
				else if(type == 1 && index+1 < match_redundant_index[factor])
					find_source(factor,index+1,type,answer,method);
				else{
					printf("ERROR");
					exit(-1);
				}					
			}
			else{
				answer[factor] = source;
				answer[i] = -1;
				find_source(i,0,0,answer,method);
			}
			break;
		}
	}
	if(!find){
		answer[factor] = source;
	}
}

void sortTwoArray(int len ,int ascending ,double (*array)[2]){
	int i,j;
	if(ascending){
		for(i = 0;i < len-1;i++)
			for(j = len-1;j > i;j--)
				if(array[j][1] < array[j-1][1]){
					SWAP(array[j][1],array[j-1][1]);
					SWAP(array[j][0],array[j-1][0]);
				}
	}
	else{
		for(i = 0;i < len-1;i++)
			for(j = len-1;j > i;j--)
				if(array[j][1] > array[j-1][1]){
					SWAP(array[j][1],array[j-1][1]);
					SWAP(array[j][0],array[j-1][0]);
				}
	}
}

int main(int argc, char* argv[])
{
	FILE*fptr,*fptr2;
	char buffer[1000];
	char bufferC = 0;
	int bufferInt;
	double dataCopy[100][30];
	int i = 0,j = 0,k = 0;
	char sourceName[100][100];
	/*double x =0.0;
	scanf("%lE",&x);
	printf("%lf",x);*/
	
	// data read
	{
	fptr = fopen(argv[1],"r");
    if(fptr == NULL){
        printf("open failure");
        return 1;
    }else{
		for(;i < 114; i++){
			while(bufferC != '\n'){
				fscanf(fptr,"%s%c",buffer,&bufferC);
			}
			bufferC = 0; 
			//printf("%s\n",buffer);
		}
		for(i = 0;i < 54;i++){
			bufferC = 0;
			j = 0;
			fscanf(fptr,"%d %s",&bufferInt,buffer);
			//printf("%s\n",buffer);
			while(bufferC != '\n'){
				fscanf(fptr,"%lE%c",&data[i][j],&bufferC);
				//printf("%lf ",data[i][j]);
				j++;
				count = j;
			}
		}
    }
	//data = data / n-butane
    for (j = 0; j < count; j++) {
        for (i = 0; i < 54; i++) {
            data[i][j] = data[i][j]/data[5][j];
        }
    }
	}
	//source profile read
	{
	fptr2 = fopen(argv[2],"r");
	 if(fptr2 == NULL){
        printf("open failure");
        return 1;
    }else{
		bufferC = 0; 
		while(bufferC != '\n'){
			fscanf(fptr2,"%s%c",buffer,&bufferC);
			//printf("%s\n",buffer);
		}
		bufferC = 0; 
		for(i = 0;i < sourceLength;i++){
			bufferC = 0; 
			j = 0;
			for(k = 0;k < 1000;k++)
				buffer[k] = '\0';
			int nonfirst = 0;
			while(!(buffer[0] >= '0' && buffer[0] <= '9')){
				fscanf(fptr2,"%s",buffer);
				if(!(buffer[0] >= '0' && buffer[0] <= '9')){
					if(nonfirst)
						strcat(sourceName[i]," ");
					strcat(sourceName[i],buffer);
					nonfirst = 1;
				}
				//printf("%s",buffer);
			}
			while(bufferC != '\n'){
				fscanf(fptr2,"%lf%c",&sourcePro[j][i],&bufferC);
				//printf("%lf %c",sourcePro[j][i],bufferC);
				j++;
			}
		}
	}
	}
	printf("\n============\nalgo.2 first phrase\n============\n\n");
	//1.sort各個source的污染物(由大大小)，篩出source前三大與factor前三大至少有兩個為同一種物種的source
	for(i = 0;i < 54;i++){
		for(j = 0;j < count;j++){
			dataCopy[i][j] = data[i][j];//copy data for sorting
		}
	}
	for(i = 0;i < 54;i++){
		for(j = 0;j < count;j++){
			dataSort[i][j] = i;//assign 0~53 to memorize what pollutants is that
		}
	}
	for(k = 0;k < count;k++){
		for(j = 0; j < 53;j++){
			for(i = 53;i > j;i--){
				if(dataCopy[i][k] > dataCopy[i-1][k]){
					SWAP(dataCopy[i][k],dataCopy[i-1][k]);
					SWAP(dataSort[i][k],dataSort[i-1][k]);
				}
			}
			//printf("%d ",dataSort[j][k]);
		}
		//printf("\n");
	}
	
	double sourcePro_copy[54][100] = {0};
	for(j = 0;j < sourceLength;j++)
		for(i = 0;i < 54;i++)
			sourcePro_copy[i][j] = sourcePro[i][j];
		
	for(j = 0;j < sourceLength;j++)
		for(i = 0;i < 54;i++)
			sourcePro_sort[i][j] = i;
		
	for(k = 0;k < sourceLength;k++)
		for(j = 0;j < 53;j++)
			for(i = 53;i > j;i--)
				if(sourcePro_copy[i][k] > sourcePro_copy[i-1][k]){
					SWAP(sourcePro_copy[i][k],sourcePro_copy[i-1][k]);
					SWAP(sourcePro_sort[i][k],sourcePro_sort[i-1][k]);
				}

	int factor_p = 0, source_p = 0;
	int match = 0;
	
	for(i=0;i<30;i++)
		for(j=0;j<100;j++)
			max_match_pollution[i][j] = 999;
	for(i=0;i<count;i++){
		printf("---factor:%d---\n",i);
		for(j = 0; j < sourceLength;j++){
			match = 0;
			match_data[i][match_index[i]][1] = 0;
			max_match_pollution[i][j] = 999;
			for(factor_p = 0; factor_p < Number_find_pollution;factor_p++)
				for(source_p = 0; source_p < Number_find_pollution;source_p++)
					if(sourcePro_sort[source_p][j] == dataSort[factor_p][i]){
						if(max_match_pollution[i][j] > factor_p)
							max_match_pollution[i][j] = factor_p;
						match++;	
						double r = data[factor_p][i] - sourcePro[source_p][j];
						r *= r;
						match_data[i][match_index[i]][1] += r;
						break;
					}
			if(match >= Number_match){
				printf("%s,%d\n",sourceName[j],j);
				match_data[i][match_index[i]][0] = j;
				match_data[i][match_index[i]][1]/= match;
				match_index[i]++;
			}
		}
		sortTwoArray(match_index[i],1,match_data[i]);
	}
	
	printf("\n============\nalgo.2 second phrase\n============\n\n");
	//2.算出篩選之source跟factor match到物種的平方和平均，選出平方和平均最小的source
	for(i=0;i<30;i++)
		for(j=0;j<100;j++)
			square_min_in_factor[i][j][0] = -1;

	for(i=0;i<count;i++){
		printf("---factor:%d---\n",i);
		square_min_index[i] = 0;
		double min = 9999;
		for(j = 0; j < match_index[i];j++){
			if(min > match_data[i][j][1]){
				square_min_index[i] = 0;
				square_min_in_factor[i][square_min_index[i]][0] = match_data[i][j][0];
				min = match_data[i][j][1];
				square_min_index[i] = 1;
			}
			else if(min == match_data[i][j][1]){
				square_min_in_factor[i][square_min_index[i]][0] = match_data[i][j][0];
				square_min_index[i]++;
			}
		}
		if(square_min_index[i]!=0)
			for(j = 0; j < square_min_index[i];j++)
				printf("%s\n",sourceName[(int)square_min_in_factor[i][j][0]]);
	}
	
	printf("\n============\nalgo.2 third phrase\n============\n\n");
	//若大小相同，則比較factor最大物種的平方和大小
	for(i = 0;i < count;i++){
		for(j = 0;j < match_index[i];j++){
			match_data[i][j][1] = calculateSquare(i,match_data[i][j][0],0);
			//printf("**%lf\n",square_min_in_factor[i][j][1]);
			}
		sortTwoArray(match_index[i],1,match_data[i]);
	}

	for(i = 0;i < count;i++){	
		for(j = 0;j < sourceLength;j++){
			int max_pollution = dataSort[0][i];
			int in_match_data = 0;
			for(k = 0;k < match_index[i];k++)
				if(match_data[i][k][0] == j)
					in_match_data = 1;
			if(sourcePro[max_pollution][j] != -999 && !in_match_data){ 
				match_redundant_data[i][match_redundant_index[i]][0] = j;
				double r = data[max_pollution][i] - sourcePro[max_pollution][j];
				r *= r;
				match_redundant_data[i][match_redundant_index[i]][1] = r;
				match_redundant_index[i]++;
				}
		}
		sortTwoArray(match_redundant_index[i],1,match_redundant_data[i]);
	}

	int answer[30];
	memset(answer,-1,sizeof(answer));
	
	for(i=0;i<count;i++)
		if(match_index[i]!=0)
			find_source(i,0,0,answer,0);
		
	for(i=0;i<count;i++)
		printf("---factor:%d---\n%s\n",i,sourceName[answer[i]]);
	
	printf("\n============\nalgo.3 third phrase\n============\n\n");
	//若大小相同，則比較所有物種之平方和平均。
	
	for(i=0;i<count;i++){
		for(j = 0; j < match_index[i];j++)
			match_data[i][j][1] = calculateSquare(i,match_data[i][j][0],1);
		sortTwoArray(match_index[i],1,match_data[i]);
	}

	for(i = 0;i < count;i++){		
		match_redundant_index[i] = 0;
		int in_match_data = 0;
		for(k = 0;k < match_index[i];k++)
			if(match_data[i][k][0] == j)
				in_match_data = 1;
		if(!in_match_data)
			for(j = 0;j < sourceLength;j++){
				match_redundant_data[i][match_redundant_index[i]][0] = j;
				match_redundant_data[i][match_redundant_index[i]][1] = calculateSquare(i,j,1);
				match_redundant_index[i]++;
			}
		sortTwoArray(match_redundant_index[i],1,match_redundant_data[i]);
	}

	memset(answer,-1,sizeof(answer));
	
	for(i=0;i<count;i++)
		if(match_index[i]!=0)
			find_source(i,0,0,answer,1);
		
	for(i=0;i<count;i++)
		printf("---factor:%d---\n%s\n",i,sourceName[answer[i]]);
	return 0;
}
