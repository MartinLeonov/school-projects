#!/usr/bin/env python3
# -*- coding: utf-8 -^-
#CST:xblanc01

import sys
import argparse
import os
import re
import ckeyword


def main():
    params = Params()
    params = params.loadArgs()
    filenames = FileSource(params)
    filenames = filenames.getSource()
    
    # analýza načtených souborů
    result = list()
    for filename in filenames:
        analysed = Analyse(filename, params)
        result.append(analysed.analyseFile())

    # seřazení a sečtení výsledků
    result.sort()
    finalSum = sum([count[1] for count in result])
    result.append(("CELKEM:", finalSum))

    # vypsání výsledků
    outputString = ''
    maxStringLenght = max([len(filename + str(count))
                          for filename, count in result])
    for filename, count in result:
        indent = (maxStringLenght - len(filename + str(count))) * ' '
        outputString += (filename + indent + ' ' + str(count) + '\n')
    if params['output'] is not None:
        try:
            with open(params['output'], 'w',
                      encoding='iso-8859-2') as outputFile:
                outputFile.write(outputString)
        except:
            sys.stderr.write("'" + params['output'] + "' nelze načíst.\n")
            sys.exit(3)
    else:
        sys.stdout.write(outputString)


class Params:
    """Zpracování a kontrola argumentů"""

    def __init__(self):
        self.parameters = dict()

    def loadArgs(self):
        """Načte a zpracuje parametry z příkazové řádky"""
        parser = ArgParse(add_help=False)
        #      = argparse.ArgumentParser(add_help=False)
        parser.add_argument('--nosubdir', action='store_true')
        parser.add_argument('--help', action='store_true')
        parser.add_argument('--input', action='store')
        parser.add_argument('--output', action='store')
        parser.add_argument('-k', action='store_true')
        parser.add_argument('-o', action='store_true')
        parser.add_argument('-i', action='store_true')
        parser.add_argument('-c', action='store_true')
        parser.add_argument('-p', action='store_true')
        parser.add_argument('-w', action='store')
        self.parameters = parser.parse_args()
        self.parameters = self.parameters.__dict__
        self.checkParams()
        return self.parameters

    def checkParams(self):
        """Zkontroluje načtené parametry, pokud jsou nesprávně zadané,
        vypíše chybové hlášení a ukončí skript
        """
        error = False

        if self.parameters['help'] and len(sys.argv) != 2:
            error = True
        elif self.parameters['help']:
            self.printHelp()

        params = ['--help', '--output', '--input', '--nosubdir',
                  '-k', '-o', '-i', '-c', '-p', '-w']
        sys.argv = [arg.split('=')[0] for arg in sys.argv]
        for param in params:
            if sys.argv.count(param) > 1:
                error = True

        nonCombinable = ['k', 'o', 'i', 'w', 'c']
        nonCombinable = [key for key in nonCombinable if self.parameters[key]]
        if len(nonCombinable) != 1:
            error = True

        if error:
            sys.stderr.write(
                "Chyba v parametrech příkazové řádky\n"
                "Pro nápovědu spusťte skript s parametrem --help\n"
            )
            sys.exit(1)

    def printHelp(self):
        """Vypíše nápovědu na stdout a úspěšně ukončí skript"""
        sys.stdout.write(
            "použití proj2.py [--input=FILEORDIR] [--nosubdir] "
            "[--output=FILENAME] [-k] [-o]\n"
            "                 [-i][-w=PATTERN] [-c] [-p] [--help]\n"
            "\n"
            "skript analyzuje zdrojový kód v jazyce C podle standardu "
            "ISO C99 a vypíše\n"
            "statistiky komentářů, klíčových slov, operátorů a řetězců\n"
            "\n"
            "volitelné argumenty:\n"
            "  --help            vypíše tuto nápovědu a ukončí skript\n"
            "  --input=FILEORDIR vstupní soubor nebo složka s C kódy\n"
            "  --nosubdir        nebude prohledávat v podadresářích\n"
            "  --output=FILENAME výstupní soubor se statistikami\n"
            "  -k                vypíše počet výskytů klíčových slov\n"
            "  -o                vypíše počet výskytů jednoduchých operátorů\n"
            "  -i                vypíše počet výskytů identifikátorů\n"
            "  -w=PATTERN        vyhledá přesný textový řetězec a vypíše\n"
            "                    počet nepřekrývajících se výskytů\n"
            "  -c                vypíše celkový počet znaků komentářů\n"
            "  -p                soubory se budou vypisovat bez úplné cesty\n"
            "\n"
            "- parametry [-k] [-o] [-i] [-w] [-c] nelze kombinovat\n"
            "  vždy je potřeba zadat pouze jeden z nich\n"
            "- je li parametrem [-input=FILEORDIR] zadán konkrétní soubor,\n"
            "  nelze již použít parametr [--nosubdir]\n"
            "\n"
            "ukázka:\n"
            "./cst.py -c --output=outfile\n"
            "./cst.py -w=int --input=folder/with/cfile/ -p --nosubdir\n"
            ""
        )
        sys.exit(0)


