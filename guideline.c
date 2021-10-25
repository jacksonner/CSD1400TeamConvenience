/* Just a guideline */

/* #define */
#define CAPS 0
#define MORE_CAPS 1
#define EVEN_MORE CAPS 2

/* arrays */
int a_rray[CAPS][MORE_CAPS][EVEN_MORE_CAPS];
float a_another_one[CAPS][MORE_CAPS];

/* variables */
int number_of_minions;
int height_cm;
float coordinates;

/* indentation - if/else */
if (/* condition */) {
    //code here
}
    else if (/* condition */) {
        //more code here
    }
    else {
        //even more code here
    }

if (/* condition */) {
    //code here
}
    else {
        //code here
    }

/* indentation - ternary operators */
int something = (condition == true
    ? /* thing happens */
    : /* else new condition*/
    ? /* thing happens */
    :/* else this thing happens*/ );