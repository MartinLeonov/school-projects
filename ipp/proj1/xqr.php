#!/usr/bin/php
<?php
#XQR:xblanc01

/**
 * předmět:  IPP
 * projekt:  proj1
 * varianta: XML Query
 * @author: Roman Blanco, xblanc01@stud.fit.vutbr.cz
 */

error_reporting(E_ERROR | E_PARSE);

// načtení parametrů
$param = loadParams($argv);
if (isset($param["inputFile"])) {
    $xmlData = loadFile($param["inputFile"]);
} else {
    $xmlData = file_get_contents("php://stdin");
}

// načtení XML dat
try {
    $xmlData = new SimpleXMLElement($xmlData);
} catch (Exception $e) {
    report("Not valid XML format\n", 4);
}

// zpracování, vyhodnocení dotazu a získání konečného XML kódu
if (isset($param["query"])) {
    $query = splitQuery($param["query"]);
    if (!empty($query['FROM'][0]) || !empty($query['FROM'][1])) {
        $result = applyQuery($xmlData, $query);
    } else {
        $result = array();
    }
    $xml = (string) NULL;
    if (!empty($result)) {
        foreach ($result as $xmlPart) {
            $xml = $xml . $xmlPart->asXML();
        }
    }

    // vypsaní kořenu, pokud je nastaven
    if (isset($param['root'])) {
        $xml = (
            "<{$param['root']}>\n"
                . "$xml" .
            "\n</{$param['root']}>"
        );
    }

    // vypsaní hlavičky
    if (!isset($param['noXmlHead'])) {
        $xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>" . "$xml";
    }

    // vypsaní XML kódu
    if (isset($param["outputFile"])) {
        writeToFile($param["outputFile"], $xml);
    } else {
        file_put_contents("php://stdout", $xml);
    }
}


/**
 * Ověření a načtení parametrů skriptu
 * @param array
 * @return array
 */

function loadParams($argv)
{
    // ověření správnosti parametrů (getopt chybně čte -input jako -n apod.)

    // prepinace
    $paramsSet[0] = "/^(--help)$/";
    $paramsSet[1] = "/^(-n)$/";
    // adresy souboru
    $paramsSet[2] = "/^--input=(.*)$/";
    $paramsSet[3] = "/^--output=(.*)$/";
    $paramsSet[4] = "/^--qf=(.*)$/";
    // retezce
    $paramsSet[5] = "/^--query=(.*)$/";
    $paramsSet[6] = "/^--root=(.*)$/";
    // spoustec
    $paramsSet[7] = "/^.*\.php$/";

    foreach ($argv as $testedArg) {
        for ($i = 0; ($i <= count($paramsSet) and empty($result)); $i++) {
            preg_match($paramsSet[$i], $testedArg, $result);
        }
        if (empty($result[0])) {
            report("Bad params. Try --help\n", 1);
        } else {
            unset($result);
        }
    }

    $shortopts = "n";
    $longopts = array(
        "help",
        "input:",
        "output:",
        "query:",
        "qf:",
        "root::",
    );

    $opts = getopt($shortopts, $longopts);

    // kontrola, zda byly parametry zadány správně
    if ((count($argv)-1 != count($opts))
    or (array_key_exists("help", $opts) and (count($argv) != 2))
    or (isset($opts["query"]) and isset($opts["qf"])))
        report("Bad params. Try --help\n", 1);

    // vypsaní nápovědy
    if (array_key_exists("help", $opts))
        printHelp();

    $param = array();
    // načtení parametrů
    if (array_key_exists("n", $opts))
        $param["noXmlHead"] = TRUE;
    if (isset($opts["input"]))
        $param["inputFile"] = $opts["input"];
    if (isset($opts["output"]))
        $param["outputFile"] = $opts["output"];
    if (isset($opts["root"]))
        $param["root"] = $opts["root"];
    if (isset($opts["query"]))
        $param["query"] = $opts["query"];
    if (isset($opts["qf"]))
    // podle zprávý na fóru při neexistujícím souboru s dotazem vracet 80
    // podle zadani vracet 80 při syn. nebo sem. chybě v dotazu
    // -> ?
    if (file_exists($opts["qf"]))
        $param["query"] = loadFile($opts["qf"]);
    else
        report("File '{$opts['qf']}' does not exist\n", 80);

    return $param;
}


/**
 * Vypsaní nápovědy a úspěsné ukončení skriptu
 */

function printHelp()
{
    echo (
        "Usage:".
        "xqr.php [options]\n\n".
        "Options:\n".
        "--input=<file>    Load XML file that will be processed\n".
        "--output=<file>   Choose file, where should be result saved\n".
        "--qf=<file>       Task saved in external file,\n".
        "                  can't be combinated with --query=<\"task>\"\n".
        "--query=\"<task>\"  Task that will be executed on xml file\n".
        "--root=element    Name of xml root\n".
        "-n                Do not generate xml head on script output\n".
        "--help            Print this help\n\n".
        "Example:\n".
        "./xqr.php --input=plants.xml --output=result.xml --qf=query\n".
        "./xqr.php --input=plants.xml -n --query=\"SELECT price ".
        "FROM cars WHERE (year > 2010)\"\n"
    );
    exit(0);
}


