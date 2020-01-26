#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define NUMBER_OF_WUMPUS_RULES 56
#define NUMBER_OF_KNOWN_SYMBOLS 48
#define TOTAL_SYMBOLS 64

struct model
{
	char **symbols;
	int *values;
	char **wumpusKnowledgeBase;
	char *alpha;
	char **listOfAllSymbols; 
};

void initializeModel(struct model *initialModel);
void getModel(char *additionalKB,char *wumpusRules,char *alphaStatement);
int TT_Check_All(struct model *finalModel,int count);
struct model * extend(struct model *extendModel,int value,int count);
int PL_True(struct model *plModel,int value);
int getValue(char *string,struct model *plModel);
// void addMissingSymbols(struct model *additionalModel,char *rulesFile,char *secondFile);
void generateNotAlpha(struct model *alphaModel);
void freeModel(struct model *initialModel);
void addMissingSymbols(struct model *additionalModel);

int main(int narg,char **args)
{
	if(narg!=4)
	{
		printf("Wrong input\n");
		return 0;
	}
	getModel(args[2],args[1],args[3]);
	return 0;
}

void getModel(char *additionalKB,char *wumpusRules,char *alphaStatement)
{
	FILE *additionalFile = fopen(additionalKB,"r");
	if(additionalFile == NULL)
	{
		printf("File cannot be opened");
	}
	else
	{
		struct model initialModel;
		initializeModel(&initialModel);
		char*buffer = (char*)malloc(200);
		int i,j;
		for(i=0;i<NUMBER_OF_KNOWN_SYMBOLS;i++)
		{
			j=0;
			fgets(buffer,200,additionalFile);
			if(buffer[0]=='(')
			{
				while(buffer[j+5]!=')')
				{
					initialModel.symbols[i][j] = buffer[j+5];
					j++;
				}
				initialModel.symbols[i][j] = '\0';
				initialModel.values[i]=0;
			}
			else
			{
				//printf("Here");
				while(buffer[j]!=13 && buffer[j]!=' ' && buffer[j]!=10)
				{
				//	printf("Here");
					initialModel.symbols[i][j] = buffer[j];
					j++;
				}
				initialModel.symbols[i][j] = '\0';
				initialModel.values[i] = 1;
				//printf("%c-",buffer[0]);
			}	
			//printf("%s,%d,%d\n",initialModel.symbols[i],initialModel.values[i],strlen(initialModel.symbols[i]));
		}
		fclose(additionalFile);
		addMissingSymbols(&initialModel);
		// for(i=0;i<TOTAL_SYMBOLS;i++)
		// {
			// printf("%d.sym:%s,value:%d\n",i+1,initialModel.symbols[i],initialModel.values[i]);
		// }
		//printf("%d,%d",initialModel.values[0],initialModel.values[1]);
		FILE *wumpusRulesFile = fopen(wumpusRules,"r");
		int length;
		if(wumpusRulesFile == NULL)
		{
			printf("File cannot be opened");
		}
		else
		{
			for(i=0;i<NUMBER_OF_WUMPUS_RULES;i++)
			{
				fgets(buffer,200,wumpusRulesFile);
				length = strlen(buffer);
				if(feof(wumpusRulesFile))
				{
					buffer[length] = '\0';
				}
				else
					buffer[length-1] = '\0';
				strcpy(initialModel.wumpusKnowledgeBase[i],buffer);
				//printf("%s\n",initialModel.wumpusKnowledgeBase[i]);
			}
			fclose(wumpusRulesFile);
		}
		FILE *alphaFile = fopen(alphaStatement,"r");
		if(alphaFile == NULL)
		{
			printf("File cannot be opened");
		}
		else
		{
			i=0;
			fgets(buffer,200,alphaFile);
			while(buffer[i]!=10 && buffer[i]!=13)
			{
				initialModel.alpha[i] = buffer[i];
				i++;
			}
			initialModel.alpha[i] = '\0';
			fclose(alphaFile);
		}
		//printf("%s\n",initialModel.alpha);
		int result1,result2;
		FILE *fp = fopen("result.txt","w");
		if(fp == NULL)
		{
			printf("File cannot be opened");
		}
		else
		{
			result1 = TT_Check_All(&initialModel,NUMBER_OF_KNOWN_SYMBOLS);
			//printf("%d,%s\n",result1,initialModel.alpha);
	    	generateNotAlpha(&initialModel);
			result2 = TT_Check_All(&initialModel,NUMBER_OF_KNOWN_SYMBOLS);
			//printf("%d,%s\n",result2,initialModel.alpha);
			if(result1 && !result2)
			{
				fprintf(fp,"%s","Definitely True");
			}
			else if(result2 && !result1)
			{
				fprintf(fp,"%s","Definitely False");
			}
			else if(!result1 && !result2)
			{
				fprintf(fp,"%s","Possibly True,Possibly False");
			}
			else
				fprintf(fp,"%s","both true and false");
			fclose(fp);
		}
		//freeModel(&initialModel);
		//free(buffer);*/
	}
}

