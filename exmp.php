<?php
  $a = 4;
  $b = 5;
  if ($a < $b)
    $b = $b + 3;
  print $b;
  while($a<$b){
    $a = $a + 3;
  }
  print $a;
  return $a;
?>