/**
 * Načtení obsahu souboru
 * @param string
 * @return string
 */

function loadFile($file)
{
    if (file_exists($file)) {
        if (is_readable($file)) {
            $openedFile = fopen($file, "r");
            $content = fread($openedFile, filesize($file));
            fclose($openedFile);
            return $content;
        } else { report("Cannot open file '{$file}'\n", 2); }
    } else { report("File '{$file}' does not exist\n", 2); }
}


/**
 * Zápis obsahu do souboru
 * @param string
 * @param string
 * @return void
 */

function writeToFile($file, $content)
{
    if (is_writable($file)) {
        if (!$openedFile = fopen($file, "w")) {
            report("Cannot write to file '{$file}'\n", 3);
        }
    } else {
        $openedFile = fopen($file, "x");
    }
    fwrite($openedFile, $content);
    fclose($openedFile);
    return;
}


/**
 * Informování o chybě, ukončení s příslušnou návratovou hodnotou
 * @param string
 * @param integer
 */

function report($message, $exitValue)
{
    file_put_contents("php://stderr", $message);
    exit($exitValue);
}


/**
 * Načtení částí XML dotazu do slovníku regulerními výrazy
 * @param string
 * @return array
 */

function splitQuery($query)
{
    $queryOrder = "/(SELECT.*(LIMIT)?.*FROM.*(WHERE)?.*(ORDER BY)?.*)/";
    preg_match($queryOrder, $query, $queryOrderTest);
    if (empty($queryOrderTest))
        report("Semantic error in query\n", 80);

    $parsSet[0]['STRING'] = "SELECT";
    $parsSet[0]['REGEX'] = "/(?<=SELECT\s)\w+/";

    $parsSet[1]['STRING'] = "LIMIT";
    $parsSet[1]['REGEX'] = "/(?<=LIMIT\s)\d+/";

    $parsSet[2]['STRING'] = "FROM";
    $parsSet[2]['REGEX'] = "/(?<=FROM\s)(\w+)?(?:\.?(\w+)?)/";

    $parsSet[3]['STRING'] = "WHERE";
    $parsSet[3]['REGEX'] = "/(?<=WHERE\s).*(?=\sORDER BY)?/";

    $parsSet[4]['STRING'] = "ORDER BY";
    $parsSet[4]['REGEX'] = "/(?<=ORDER BY\s)\w+(?:\.\w+)?/";

    $parsSet[5]['STRING'] = "ORDER BY";
    $parsSet[5]['REGEX'] = "/(?:ASC|DESC)$/";

    // načtení výrazů
    for ($i = 0; $i < count($parsSet); $i++) {
        if (strpos($query, $parsSet[$i]['STRING']) !== false) {
            preg_match($parsSet[$i]['REGEX'], $query,
                $queryPartsArr[$parsSet[$i]['STRING']]);
        }
    }

    // ověření správnosti zadaných výrazů (části se matchnuly)
    foreach ($queryPartsArr as $queryPart => $partName) {
        if (empty($queryPart) and (key($partName) != "FROM"))
            report("Semantic error in query\n", 80);
    }

    // rozdělení FROM z dotazu
    if (isset($queryPartsArr['FROM'])) {
        $queryPartsArr['FROM'] = preg_split("/[.]/",
            $queryPartsArr['FROM'][0]);
    }

    // rozdělení ORDER BY z dotazu
    if (isset($queryPartsArr['ORDER BY'])) {
        $queryPartsArr['ORDER BY'] = preg_split("/[.]/",
            $queryPartsArr['ORDER BY'][0]);
    }

    // rozdělení WHERE z dotazu
    if (isset($queryPartsArr['WHERE'])) {
        $splitWhereExpr = "/^((?:NOT\s)*)(\w+)?(?:\.(\w+)?)?\s+"
                        . "(CONTAINS|=|>|<)\s+"
                        . "(\\\"(?:\w|\s)*\\\"|-?\d+(?:\\.\d+)?\b|\\\"\\\")/";
        preg_match($splitWhereExpr, $queryPartsArr['WHERE'][0],
            $queryPartsArr['WHERE']);
        // abych se zbavil celého matchnutého výrazu
        array_shift($queryPartsArr['WHERE']);
        // může být zanořená negace (NOT NOT) -> vyhodnocení:
        $negCount = substr_count($queryPartsArr['WHERE'][0], "NOT");
        if (($negCount % 2) == 0) {
            $queryPartsArr['WHERE'][0] = (string) NULL;
        } else {
            $queryPartsArr['WHERE'][0] = "NOT";
        }

        // ověření správnosti query WHERE části dotazu

        // pokud se výraz správně matchnul, má 5 položek
        if  ((count($queryPartsArr['WHERE']) != 5)
        // CONTAINS nepovoluje jako parametr číslo
        or  ($queryPartsArr['WHERE'][3] == "CONTAINS")
        and (is_numeric($queryPartsArr['WHERE'][4]))
        // element nesmí začínat číslem
        or  (isset($queryPartsArr['WHERE'][1][0]))
        and (is_numeric($queryPartsArr['WHERE'][1][0]))) {
            report("Semantic error in WHERE condition\n", 80);
        }
    }

    return $queryPartsArr;
}