void freeModel(struct model *initialModel)
{
	int i = 0;
	
	for(i=0;i<TOTAL_SYMBOLS;i++)
	{
		free(initialModel->symbols[i]);
	}
	free(initialModel->symbols);
	free(initialModel->values);
	
	for(i=0;i<NUMBER_OF_WUMPUS_RULES;i++)
	{
		free(initialModel->wumpusKnowledgeBase[i]);
	}
	free(initialModel->wumpusKnowledgeBase);
	
	free(initialModel->alpha);
}

void generateNotAlpha(struct model *alphaModel)
{
	char *notAlpha = (char*)malloc(200);
	strcpy(notAlpha,"(not ");
	strcat(notAlpha,alphaModel->alpha);
	strcat(notAlpha,")");
	strcpy(alphaModel->alpha,notAlpha);
	//free(notAlpha);
}

int TT_Check_All(struct model *finalModel,int count)
{
	int c;
	if(count>(TOTAL_SYMBOLS-1))
	{
		//printf("%d.%d,%d-\n",z++,finalModel->values[count],count);
		if(PL_True(finalModel,1))
		{
			return PL_True(finalModel,2);
		}
		else
		{
			return 1;
		}
	}
	else
	{
		//printf("%d.%d,%d\n",z++,finalModel->values[count],count);
		//finalModel->values[count] = 1;
		c = count;
		count = count + 1;
		return TT_Check_All(extend(finalModel,1,c),count) && 
			   TT_Check_All(extend(finalModel,0,c),count);
	}
}
struct model * extend(struct model *extendModel,int value,int count)
{
	extendModel->values[count] = value;
	return extendModel;
}

int PL_True(struct model *plModel,int value)
{
	int result;
	if(value == 1)
	{
		//printf("\nKB:ABC:%d,%d,%d",plModel->values[0],plModel->values[1],plModel->values[2]);
		int i;
		for(i=0;i<NUMBER_OF_WUMPUS_RULES;i++)
		{
			result = getValue(plModel->wumpusKnowledgeBase[i],plModel);
		    //printf("%d,%s\n",result,plModel->wumpusKnowledgeBase[i]);
			if(!result)
				return 0;
		}
		return 1;
	}
	else
	{
		result = getValue(plModel->alpha,plModel);
	    //printf("%d,%s\n",result,plModel->alpha);
		return result;
	}
}

