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
# Vytvoreni poli se soubory
if [ $# -ne 0 ] ; then # Pokud je spusten s parametry
  par_arr=("$@") # Vytvoreni pole z parametru
  for i in ${par_arr[@]} ; do
    if [ -f $dir/$i ] ; then # Overeni existence souboru
      if [ -z "`grep '^ignore' ./.le/.config`" ] ; then # Pokud neni stanoveny ignore -> bere vsechny soubory
        prj_arr[$c]=`find $dir/$i | sed 's/^.*\///' | grep -vE "^\..*"`
      else
        prj_arr[$c]=`find $dir/$i | sed 's/^.*\///' | grep -vE "^\..*" | grep -vE $ign_lst`
      fi
      c=$((c+1))
    fi
  done
else # Nejsou zadane argumenty
  if [ -z "`grep '^ignore' ./.le/.config`" ] ; then # Pokud neni stanoveny ignore -> bere vsechny soubory
    prj_arr=(`find $dir -maxdepth 1 -type f | sed 's/^.*\///' | grep -vE "^\..*"`)
  else
    prj_arr=(`find $dir -maxdepth 1 -type f | sed 's/^.*\///' | grep -vE "^\..*" | grep -vE $ign_lst`)
  fi
fi
if [ $# -ne 0 ] ; then
  c=1 # counter
  par_arr=("$@") # Vytvoreni pole z parametru
  for i in ${par_arr[@]} ; do
    if [ -f ./.le/$i ] ; then # Overeni existence souboru
      if [ -z "`grep '^ignore' ./.le/.config`" ] ; then # Pokud neni stanoveny ignore -> bere vsechny soubory
        ref_arr[$c]=`find ./.le/$i | sed 's/^.*\///' | grep -vE "^\..*"`
      else
        ref_arr[$c]=`find ./.le/$i | sed 's/^.*\///' | grep -vE "^\..*" | grep -vE $ign_lst`
      fi
      c=$((c+1))
    fi
  done
else # Nejsou zadane argumenty
  if [ -z "`grep '^ignore' ./.le/.config`" ] ; then # Pokud neni stanoveny ignore -> bere vsechny soubory
    ref_arr=(`find ./.le/ -maxdepth 1 -type f | sed 's/^.*\///' | grep -vE "^\..*"`)
  else
    ref_arr=(`find ./.le/ -maxdepth 1 -type f | sed 's/^.*\///' | grep -vE "^\..*" | grep -vE $ign_lst`)
  fi
fi
# Cyklus pro soubory z projdir
for i in ${prj_arr[@]} ; do
if [ -f ./$i -a -f ./.le/$i ] ; then # Soubor existuje ve vsech slozkach
  if [ -z "`diff $dir/$i ./.le/$i`" ] ; then # Soubor v projektove a referencni jsou stejne
    if [ -z "`diff ./.le/$i ./$i`" ] ; then # Soubor je ve vsech slozkach stejny (P1)
      echo ".: $i"
    else # Soubor se lisi v experimentalni slozce (P2)
      echo "M: $i"
    fi
  else
    if [ -z "`diff $dir/$i ./$i`" ] ; then # Lisi se jen v referencni slozce (P3)
      cp $dir/$i ./.le/
      echo "UM: $i"
    else # Soubory v projektove a experimentalni nejsou stejne
      if [ -z "`diff ./$i ./.le/$i`" ] ; then # Soubor se lisi jen v projektove slozce (P4)
        cp $dir/$i ./
        cp $dir/$i ./.le/
        echo "U: $i"
      else # Soubor se lisi ve vsech slozkach
        diff -u ./.le/$i $dir/$i | patch ./$i  &>/dev/null
        if [ $? -eq 0 ] ; then # Podarilo se patchnout soubor (P5I)
          cp $dir/$i ./.le/
          echo "M+: $i"
        else
          echo "M!: $i conflict!" # Neporadilo se (P5II)
        fi
        rm -f ./*.rej ./*.orig
      fi
    fi
  fi
else # Soubor chybi v experimentalni nebo referencni slozce
  if [ ! -f ./.le/$i ] ; then # Souor existuje v referencni -> Chybi v experimentalni (P6)
    echo "C: $i"
    cp $dir/$i ./
    cp $dir/$i ./.le/
  fi
fi
done
# Cyklus pro soubory z referencni slozky
for i in ${ref_arr[@]} ; do
  if [ -f ./.le/$i -a ! -f $dir/$i ] ; then # Pokud soubor existuje v referencni ale ne v projektove slozce (P7)
    rm -f ./.le/$i &>/dev/null
    rm -f ./$i &>/dev/null
    echo "D: $i"
  fi
done