class ArgParse(argparse.ArgumentParser):

    def error(self, _):
        """Nahrazení defaultní 'error' metody z třídy 'ArgumentParser'
        z knihovny 'argparse' za vlastní
        """
        sys.stderr.write(
            "Chyba v parametrech příkazové řádky\n"
            "Pro nápovědu spusťte skript s parametrem --help\n"
        )
        sys.exit(1)


class FileSource:
    """Načtení zdrojových souborů s C kódem"""

    def __init__(self, params):
        """Inicializuje instanční proměnné

        Parametry:
        params - zpracované parametry příkazové řádky
        """
        self.input = params['input']
        self.nosubdir = params['nosubdir']

    def getSource(self):
        """Vrací seznam s cestami nalezených *.c a *.h souborů"""
        cFiles = list()

        # není zadán --input, analyzují se soubory aktuálního adresáře
        if self.input is None:
            self.input = os.path.dirname(os.path.realpath(__file__))

        # nahrazení ~ za domovský adresář
        self.input = os.path.expanduser(self.input)

        if not os.path.isfile(self.input) and not os.path.isdir(self.input):
            sys.stderr.write("Adresar '" + self.input + "' neexistuje\n")
            sys.exit(2)
        # je zadán konkrétní soubor
        if os.path.isfile(self.input):
            cFiles.append(os.path.abspath(self.input))
            return cFiles
        # je zadán adresář, zrušeno vyhledávání v podadresářích
        if self.nosubdir:
            cFiles = [os.path.join(self.input, filename) for filename
                      in os.listdir(self.input) if self.isCFile(filename)]
        # je zadán adresář, vyhledávání v podadresářích je povoleno
        else:
            # TODO prepsat (komprehence)
            for dirname, dirnames, filenames in os.walk(self.input):
                for filename in filenames:
                    if self.isCFile(filename):
                        cFiles.append(os.path.join(dirname, filename))
        return cFiles

    def isCFile(self, filename):
        """Ověřuje, zda soubor vyhovuje požadavkům (*.c nebo *.h)

        Parametry:
        filename - cesta k načtenému souboru
        """
        return re.match('.+\.c$', filename) or re.match('.+\.h$', filename)


