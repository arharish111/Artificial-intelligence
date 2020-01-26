Name: Harish Harish
--------------------------------------------
Programming language:C
----------------------------------------------
Compile : gcc -o check check_true_false.c
Run: check wumpus_rules.txt [additional_knowledge_file] [statement_file]
----------------------------------------------------------------------------------------	 
Code Structure:

void initializeModel(struct model *initialModel);
---To initialize input.

void getModel(char *additionalKB,char *wumpusRules,char *alphaStatement);
----To perform all necessary actions