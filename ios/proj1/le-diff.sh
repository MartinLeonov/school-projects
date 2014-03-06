#!/bin/sh
if [ -f "./.le/.config" ] ; then
  if [ -z "`grep '^projdir' ./.le/.config`" ] ; then
    echo "V konfiguracnim souboru neni zapsana cesta projektove slozky"
    exit 1
  else
    dir="`grep '^projdir' ./.le/.config | sed 's/^projdir //'`"
    if [ -n "`grep '^ignore' ./.le/.config`" ] ; then
      ign_lst="`grep '^ignore' ./.le/.config | sed "s/ignore //" | tr '\n' '|' | sed 's/.$//'`"
    fi
  fi
else
  echo "Nenalezen konfiguracni soubor"
  exit 1
fi
if [ $# -eq 0 ] ; then
  if [ -z "`grep '^ignore' ./.le/.config`"  ] ; then
    fls_arr=$(find ./ $dir -maxdepth 1 -type f 2>/dev/null | sed "s/^.*\///" | sort -u | grep -vE "^\..*")
  else
    fls_arr=$(find ./ $dir -maxdepth 1 -type f 2>/dev/null | sed "s/^.*\///" | sort -u | grep -vE "^\..*" | grep -vE "$ign_lst")
  fi
else
  if [ -z "`grep '^ignore' ./.le/.config`" ] ; then
    fls_arr=$(find "$@" 2>/dev/null | grep -vE '^\..*')
  else
    fls_arr=$(find "$@" 2>/dev/null | grep -vE '^\..*' | grep -vE "$ign_lst")
  fi
fi
for i in ${fls_arr[@]} ; do
  if [ -f "$dir/$i" -a -f "./$i" ] ; then
    diff -u $dir/$i ./$i  
    if [ $? -eq 0 ] ; then
      echo ".: $i"
    fi
  else
    if [ -f "$dir/$i" ] ; then
      echo "C: $i"
    else
      echo "D: $i"
    fi
  fi
done