int getValue(char *string,struct model *plModel)
{
    int count=0;
    char *str = (char*)malloc(200);
    //strcpy(str,string);
    if(string[0]=='(')
    {
        
        count++;
        int i = count,j=0;
        while(string[i]!=' ')
        {
            count++;
            str[j] = string[i];
            j++;
            i++;
        }
        count++;
        str[j] = '\0';
        //printf("%s\n",str);
        if(!strcmp(str,"if") || !strcmp(str,"iff"))
        {
            char *subsentence1 = (char*)malloc(200);
            char *subsentence2 = (char*)malloc(200);
            i=0;
            int openBracket = 0,closeBracket = 0;
             if(string[count]=='(')
             {
                openBracket = 1;closeBracket = 0;
                subsentence1[i] = string[count];
                while(openBracket!=closeBracket)
                {
                    count++;
                    i++;
                    subsentence1[i] = string[count];
                    if(string[count]==')')
                    closeBracket = closeBracket + 1;
                    else if(string[count]=='(')
                    openBracket = openBracket + 1;
                }
                subsentence1[i+1]='\0';
                count++;
                openBracket = 0,closeBracket = 0;
                i=0;
                if(string[count]=='(')
                {
                    openBracket++;
                    subsentence2[i] = string[count];
                    while(openBracket!=closeBracket)
                    {
                        count++;
                        i++;
                        subsentence2[i] = string[count];
                        if(string[count]==')')
                        closeBracket = closeBracket + 1;
                        else if(string[count]=='(')
                        openBracket = openBracket + 1;
                    }
                    subsentence2[i+1] = '\0';
                }
                else
                {
                    while(string[count]!=')')
                    {
                        subsentence2[i] = string[count];
                        count++;
                        i++;
                    }
                    subsentence2[i] = '\0';
                }
            }
            else
            {
              while(string[count]!=' ' && string[count]!=10) 
              {
                  subsentence1[i] = string[count];
                  count++;
                  i++;
              }
              subsentence1[i] = '\0';
              //printf("%s\n",subsentence1);
              count++;
              i=0;
              if(string[count]=='(')
              {
                     openBracket++;
                     subsentence2[i] = string[count];
                     //printf("%d",closeBracket);
                     while(openBracket!=closeBracket)
                     {
                         count++;
                         i++;
                         //printf("%d,%d,%d",openBracket,closeBracket,count);
                         //printf("-%c\n",string[count]);
                         subsentence2[i] = string[count];
                         if(string[count]==')')
                         {
                            closeBracket = closeBracket + 1;
                         }
                         else if(string[count]=='(')
                         {
                            openBracket = openBracket + 1;
                         }
                     }
                     subsentence2[i+1] = '\0';
              }
                else
                {
                    while(string[count]!=')')
                    {
                        subsentence2[i] = string[count];
                        count++;
                        i++;
                    }
                    subsentence2[i] = '\0';
                }
             }
             //printf("%s,%s\n",subsentence1,subsentence2);
             if(!strcmp(str,"if"))
             {
                 if(!getValue(subsentence1,plModel))
                 return 1;
                 else
                 return getValue(subsentence2,plModel);
             }
             else
             {
                if(getValue(subsentence1,plModel)==getValue(subsentence2,plModel))
                return 1;
                else
                return 0;
             }
        }
        else if(!strcmp(str,"and") || !strcmp(str,"or") || !strcmp(str,"xor") || 
                  !strcmp(str,"not"))
        {
            int open=1,close=0,num_true=0;
            while(open!=close)
            {
                char *subsentence = (char*)malloc(200);
                int openBracket = 0,closeBracket = 0;
                i=0;
                if(string[count]=='(')
                {
                    openBracket = 1;closeBracket = 0;
                    subsentence[i] = string[count];
                    while(openBracket!=closeBracket)
                    {
                        count++;
                        i++;
                        subsentence[i] = string[count];
                        if(string[count]==')')
                        closeBracket = closeBracket + 1;
                        else if(string[count]=='(')
                        openBracket = openBracket + 1;
                    }
                    subsentence[i+1]='\0';
                    count++;
                }
                else
                {
                    while(string[count]!=' ' && string[count]!=')' && string[count]!=10) 
                    {
                      subsentence[i] = string[count];
                      count++;
                      i++;
                    }
                    subsentence[i] = '\0';
                }
                //printf("%s\n",subsentence);
                if(!strcmp(str,"and"))
                {
                    if(!getValue(subsentence,plModel))
                    return 0;
                }
                if(!strcmp(str,"or"))
                {
                    if(getValue(subsentence,plModel))
                    return 1;
                }
                if(!strcmp(str,"xor"))
                {
                    if(getValue(subsentence,plModel))
                    num_true++;
                    if(num_true>1)
                    return 0;
                }
                if(!strcmp(str,"not"))
                {
                    if(getValue(subsentence,plModel))
                    return 0;
                    else
                    return 1;
                }
                if(string[count]==')')
                close++;
                else
                count++;
            }
            if(!strcmp(str,"and"))
            return 1;
            if(!strcmp(str,"or"))
            return 0;
            if(!strcmp(str,"xor"))
            if(num_true==1)
            return 1;
        }
    }
    else
    {
        int i;
		for(i=0;i<TOTAL_SYMBOLS;i++)
		{
			if(!strcmp(string,plModel->symbols[i]))
				break;
		}
		//printf("val:%s,%s,%s\n",plModel->symbols[0],plModel->symbols[1],plModel->symbols[2]);
		//printf("val:%d,%s\n",plModel->values[i],plModel->symbols[i]);
		return plModel->values[i];
    }
	//free(str);
}

