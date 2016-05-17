#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<string.h>
#define SWAP(x, y) tmp = x; x = y; y = tmp;
#define sourceBuffer 30
#define pollutantBuffer 100
#define factorBuffer 30
#define Number_find_pollutant 6
#define Number_match 2
#define Algo4_num 3
#define Algo5_percent 60
#define Algo5_select 10

#define _Algo5_percent Algo5_percent * 0.01

double data[pollutantBuffer][factorBuffer]; //output3fac_profiles
int dataSort[pollutantBuffer][factorBuffer];
double sourcePro[pollutantBuffer][sourceBuffer]; //source
int sourceSortIndex[pollutantBuffer][sourceBuffer];
int sourcePro_sort[pollutantBuffer][sourceBuffer] = {0};
int factorCount;
int pollutantCount;
int sourceCount;
double tmp;
char sourceName[sourceBuffer][100];
char pollutantName[pollutantBuffer][100];

int match_index[factorBuffer] = {0};  // maximal number of match
double match_data[factorBuffer][100][2]; // 0 is index of source , 1 is square
int max_match_pollutant[factorBuffer][100];

int square_min_index[factorBuffer];
double square_min_in_factor[factorBuffer][100][2]; //index of source

int match_redundant_index[factorBuffer];
double match_redundant_data[factorBuffer][100][2]; //source only have the maximal pollutant and sort by square
char species[sourceBuffer][100][100];
int speciesIndex[sourceBuffer];

char getOS(){
	#if defined _WIN32 || defined _WIN64
	return '\n';
	#elif __APPLE__ || __MACH__
	return '\r';
	#else
    return '\n';
    #endif
}