/**
 * Vybere vyhovujici polozky z XML dat na zaklade dotazu
 * @param SimpleXmlElement
 * @param array
 * @return SimpleXmlElement
 */

// TODO zrušit XPath a vyhledávat ručně
function applyQuery($data, $query)
{
    $xpathCmdFrom = (string) NULL;
    $xpathCmdSelect = (string) NULL;
    $xpathCmdWhere = (string) NULL;

    // FROM
        // element: $query['FROM'][0]
        // attribute: $query['FROM'][1]
    if (!isset($query['FROM'])) {
        return (string) NULL;
    } else {
        if ($query['FROM'][0] == "") {
            $xpathCmdFrom = $xpathCmdFrom . "//*";
        } else if ($query['FROM'][0] == "ROOT") {
            $xpathCmdFrom = $xpathCmdFrom . "//*";
        } else {
            $xpathCmdFrom = $xpathCmdFrom . "//{$query['FROM'][0]}";
        }
        if (isset($query['FROM'][1]) and $query['FROM'][1] != "") {
            $xpathCmdFrom = $xpathCmdFrom . "[@{$query['FROM'][1]}]";
        }
    }

    // vyhodnocení části FROM
    $data = $data->xpath($xpathCmdFrom);

    // je hledán první výskyt (+ kontrola zda neni mezivysledek prazdky)
    $tmpResult = array_filter($data);
    if (!empty($tmpResult)) {
        $data = $data[0];
    } else {
        return (string) NULL;
    }

    // SELECT
        // element: $query['SELECT'][0]
    if (!isset($query['SELECT'])) {
        return (string) NULL;
    } else {
        $xpathCmdSelect = $xpathCmdSelect . ".//{$query['SELECT'][0]}";
    }

    // vyhodnocení části SELECT
    $data = $data->xpath($xpathCmdSelect);

    // WHERE (rozšiřuje příkaz pro SELECT)
        // NOT: $query['WHERE'][0]
        // element: $query['WHERE'][1]
        // attribute: $query['WHERE'][2]
        // relation-operator: $query['WHERE'][3]
        // literal: $query['WHERE'][4]
    if (isset($query['WHERE'])) {
        // pokud je operátorem CONTAINS
        if ($query['WHERE'][3] == "CONTAINS") {
            if (isset($query['WHERE']['2'])) {
                $xpathCmdWhere = $xpathCmdWhere
                    . "contains(@{$query['WHERE'][2]}, {$query['WHERE'][4]})";
            } else {
                $xpathCmdWhere = $xpathCmdWhere
                    . "contains(., {$query['WHERE'][4]})";
            }
        // pokud je operátorem porovnání = < >
        } else {
            if ($query['WHERE'][1] == "") {
                $xpathCmdWhere = $xpathCmdWhere . "*";
            }
            $xpathCmdWhere = $xpathCmdWhere . "{$query['WHERE'][1]}";
            if ($query['WHERE'][2] != "") {
                $xpathCmdWhere = $xpathCmdWhere . "[@{$query['WHERE'][2]}]";
            }
            switch ($query['WHERE'][3]) {
                case "=":
                    $xpathCmdWhere = $xpathCmdWhere
                        . "={$query['WHERE'][4]}";
                    break;
                case "<":
                    $xpathCmdWhere = $xpathCmdWhere
                        . "<{$query['WHERE'][4]}";
                    break;
                case ">":
                    $xpathCmdWhere = $xpathCmdWhere
                        . ">{$query['WHERE'][4]}";
                    break;
            }
        }
        if ($query['WHERE'][0] == "NOT") {
            $xpathCmdWhere = "[not({$xpathCmdWhere})]";
        } else {
            $xpathCmdWhere = "[{$xpathCmdWhere}]";
        }
        if ($query['SELECT'][0] != "") {
            $xpathCmdWhere = "//{$query['SELECT'][0]}{$xpathCmdWhere}";
        } else {
            $xpathCmdWhere = "//*{$xpathCmdWhere}";
        }

        // vyhodnocení části WHERE
        foreach($data as $item) {
            $data = $item->xpath($xpathCmdWhere);
        }
    }

    // LIMIT
        // number: $query['LIMIT'][0]
    if (isset($query['LIMIT'])) {
        $data = array_slice($data, 0, $query['LIMIT'][0]);
    }

    return $data;
}

?>