void initializeModel(struct model *initialModel)
{
	int i = 0;
	initialModel->symbols = (char**)malloc(200*TOTAL_SYMBOLS);
	if(initialModel->symbols)
	{
		for(i=0;i<TOTAL_SYMBOLS;i++)
		{
			initialModel->symbols[i] = (char*)malloc(200);
		}
	}
	
	initialModel->values = (int*)malloc(sizeof(int)*TOTAL_SYMBOLS);
	
	initialModel->wumpusKnowledgeBase = (char**)malloc(200*NUMBER_OF_WUMPUS_RULES);
	if(initialModel->wumpusKnowledgeBase)
	{
		//printf("Here");
		for(i=0;i<NUMBER_OF_WUMPUS_RULES;i++)
		{
			initialModel->wumpusKnowledgeBase[i] = (char*)malloc(200);
			// strcpy(initialModel->wumpusKnowledgeBase[i],"Adsasd");
			// printf("%s",initialModel->wumpusKnowledgeBase[i]);
		}
	}
	
	initialModel->alpha = (char*)malloc(200);
	
	initialModel->listOfAllSymbols = (char**)malloc(200*TOTAL_SYMBOLS);
	if(initialModel->listOfAllSymbols)
	{
		for(i=0;i<TOTAL_SYMBOLS;i++)
		{
			initialModel->listOfAllSymbols[i] = (char*)malloc(200);
		}
	}
	strcpy(initialModel->listOfAllSymbols[0],"M_1_1");
	strcpy(initialModel->listOfAllSymbols[1],"M_1_2");
	strcpy(initialModel->listOfAllSymbols[2],"M_1_3");
	strcpy(initialModel->listOfAllSymbols[3],"M_1_4");
	strcpy(initialModel->listOfAllSymbols[4],"M_2_1");
	strcpy(initialModel->listOfAllSymbols[5],"M_2_2");
	strcpy(initialModel->listOfAllSymbols[6],"M_2_3");
	strcpy(initialModel->listOfAllSymbols[7],"M_2_4");
	strcpy(initialModel->listOfAllSymbols[8],"M_3_1");
	strcpy(initialModel->listOfAllSymbols[9],"M_3_2");
	strcpy(initialModel->listOfAllSymbols[10],"M_3_3");
	strcpy(initialModel->listOfAllSymbols[11],"M_3_4");
	strcpy(initialModel->listOfAllSymbols[12],"M_4_1");
	strcpy(initialModel->listOfAllSymbols[13],"M_4_2");
	strcpy(initialModel->listOfAllSymbols[14],"M_4_3");
	strcpy(initialModel->listOfAllSymbols[15],"M_4_4");
	
	strcpy(initialModel->listOfAllSymbols[16],"S_1_1");
	strcpy(initialModel->listOfAllSymbols[17],"S_1_2");
	strcpy(initialModel->listOfAllSymbols[18],"S_1_3");
	strcpy(initialModel->listOfAllSymbols[19],"S_1_4");
	strcpy(initialModel->listOfAllSymbols[20],"S_2_1");
	strcpy(initialModel->listOfAllSymbols[21],"S_2_2");
	strcpy(initialModel->listOfAllSymbols[22],"S_2_3");
	strcpy(initialModel->listOfAllSymbols[23],"S_2_4");
	strcpy(initialModel->listOfAllSymbols[24],"S_3_1");
	strcpy(initialModel->listOfAllSymbols[25],"S_3_2");
	strcpy(initialModel->listOfAllSymbols[26],"S_3_3");
	strcpy(initialModel->listOfAllSymbols[27],"S_3_4");
	strcpy(initialModel->listOfAllSymbols[28],"S_4_1");
	strcpy(initialModel->listOfAllSymbols[29],"S_4_2");
	strcpy(initialModel->listOfAllSymbols[30],"S_4_3");
	strcpy(initialModel->listOfAllSymbols[31],"S_4_4");
	
	strcpy(initialModel->listOfAllSymbols[32],"P_1_1");
	strcpy(initialModel->listOfAllSymbols[33],"P_1_2");
	strcpy(initialModel->listOfAllSymbols[34],"P_1_3");
	strcpy(initialModel->listOfAllSymbols[35],"P_1_4");
	strcpy(initialModel->listOfAllSymbols[36],"P_2_1");
	strcpy(initialModel->listOfAllSymbols[37],"P_2_2");
	strcpy(initialModel->listOfAllSymbols[38],"P_2_3");
	strcpy(initialModel->listOfAllSymbols[39],"P_2_4");
	strcpy(initialModel->listOfAllSymbols[40],"P_3_1");
	strcpy(initialModel->listOfAllSymbols[41],"P_3_2");
	strcpy(initialModel->listOfAllSymbols[42],"P_3_3");
	strcpy(initialModel->listOfAllSymbols[43],"P_3_4");
	strcpy(initialModel->listOfAllSymbols[44],"P_4_1");
	strcpy(initialModel->listOfAllSymbols[45],"P_4_2");
	strcpy(initialModel->listOfAllSymbols[46],"P_4_3");
	strcpy(initialModel->listOfAllSymbols[47],"P_4_4");
	
	strcpy(initialModel->listOfAllSymbols[48],"B_1_1");
	strcpy(initialModel->listOfAllSymbols[49],"B_1_2");
	strcpy(initialModel->listOfAllSymbols[50],"B_1_3");
	strcpy(initialModel->listOfAllSymbols[51],"B_1_4");
	strcpy(initialModel->listOfAllSymbols[52],"B_2_1");
	strcpy(initialModel->listOfAllSymbols[53],"B_2_2");
	strcpy(initialModel->listOfAllSymbols[54],"B_2_3");
	strcpy(initialModel->listOfAllSymbols[55],"B_2_4");
	strcpy(initialModel->listOfAllSymbols[56],"B_3_1");
	strcpy(initialModel->listOfAllSymbols[57],"B_3_2");
	strcpy(initialModel->listOfAllSymbols[58],"B_3_3");
	strcpy(initialModel->listOfAllSymbols[59],"B_3_4");
	strcpy(initialModel->listOfAllSymbols[60],"B_4_1");
	strcpy(initialModel->listOfAllSymbols[61],"B_4_2");
	strcpy(initialModel->listOfAllSymbols[62],"B_4_3");
	strcpy(initialModel->listOfAllSymbols[63],"B_4_4");
}