double calculateSquare(int factor,int source,int method){
    //method 0 is factor最大物種的平方和大小 , method 1 is 所有物種之平方和平均
    double value = 0;
    if(method == 0){
        int pollutant = dataSort[max_match_pollutant[factor][source]][factor];
        double r = data[pollutant][factor] - sourcePro[pollutant][source];
        r *= r;
        value = r;
    }
    else if(method == 1){
        int avg_count = 0;
        int i = 0;
        double sum = 0;
        for(i=0;i<pollutantCount;i++)
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

void find_source(int factor,int index,int type,int answer[factorBuffer],int method){
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
	for(;i < factorCount;i++){
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

//排除不同factor有相同source的情況 ssssIndex[i][j]表示第j個factor第i優先的source
void sourceFit(int answer[factorBuffer], int j, int i, double sourceSortSumSq[sourceBuffer][factorBuffer], int ssssIndex[sourceBuffer][factorBuffer]){
    int current;//目前選的source
    int l = 0;
    current = ssssIndex[i][j];
    answer[j] = current;
    for (int k = 0; k < factorCount; k ++) {
        if (current == answer[k] && k != j) {
            for (int sourceIndex = 0; sourceIndex < sourceCount; sourceIndex++) {
                if (ssssIndex[sourceIndex][k] == answer[k]) {
                    l = sourceIndex;
                }
            }
            if (sourceSortSumSq[i][j] <= sourceSortSumSq[l][k]) sourceFit(answer, j, i+1, sourceSortSumSq, ssssIndex);
            else sourceFit(answer, k, l+1, sourceSortSumSq, ssssIndex);
        }
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

void readFile(char* argv[]){
	FILE *fptr, *fptr2, *fptr3;
	char buffer[1000];
	char bufferC = 0;
	char *pch;
	int bufferInt;
	int i = 0,j = 0,k = 0;
	char token = getOS();
	// data read	
	fptr = fopen(argv[1],"r");
    if(fptr == NULL){
        printf("open output8fac_profiles failure");
        exit(-1);
    }else{
		for(i = 0;i < 3;i++){
			while(strcmp(buffer,"Profiles")){
				fscanf(fptr,"%s%c",buffer,&bufferC);
			}
			strcpy(buffer,"");
		}
		bufferC = 0; 
		for(i = 0;i < 2;i++){
			while(bufferC != token){
				fscanf(fptr,"%s%c",buffer,&bufferC);
				//printf("%s\n",buffer);
			}
			bufferC = 0; 
		}
		//printf("%s\n",buffer);
		i = 0;
		k = 0;
		pollutantCount = 0;
		while(1){
			bufferC = 0;
			j = 0;
			if(fscanf(fptr,"%d %s",&bufferInt,buffer)==EOF)
				break;
			pch = strstr(buffer, "(ppbC)");
            *pch = '\0';
			strcpy(pollutantName[k],buffer);
			//printf("%s\n",buffer);
			while(bufferC != token){
				fscanf(fptr,"%lE%c",&data[i][j],&bufferC);
				//printf("%lf ",data[i][j]);
				j++;
				factorCount = j;
			}
			i++;
			pollutantCount++;
			k++;
		}
    }
	//data = data / n-butane
    for (j = 0; j < factorCount; j++) {
        for (i = 0; i < pollutantCount; i++) {
            data[i][j] = data[i][j]/data[5][j];
        }
    }
	
	//source profile read
	
	fptr2 = fopen(argv[2],"r");
	 if(fptr2 == NULL){
        printf("open source failure");
        exit(-1);
    }else{
		bufferC = 0; 
		while(bufferC != token){
			fscanf(fptr2,"%s%c",buffer,&bufferC);
			//printf("%s\n",buffer);
		}
		bufferC = 0; 
		int end = 0;
		i = 0;
		while(1){
			memset(buffer,'\0',sizeof(buffer));
			int nonfirst = 0;
			while(!(buffer[0] >= '0' && buffer[0] <= '9')){
				if(fscanf(fptr2,"%s",buffer) == EOF){
					end  = 1;
					break;
				}
				if(!(buffer[0] >= '0' && buffer[0] <= '9')){
					if(nonfirst)
						strcat(sourceName[i]," ");
					strcat(sourceName[i],buffer);
					nonfirst = 1;
				}
				//printf("%s",buffer);
			}
			if(end)
				break;
			bufferC = 0;
			j = 0;
			while(bufferC != token){
				fscanf(fptr2,"%lf%c",&sourcePro[j][i],&bufferC);
				//printf("%lf %c",sourcePro[j][i],bufferC);
				j++;
			}
			if(j!=pollutantCount){
				printf("number of pollution of fac_profiles and source are not the same\n");
				exit(-1);
			}
			i++;
			sourceCount++;
		}
	}
	fptr3 = fopen(argv[3],"r");
	 if(fptr3 == NULL){
        printf("open species failure");
        exit(-1);
    }
	bufferC = 0;
	i = 0;
	j = 0;
	int nonfirst = 0;
	while(fscanf(fptr3,"%s%c",buffer,&bufferC) != EOF){
		//printf("%s %d\n",buffer,bufferC);
		if (nonfirst == 1) {
			strcat(species[i][0]," ");
			strcat(species[i][0],buffer);
			if (bufferC != ' '){
				nonfirst = 0;
				j++;
			}
		}
		else if (bufferC == ' ') {
			memset(species[i][0],'\0',sizeof(species[i][0]));
			strcat(species[i][0],buffer);
			nonfirst = 1;
		}
		else{
			memset(species[i][j],'\0',sizeof(species[i][j]));
			strcpy(species[i][j],buffer);
			j++;
			if (bufferC == token) {
				speciesIndex[i] = j;
				i++;
				j = 0;
			}
		}
	}

	fclose(fptr);
	fclose(fptr2);
	fclose(fptr3);
}

int main(int argc, char* argv[])
{
    double dataCopy[100][factorBuffer];
    int i = 0,j = 0,k = 0;
    readFile(argv);
    printf("\n============\nalgo.2 first phrase\n============\n\n");
    //1.sort各個source的污染物(由大大小)，篩出source前三大與factor前三大至少有兩個為同一種物種的source
    for(i = 0;i < pollutantCount;i++){
        for(j = 0;j < factorCount;j++){
            dataCopy[i][j] = data[i][j];//copy data for sorting
        }
    }
    for(i = 0;i < pollutantCount;i++){
        for(j = 0;j < factorCount;j++){
            dataSort[i][j] = i;//assign 0~pollutantCount-1 to memorize what pollutants is that
        }
    }
    for(k = 0;k < factorCount;k++){
        for(j = 0; j < pollutantCount-1;j++){
            for(i = pollutantCount-1;i > j;i--){
                if(dataCopy[i][k] > dataCopy[i-1][k]){
                    SWAP(dataCopy[i][k],dataCopy[i-1][k]);
                    SWAP(dataSort[i][k],dataSort[i-1][k]);
                }
            }
            //printf("%d ",dataSort[j][k]);
        }
        //printf("\n");
    }
    
    double sourcePro_copy[pollutantBuffer][100] = {0};
    for(j = 0;j < sourceCount;j++){
        for(i = 0;i < pollutantCount;i++){
            sourcePro_copy[i][j] = sourcePro[i][j];
            sourceSortIndex[i][j] = i;
        }
    }
    
    for(j = 0;j < sourceCount;j++)
        for(i = 0;i < pollutantCount;i++)
            sourcePro_sort[i][j] = i;
    
    for(k = 0;k < sourceCount;k++)
        for(j = 0;j < pollutantCount-1;j++)
            for(i = pollutantCount-1;i > j;i--)
                if(sourcePro_copy[i][k] > sourcePro_copy[i-1][k]){
                    SWAP(sourcePro_copy[i][k],sourcePro_copy[i-1][k]);
                    SWAP(sourcePro_sort[i][k],sourcePro_sort[i-1][k]);
                    SWAP(sourceSortIndex[i][k], sourceSortIndex[i-1][k]);
                }
    
    int factor_p = 0, source_p = 0;
    int match = 0;
    
    for(i=0;i<factorBuffer;i++)
        for(j=0;j<100;j++)
            max_match_pollutant[i][j] = 999;
    for(i=0;i<factorCount;i++){
        printf("---factor:%d---\n",i);
        for(j = 0; j < sourceCount;j++){
            match = 0;
            match_data[i][match_index[i]][1] = 0;
            max_match_pollutant[i][j] = 999;
            for(factor_p = 0; factor_p < Number_find_pollutant;factor_p++)
                for(source_p = 0; source_p < Number_find_pollutant;source_p++)
                    if(sourcePro_sort[source_p][j] == dataSort[factor_p][i]){
                        if(max_match_pollutant[i][j] > factor_p)
                            max_match_pollutant[i][j] = factor_p;
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
    for(i=0;i<factorBuffer;i++)
        for(j=0;j<100;j++)
            square_min_in_factor[i][j][0] = -1;
    
    for(i=0;i<factorCount;i++){
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
    for(i = 0;i < factorCount;i++){
        for(j = 0;j < match_index[i];j++){
            match_data[i][j][1] = calculateSquare(i,match_data[i][j][0],0);
            //printf("**%lf\n",square_min_in_factor[i][j][1]);
        }
        sortTwoArray(match_index[i],1,match_data[i]);
    }
    
    for(i = 0;i < factorCount;i++){
        for(j = 0;j < sourceCount;j++){
            int max_pollutant = dataSort[0][i];
            int in_match_data = 0;
            for(k = 0;k < match_index[i];k++)
                if(match_data[i][k][0] == j)
                    in_match_data = 1;
            if(sourcePro[max_pollutant][j] != -999 && !in_match_data){
                match_redundant_data[i][match_redundant_index[i]][0] = j;
                double r = data[max_pollutant][i] - sourcePro[max_pollutant][j];
                r *= r;
                match_redundant_data[i][match_redundant_index[i]][1] = r;
                match_redundant_index[i]++;
            }
        }
        sortTwoArray(match_redundant_index[i],1,match_redundant_data[i]);
    }
    
    int answer[factorBuffer];
    memset(answer,-1,sizeof(answer));
    
    for(i=0;i<factorCount;i++)
        if(match_index[i]!=0)
            find_source(i,0,0,answer,0);
    
    for(i=0;i<factorCount;i++)
        printf("factor %d : %s\n",i+1,sourceName[answer[i]]);
    
    printf("\n============\nalgo.3 third phrase\n============\n\n");
    //若大小相同，則比較所有物種之平方和平均。
    
    for(i=0;i<factorCount;i++){
        for(j = 0; j < match_index[i];j++)
            match_data[i][j][1] = calculateSquare(i,match_data[i][j][0],1);
        sortTwoArray(match_index[i],1,match_data[i]);
    }
    
    for(i = 0;i < factorCount;i++){
        match_redundant_index[i] = 0;
        int in_match_data = 0;
        for(k = 0;k < match_index[i];k++)
            if(match_data[i][k][0] == j)
                in_match_data = 1;
        if(!in_match_data)
            for(j = 0;j < sourceCount;j++){
                match_redundant_data[i][match_redundant_index[i]][0] = j;
                match_redundant_data[i][match_redundant_index[i]][1] = calculateSquare(i,j,1);
                match_redundant_index[i]++;
            }
        sortTwoArray(match_redundant_index[i],1,match_redundant_data[i]);
    }
    
    memset(answer,-1,sizeof(answer));
    
    for(i=0;i<factorCount;i++)
        if(match_index[i]!=0)
            find_source(i,0,0,answer,1);
    
    for(i=0;i<factorCount;i++)
        printf("---factor:%d---\n%s\n",i,sourceName[answer[i]]);
    
    printf("\n============\nalgo.4 first phrase\n============\n\n");
    //印出source前三大污染物
    double sourceSortSumSq[sourceBuffer][factorBuffer];
    for(j = 0;j < sourceCount; j++){
        printf("%s : ",sourceName[j]);
        for(i = 0;i < Algo4_num; i++){
           printf("%s ",pollutantName[sourceSortIndex[i][j]]);
        }
        printf("\n");
    }
    
    //計算source前三大污染物種與factor該三物種的方均根
    double r = 0;
    for(k = 0;k < factorCount;k++){
        for(j = 0;j < sourceCount; j++){
            for(i = 0; i < Algo4_num; i++){
                r = data[sourceSortIndex[i][j]][k] - sourcePro[sourceSortIndex[i][j]][j];
                r *= r;
                sourceSortSumSq[j][k] +=r;
            }
            //printf("%lf ",sourceSortSumSq[k][j]);
        }
        //printf("\n");
    }
    
    //排序方均根及其index
    int ssssIndex[sourceBuffer][factorBuffer];
    memset(ssssIndex,-1,sizeof(ssssIndex));
    for(j = 0;j < factorCount;j++){
        for(i = 0;i < sourceCount; i++){
            ssssIndex[i][j] = i;
        }
    }
    for(k = 0;k < factorCount;k++){
        for(j = 0;j < sourceCount;j++){
            for (i = sourceCount-1; i > j ; i--) {
                if(sourceSortSumSq[i-1][k] > sourceSortSumSq[i][k]){
                    SWAP(sourceSortSumSq[i-1][k], sourceSortSumSq[i][k]);
                    SWAP(ssssIndex[i-1][k], ssssIndex[i][k]);
                }
            }
        }
    }
    
    //找出每個factor對應的唯一最佳source
    memset(answer,-1,sizeof(answer));
    for (j = 0; j < factorCount; j++) {
        sourceFit(answer,j,0,sourceSortSumSq,ssssIndex);
    }
    //印出來
    printf("\n============\nalgo.4 third phrase\n============\n\n");
    for(i=0;i<factorCount;i++)
        printf("factor %d : %s\n",i+1,sourceName[answer[i]]);
    
	printf("\n============\nalgo.5 first phrase\n============\n\n");
	for (i = 0;i < factorCount;i++) {
		printf("factor %d: ", i);
		for (j = 0;j < Algo5_select;j++)
			printf("%s ",pollutantName[dataSort[j][i]]);
		puts("");
	}
	for (i = 0;i < factorCount;i++) {
		printf("---factor:%d---\n",i+1);
		for (k = 0; k < sourceCount; k++) {
			match = 0;
			for (j = 0;j < Algo5_select;j++){
				for (int l = 1; l < speciesIndex[k]; l++) {
					if (!strcmp(pollutantName[dataSort[j][i]],species[k][l])){
						//printf("%s ",pollutantName[dataSort[j][i]]);
						match++;
					}
				}
			}
			if (match * 1.0 /(speciesIndex[k] - 1) > _Algo5_percent)
					printf("%s\t%.1f%(%d / %d)\n" ,species[k][0] ,(match * 1.0 /(speciesIndex[k] - 1))*100 ,match ,speciesIndex[k] - 1);
		}
	}
	
    return 0;
}

