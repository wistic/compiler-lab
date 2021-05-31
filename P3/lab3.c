#include <stdio.h>
#include <ctype.h>

void first(char[], char);
void addToResultSet(char[], char);

/* Global Variables */
int numOfProductions;
char productionSet[100][100];

int main()
{
	int i;
	char choice;
	char result[100];
	printf("How many number of productions? : ");
	scanf(" %d", &numOfProductions);

	printf("\nIf the production is A -> ABaac | $ 	\
			\nwhere, $ is Epsilon 					\
			\ninput it like A=ABaac A=$\n\n");

	for (i = 0; i < numOfProductions; i++)
	{
		printf("Enter productions Number %d : ", i + 1);
		scanf(" %s", productionSet[i]);
	}

	char choice_to_ask_more;
	do
	{
		printf("\n Find the first of : ");
		scanf(" %c", &choice);
		first(result, choice);
		printf("\n first(%c)= { ", choice);
		for (i = 0; result[i] != '\0'; i++)
			printf("%c ", result[i]);
		printf("}\n");
		printf("Do you want to ask more?");
		scanf(" %c", &choice_to_ask_more);
	} while (choice_to_ask_more == 'y' || choice_to_ask_more == 'Y');

	return 0;
}

void first(char *Result, char c)
{
	int i, j, k;
	char subResult[20];
	int foundEpsilon;
	subResult[0] = '\0';
	Result[0] = '\0';
	//If X is terminal, FIRST(X) = {X}.
	if (!(isupper(c)))
	{
		addToResultSet(Result, c);
		return;
	}
	//If X is non terminal
	//Read each production
	for (i = 0; i < numOfProductions; i++)
	{
		//Find production with X as LHS
		if (productionSet[i][0] == c)
		{
			//If X → ε is a production, then add ε to FIRST(X).
			if (productionSet[i][2] == '$')
				addToResultSet(Result, '$');
			//If X is a non-terminal, and X → Y1 Y2 … Yk
			//is a production, then add a to FIRST(X)
			//if for some i, a is in FIRST(Yi),
			//and ε is in all of FIRST(Y1), …, FIRST(Yi-1).
			else
			{
				j = 2;
				while (productionSet[i][j] != '\0')
				{
					foundEpsilon = 0;
					first(subResult, productionSet[i][j]);
					for (k = 0; subResult[k] != '\0'; k++)
						addToResultSet(Result, subResult[k]);
					for (k = 0; subResult[k] != '\0'; k++)
						if (subResult[k] == '$')
						{
							foundEpsilon = 1;
							break;
						}
					//No ε found, no need to check next element
					if (!foundEpsilon)
						break;
					j++;
				}
			}
		}
	}
	return;
}

void addToResultSet(char Result[], char val)
{
	int k;
	for (k = 0; Result[k] != '\0'; k++)
		if (Result[k] == val)
			return;
	Result[k] = val;
	Result[k + 1] = '\0';
}
