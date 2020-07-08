#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_FIELDS 10
#define NUM_MESSAGES 2

typedef enum {
    TEMPERATURE,
    LOCATION,
    NUM_BUOY_MSG_TYPES
} buoyMsgType_t;

typedef struct
{
    float avgTmp;
    int   numSamples;
} buoyMsg_Tmp_t;

typedef struct
{
    float latitude;
    char  NorS;
    float longitude;
    char  EorW;
} buoyMsg_Loc_t;

typedef struct
{
    buoyMsgType_t type;
    union
    {
        buoyMsg_Tmp_t tmp;
        buoyMsg_Loc_t loc;
    } msg;
} buoyMsg_t;

typedef enum { INT, FLOAT, CHAR , NUM_DATA_TYPES } dataType_t;

typedef struct
{
    dataType_t   type;
    size_t       offset;
    const char * name;
} fieldInfo_t;

typedef struct
{
    const char * msgName;
    int numFields;
    fieldInfo_t fields[MAX_FIELDS];
} buoyMsg_Descriptor_t;

buoyMsg_Descriptor_t Descriptors[NUM_BUOY_MSG_TYPES] =
{
    { "Temp", 2, { {FLOAT, offsetof(buoyMsg_t, msg) + offsetof(buoyMsg_Tmp_t, avgTmp),     "Average Temp"},
                   {INT,   offsetof(buoyMsg_t, msg) + offsetof(buoyMsg_Tmp_t, numSamples), "Number of Samples"} } },
    { "Loc",  4, { {FLOAT, offsetof(buoyMsg_t, msg) + offsetof(buoyMsg_Loc_t, latitude),   "Latitude"},
                   {CHAR,  offsetof(buoyMsg_t, msg) + offsetof(buoyMsg_Loc_t, NorS),       "N/S"},
                   {FLOAT, offsetof(buoyMsg_t, msg) + offsetof(buoyMsg_Loc_t, longitude),  "Longitude"},
                   {CHAR,  offsetof(buoyMsg_t, msg) + offsetof(buoyMsg_Loc_t, EorW),       "E/W"} } }
};

int main( int argc, char * argv[])
{
    buoyMsg_Tmp_t temp1 = { 4.5f, 15 };
    buoyMsg_Loc_t loc1 = { 33.4567f, 'N', 124.8724f, 'E' };
    buoyMsg_t messages[NUM_MESSAGES] = { { TEMPERATURE, {.tmp=temp1} },
                                         { LOCATION, {.loc=loc1}} };

    for(int msg_idx = 0; msg_idx < NUM_MESSAGES; msg_idx++)
    {
        buoyMsg_t thisMsg = messages[msg_idx];
        buoyMsg_Descriptor_t thisDescriptor = Descriptors[thisMsg.type];
        printf("---%s---\n", thisDescriptor.msgName);
        for(int field_idx = 0; field_idx < thisDescriptor.numFields; field_idx++)
        {
            fieldInfo_t thisField = thisDescriptor.fields[field_idx];
            switch(thisField.type)
            {
                case INT:
                    printf("%s: %d\n", thisField.name, *(int *)((uint8_t *)(&thisMsg)+thisField.offset) );
                    break;
                case FLOAT:
                    printf("%s: %f\n", thisField.name, *(float *)((uint8_t *)(&thisMsg)+thisField.offset) );
                    break;
                case CHAR:
                    printf("%s: %c\n", thisField.name, *(char *)((uint8_t *)(&thisMsg)+thisField.offset) );
                    break;
            }
        }
        printf("\n");
    }
}