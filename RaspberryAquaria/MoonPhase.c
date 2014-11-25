//============================================================================
//				Moon phase approximation
//============================================================================
int Moon_phase(int year,int month,int day)
{
    /*k
      Calculates the moon phase (0-7), accurate to 1 segment.
      0 = > new moon.
      4 => Full moon.
    */
    
    int g, e;

    if (month == 1) --day;
    else if (month == 2) day += 30;
    else // m >= 3
    {
        day += 28 + (month-2)*3059/100;

        // adjust for leap years
        if (!(year & 3)) ++day;
        if ((year%100) == 0) --day;
    }
    
    g = (year-1900)%19 + 1;
    e = (11*g + 18) % 30;
    if ((e == 25 && g > 11) || e == 24) e++;
    return ((((e + day)*6+11)%177)/22 & 7);
}