// void addMissingSymbols(struct model *additionalModel,char *rulesFile,char *secondFile)
// {
	// //printf("%s",rulesFile);
    // FILE *file = fopen(rulesFile,"r");
	// if(file == NULL)
	// {
		// printf("Cannot open file");
	// }
	// else
	// {
		// char *string = (char*)malloc(200);
		// char *symbol = (char*)malloc(200);
		// int i,count,length,j,foundCheck,symCount=NUMBER_OF_KNOWN_SYMBOLS;
		// while(fgets(string,200,file))
		// {
            // //printf("%s\n",string);
			// if(symCount >= TOTAL_SYMBOLS)
			// break;
			// if(feof(file))
			// length = strlen(string);
			// else
			// length = strlen(string) - 3;
			// count = 0;
			// if(string[count]=='#' || string[count]==10 || string[count]==13)
			// {
				// continue;
			// }
			// while(1)
			// {
				// i=0,foundCheck=0;
				// //printf("val:%c,%d,%d\n",string[count],count,length);
				// if(count>=length)
				// {
					// break;
				// }
				// //printf("Here");
				// if(string[count]=='(')
				// {
					// while(string[count]!=' ')
					// count++;
					// count++;
				// }
				// else if(string[count]==')' || string[count]==' ')
				// {
					// count++;
				// }
				// else
				// {
					// while(string[count]!=' ' && string[count]!=')' && string[count]!=13 && string[count]!=0)
					// {
						// symbol[i] = string[count];
						// i++;
						// count++;
					// }
					// symbol[i] = '\0';
					// //printf("S:%s\n",symbol);
					// for(j=0;j<symCount;j++)
					// {
						// if(!strcmp(symbol,additionalModel->symbols[j]))
						// {
							// foundCheck = 1;
							// //printf("%s,%s\n",additionalModel->listOfAllSymbols[i],additionalModel->symbols[j]);
							// break;
						// }
					// }
					// if(foundCheck!=1)
					// {
						// strcpy(additionalModel->symbols[symCount],symbol);
						// symCount++;
						// foundCheck = 0;
					// }
				// }
			// }
			// if(feof(file))
			// {
				// break;
			// }
		// }
		// fclose(file);
		// // free(symbol);
		// // free(string);
	// }
// }

void addMissingSymbols(struct model *additionalModel)
{
	int i,j,foundCheck=0,count=NUMBER_OF_KNOWN_SYMBOLS;
	for(i=0;i<TOTAL_SYMBOLS;i++)
	{
		foundCheck = 0;
		for(j=0;j<count;j++)
		{
			//if(i==10)
			//printf("%d:%s,%s\n",i,additionalModel->listOfAllSymbols[i],additionalModel->symbols[j]);
			if(!strcmp(additionalModel->listOfAllSymbols[i],additionalModel->symbols[j]))
			{
				//printf("%d:%s,%s\n",i,additionalModel->listOfAllSymbols[i],additionalModel->symbols[j]);
				foundCheck = 1;
				break;
			}
		}
		if(foundCheck!=1)
		{
			//printf("%d:%s\n",i,additionalModel->listOfAllSymbols[i]);
			strcpy(additionalModel->symbols[count],additionalModel->listOfAllSymbols[i]);
			count++;
			foundCheck = 0;
		}
	}
}