class Analyse:
    """Analyzuje soubor zaslaný v parametru"""

    def __init__(self, filename, params):
        """Inicializuje instanční proměnné

        Parametry:
        filename - analyzovaný souboru
        params   - zpracované parametry příkazové řádky
        """
        self.filename = filename
        self.params = params
        self.count = 0
        try:
            with open(filename, 'r', encoding='iso-8859-2') as readFile:
                self.content = readFile.read()
        except:
            if os.path.isfile(params['input']):
                sys.stderr.write("'" + filename + "' nelze načíst.\n")
                sys.exit(2)
            else:
                sys.exit(21)

    def analyseFile(self):
        """Podle parametru vybere správnou metodu, spustí analýzu.
        Vrací podle parametru buďto název souboru nebo absolutní cestu
        a výsledek analýzy
        """
        if self.params['k']:
            self.countKeywords()
        elif self.params['o']:
            self.countOperators()
        elif self.params['i']:
            self.countIdentificators()
        elif self.params['w']:
            self.countPattern()
        elif self.params['c']:
            self.countCommentChars()

        if self.params['p']:
            return os.path.basename(self.filename), self.count
        else:
            return os.path.abspath(self.filename), self.count

    def countKeywords(self):
        """Nalezne a sečte klíčová slova"""
        words = self.searchInContent(r'\b[_a-zA-Z][_a-zA-Z0-9]*\b')
        keywords = [word for word in words if word in ckeyword.list]
        self.count = len(keywords)

    def countOperators(self):
        """Nalezne a sečte jednoduché operátory"""
        # odstranění dekladace ukazatelů (* v tomto případě není operátor)
        self.content = re.sub(r'\b(?:char|const|double|float|int|long|short|'
                              'void|_Bool|_Complex)(?:\s*,?\s*\(?\s*\*+\s*'
                              '\(?\w*\)?)+', '', self.content)
        operatorsRegex = (
            # jeden operator nasledovany =
            '((?:<<|>>|\+|-|<|>|!|\*|\/|%|&|\||\^)='
            # operatory se dvemi znaky
            '|\|\||&&|<<|>>|--|\+\+|->|=='
            # jednoznakove operatory
            '|(?:\%|\&|\+|\-|\=|\/|\||\.[_a-zA-Z\s]|\*|\:q:|>|<|\!|~|\^))'
        )
        operators = self.searchInContent(operatorsRegex)
        self.count = len(operators)

    def countIdentificators(self):
        """Nalezne a sečte identifikátory"""
        words = self.searchInContent(r'\b[_a-zA-Z][_a-zA-Z0-9]*\b')
        identificators = [word for word in words if word not in ckeyword.list]
        self.count = len(identificators)

    def countPattern(self):
        """Nalezne a sečte výskyty textového řetězce (parametr -w)"""
        self.count += self.content.count(self.params['w'])

    def countCommentChars(self):
        """Sečte znaky komentářů včetně uvozujících znaků"""
        prevChar = '\00'
        comment = False
        oneLineComment = False
        string = False

        for char in self.content:
            # řetězce mimo a v komentářích
            if char == "\"":
                if not string and not comment:
                    string = True
                elif string:
                    string = False
                elif comment:
                    self.count += 1
                continue
            # počítání znaků v jednořádnkovém komentáři
            if not string:
                if comment and oneLineComment:
                    self.count += 1
                    if not prevChar == '\\' and char == '\n':
                        comment = False
                        oneLineComment = False
                # počítání znaků ve víceřádkovém komentáři
                elif comment and not oneLineComment:
                    self.count += 1
                    if prevChar == '*' and char == '/':
                        comment = False
                        oneLineComment = False
                # detekce začátku komentáře
                elif not comment:
                    if prevChar == '/' and char == '/':
                        comment = True
                        oneLineComment = True
                        self.count += 2
                    elif prevChar == '/' and char == '*':
                        comment = True
                        oneLineComment = False
                        self.count += 2
            prevChar = char

    def searchInContent(self, search):
        """Ze zdrojového textu odebere nevyhovující části, vyhledá
        a vrátí seznam požadovaných prvků zadané regulárním výrazem

        Parametry:
        search - zadaný regulární výraz
        """
        # odstranění escape sekvence
        self.content = re.sub(r'\\\n', '', self.content)
        # odstranění komentářů, řetězců a maker
        regex = re.compile(r'/\*(?:.|[\r\n])*?\*/|//.*$|\".*\"|#.*$|\'\\?.\'',
                           re.M)
        while regex.search(self.content):
            self.content = regex.sub('', self.content)
        # vyhledání prvků podle regulárního výrazu z parametru
        regex = re.compile(search)
        searched = regex.findall(self.content)
        return searched


if __name__ == '__main__':
    main()
