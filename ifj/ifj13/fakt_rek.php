<?php


$z = put_string("Zadejte cislo pro vypocet faktorialu\n");
$a = get_string();
$a = intval($a);


function factorial($n)
{

    if ($n < 2)
     {
       $result = 1;
     }
    else
     {
       $decremented_n = $n - 1;
       $temp_result = factorial($decremented_n);
       $result = $n * $temp_result;
     }
     return $result;

}



if ($a < 0)
{


   $message = "Faktorial nelze spocitat\n";

}
else
{


   $vysl = factorial($a);
   $message = "Vysledek je: " . $vysl . "\n";

}
$zzz = put_string($message);

