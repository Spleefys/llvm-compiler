<?php
  $a = 4;
  $b = [5,7,9];
  $c = "<";
  if ($a < $b[0])
    $a = $b[0];
  print $a;
  echoc $c;
  print $b[2];
  while($a < $b[2]){
    $a=$a + 1;
  }
  print $a;
  echo "Hello";
  return $a;
?>