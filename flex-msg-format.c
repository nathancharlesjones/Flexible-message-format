#include <stdio.h>

#define MAX_FIELDS 10
#define NUM_MESSAGES 2

typedef enum { INT, FLOAT, CHAR , NUM_DATA_TYPES } dataType_t;

typedef struct
{
	dataType_t type;
	union
	{
	    int   intVar;
	    float floatVar;
	    char  charVar;
	};
	const char * fieldName;
} fieldInfo_t;

typedef struct
{
	const char * msgName;
	int numFields;
	fieldInfo_t fields[MAX_FIELDS];
} genericMsg_t;

int main( int argc, char * argv[])
{
	genericMsg_t temp1 = {                              "Temp msg",
	                                                             2,
	                        { { FLOAT, .floatVar=4.5f,    "Temp" },
	                          {   INT,     .intVar=10, "Samples" } }
	                     };

	genericMsg_t loc1 = {                                  "Loc msg",
	                                                               4,
	                        { { FLOAT,  .floatVar=33.4567f,  "Lat" },
	                          {  CHAR,        .charVar='N',  "N/S" },
	                          { FLOAT, .floatVar=124.8724f, "Long" },
	                          {  CHAR,        .charVar='E',  "E/W" } }
	                    };
	
	genericMsg_t messages[NUM_MESSAGES] = { temp1, loc1 };

	for(int msg_idx = 0; msg_idx < NUM_MESSAGES; msg_idx++)
	{
	    genericMsg_t thisMsg = messages[msg_idx];
	    printf("---%s---\n", thisMsg.msgName);
	    for(int field_idx = 0; field_idx < thisMsg.numFields; field_idx++)
	    {
	        fieldInfo_t thisField = thisMsg.fields[field_idx];
	        switch(thisField.type)
			{
				case INT:
				    printf("%s: %d\n", thisField.fieldName, thisField.intVar);
				    break;
				case FLOAT:
				    printf("%s: %f\n", thisField.fieldName, thisField.floatVar);
				    break;
				case CHAR:
				    printf("%s: %c\n", thisField.fieldName, thisField.charVar);
				    break;
				default:
				    break;
			}
	    }
	    printf("\n");
	}
}