/*round.h 
Template function takes a value and a target to round to (defaults to 1). 
e.g. Round(525.345, .03) =525.36 ...Round(525.345) =525
For working with currency you can change the default value to .01 .
Template will return the multiple of 'to_nearest' closest to 'value'. 
This is more universal than rounding to a precision. You could round 
in 1/8th's by setting to_nearest = .125 . Even Round(499.99, 1000) =0 .

**********************NOTICE********************************
This file is free for use or modification as required. If you
improve upon it add your name, date & e-mail below.
Then e-mail a copy to the previous authors.
************************************************************ 
D. Schemenauer Feb. 2000.	<dschemen@telus.net>
*/
// ----------------------------------------------------------- 
#ifndef roundH 
#define roundH 
// ----------------------------------------------------------- 
template <class T> T Round(T value, double to_nearest =1) 
{ 
  // There is only one 'nearest' 0
  if(to_nearest ==0) 
     return 0;

  // Negative values of to_nearest cause errors
  else if(to_nearest <0)
     to_nearest*=-1;
      

  int v2; 

  if(value>0) 
  { 
     v2 =(value + to_nearest/2)/to_nearest; 
     return v2 * to_nearest; 
  } 
  else if(value<0) 
  { 
     v2 =(value - to_nearest/2)/to_nearest; 
     return v2 * to_nearest; 
  } 
  else 
     return 0; 
} 
// ---------------------------------------------------------- 
#endif 

