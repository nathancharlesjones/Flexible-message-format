Hey, folks! I was playing around with some code that's described in "Code Complete" about how to make a "flexible message format" and thought I'd share it here. I'm not sure the result I obtained (below) was exactly what the author, Steve McConnell, was intending, but it's an interesting academic exercise that may be of use to some folks. I welcome any and all questions, comments, or recommendations for improvement!

Problem statement: An application (here, a buoy management system) needs to record and print messages of various types and lengths. There are 20 known types of messages at the moment, but more may be created later so although their types and lengths will be known at compile-time, the functions that process them must be able to handle arbitrary-length and arbitrary-type messages. How do you structure this data in such a way as to do that?

The answer I think most folks would probably reach for at this point is an object-oriented approach: define a parent "message" data type and have subsequent messages extend that class. This would result in at least as many classes as there are messages, each with their own set of associated functions. Although this is probably the most straight-forward approach (though not without it's own set of difficulties in C), it may also be a slightly heavy-handed approach, since there's a lot of boiler-plate code that would need to get written in that case and each class would probably have it's own set of associated header and source files.

I wanted to try a different way. C/C++ already has a construct that allows for "arbitrary-type" data called `union`. A `union`, like `struct` defines a new data type, but instead of allocating memory for each of it's data members, a union OVERLAYS the data members, one on top of another, allocating only enough space for the largest data member. Only one data member is said to be "active" at a time (and trying to reference the others results in undefined behavior) but any of them can be written to, one after the other. Unions are often put inside a struct with other data members that hold information about which part of the union is active. See [here](https://www.tutorialspoint.com/cprogramming/c_unions.htm) or [here](https://www.geeksforgeeks.org/union-c/) for more about unions.

So let's use a union as the basis for our "arbitrary-type" messages. Our union will hold just a few basic data types, which we can use to hold specific pieces of information for each of our messages (such as a float for "latitude" or an integer for "number of samples"). We'll refer to each piece of information as a `field_t`. It might look like this:
```
typedef union {
    int   intVar;
    float floatVar;
    char  charVar;
} field_t;
```
But wait? How do we know which part of the union is active? Let's put our union in a struct and add an enumerated data type to indicate the active component. Let's also add a string to give our field a human-readable name. Since our struct contains not just our field but also some relevant information about each one, we'll call our new struct `fieldInfo_t`.
```
typedef enum { INT, FLOAT, CHAR, NUM_DATA_TYPES } dataType_t;
typedef struct
{
    dataType_t   type;
    union
    {
        int      intVar;
        float    floatVar;
        char     charVar;
    };
    const char * name;
} fieldInfo_t;
```
Wait, what happened to the tag for our union, `field_t`? I left it off on purpose, to make referencing the union data members easier. As written above, the data members in the union act like data members of the struct, so we can reference them in the following manner (assuming `myFieldInfo` is an instance of fieldInfo_t): `myFieldInfo.intVar`, `myFieldInfo.floatVar`, etc. This is called an "anonymous union". Had we left in the union type of `field_t` (with, say, an instance name of `myField`), then we could only reference the union's data members through that tag (e.g. `myFieldInfo.intVar` would become `myFieldInfo.myField.intVar`, `myFieldInfo.floatVar` would become `myFieldInfo.myField.floatVar`, etc). Otherwise, there's no difference (that I know of) between the two.

We can now create a generic message data type which is composed of these fieldInfo_t data types. To handle our requirement for "arbitrary-length", we'll compose each message of an array of `fieldInfo_t` variables and add an integer in the message to indicate how many fields there are. We'll also give each message a string name for readability.
```
#define MAX_FIELDS 10
typedef struct
{
    const char * msgName;
    int          numFields;
    fieldInfo_t  fields[MAX_FIELDS];
} genericMsg_t;
```
Let's say we have a "temperature" message that holds the average temperature measurement (a float) and the number of samples that were averaged to get that measurement (an integer). Here's how we'd build that message:
```
genericMsg_t temp1 = { "Temp msg",                              //msgName
                        2,                                      //numFields
                    { { FLOAT, .floatVar=4.5f, "Temp" },        //fields[0]
                      { INT,   .intVar=10,     "Samples" } } }; //fields[1]
```
The first two lines initialize the string name for our message and the number of fields. After that we initialize the first two elements in our fieldInfo_t array (the rest get initialized to 0). Each fieldInfo_t element requires a data type (using the enumeration), a value, and a string name. The syntax in the middle might seem weird, but it's our way of telling the compiler which element in our union is the active one and then setting it appropriately.

Now our program can operate on `genericMsg_t` variables, putting them in arrays and passing them to and from functions. We have all of the information we need to operate on them, since encoded in each message is the number of fields it contains and in each field is information about which part of the union is the active member. In my example, I build two sample messages and then iterate over each one to print them out. By assuming each message and field have the same format to be printed, I only need a single switch-case block in the innermost part of the loop to call the correct version of printf (based on whether the active member in the union is an int, a float, or a char).

Again, this may really be only an academic exercise. But one distinct advantage of this solution (as opposed to the object-oriented one described above), is that messages can be constructed at run-time with truly arbitrary data; this solution supports 59,049 types of messages (10 fields with 3 possible data types each), which would be completely unsustainable with an OOP approach. The biggest downside is that the types of data that are supported have to be hard-coded into the program (as int, float, and char are here) and the logic to operate on each message can be somewhat complicated. If we needed a variety of functions to be able to operate on these data types (e.g. compute atmospheric conditions based on "temperature" messages, have drastically different printing implementations for each message type, etc) then the OOP approach is probably preferable.

If you got this far, thanks for reading and happy hacking!