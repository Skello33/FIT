<?php


/** Prints a single line
 * @param $line - line to print
 */
function print_line($line)
{
    foreach ($line as $var) {
        echo "\ncurrent word is: $var \n\n";
    }
}

/** Controls instruction syntax and cuts it from the line
 * @param $instruction - line to check
 * @return string - instruction
 */
function get_instr($instruction)
{
    if (find_first_character($instruction) == '#') {
        return "COMMENT";
    }
    //EMPTY line WITH WHITESPACES
    if (find_first_character($instruction) == "\n"){
        return "\n";
    }
    $pattern = '/([A-Za-z0-9])+/';
    $found = preg_match($pattern, $instruction, $output);
    if ($found == 0) {
        return "NOT_FOUND";
    }
    return strtoupper($output[0]);
}

/** Cuts line into words
 * @param $input - input line
 * @return array[]|false|string[] - array of words
 */
function cut_line($input)
{
    $pattern = '/[\s]+/';

    $result = preg_split($pattern, $input, NULL, PREG_SPLIT_NO_EMPTY);
    return $result;
}

/**
 * @param $input
 * @return mixed
 */
function find_first_character($input)
{
    $i = 0;
    while (($input[$i] == "\t") || ($input[$i] == " ")) {
        $i++;
    }
    return $input[$i];
}

/** Removes comments from the line
 * @param $input - line to transform
 * @return mixed - line without comment
 */
function remove_comment($input)
{
    $pattern = '/#/';
    $result = preg_split($pattern, $input, NULL, PREG_SPLIT_NO_EMPTY);
    if (!$result) {
        echo "\n\n\nhere\n\n\n";
        var_dump($input);
    }
    return $result[0];
}

/** Reads separate lines from STDIN
 * @param $stdin - file descriptor for STDIN
 * @return mixed - line read from input
 */
function get_input($stdin)
{
    $i = 0;
    while ($output[$i] = fgets($stdin)) {
        //echo "here" . $i . "\n";
        $i++;
    }
    return $output;
}

/**
 * @param $input
 * @param $line_cnt
 */
function var_control($input)
{
    //$pattern = '/^((LF)|(GF)|(TF))@([\_\-\$\&\%\*\!\?\d\w])+$/'; ///old one
    $pattern = '/^((LF)|(GF)|(TF))@([\_\-\$\&\%\*\!\?a-zA-Z])+([\_\-\$\&\%\*\!\?\d\w])*$/';
    $value = preg_match($pattern, $input);
    if (!$value) {
        exit(23);
    }
}

/**
 * @param $input
 * @param $line_cnt
 */
function label_control($input)
{
    $pattern = '/^([\_\-\$\&\%\*\!\?\d\w])+$/';
    $value = preg_match($pattern, $input);
    if (!$value) {
        exit(23);
    }
}

/**
 * @param $input
 * @param $line_cnt
 */
function type_control($input)
{
    $pattern = '/^(int|string|bool)$/';
    $value = preg_match($pattern, $input);
    if (!$value) {
        exit(23);
    }
}

/**
 * @param $input
 * @param $line_cnt
 */
function symb_control($input)
{
    //$pattern = '/^(((LF)|(GF)|(TF))@..*)|(string@((\\\\\d\d\d)|([^\000-\040\043\134]))*$)|(bool@(true|false))|(int@(\+|\-)?[0-9]+)/';
    $pattern = '/^(((LF)|(GF)|(TF))@([\_\-\$\&\%\*\!\?a-zA-Z])+([\_\-\$\&\%\*\!\?\d\w])*)|(string@((\\\\\d\d\d)|([^\000-\040\043\134]))*$)|(bool@(true|false)$)|(int@(\+|\-)?[0-9]+$)|(nil@nil$)/';
    $value = preg_match($pattern, $input);
    if (!$value) {
        exit(23);
    }
}

/**
 * @param $count
 * @param $words_count
 * @param $line_cnt
 */
function count_control($count, $words_count)
{
    if ($count != $words_count) {
        exit(23);
    }
}

/**
 * @param $XMLoutput
 * @param $counter
 * @param $opcode
 */
function instruction_begin($XMLoutput, $counter, $opcode)
{
    global $XMLoutput;
    $XMLoutput = $XMLoutput . "    <instruction order=\"$counter\" opcode=\"$opcode\">\n";
}

/**
 * @param $XMLoutput
 */
function instruction_end($XMLoutput)
{
    global $XMLoutput;
    $XMLoutput = $XMLoutput . "    </instruction>\n";
    //echo "here";
}

//function short_instr($XMLoutput, $counter, $opcode) {
//    global $XMLoutput;
//    $XMLoutput = $XMLoutput . "    <instruction order=\"$counter\" opcode=\"$opcode\" />\n";
//}

/**
 * @param $arg
 * @param $argnum
 */
function var_gen($arg, $argnum)
{
    global $XMLoutput;
    $XMLoutput = $XMLoutput . "        <arg$argnum type=\"var\">$arg</arg$argnum>\n";
}

/**
 * @param $arg
 * @param $argnum
 */
function label_gen($arg, $argnum)
{
    global $XMLoutput;
    $XMLoutput = $XMLoutput . "        <arg$argnum type=\"label\">$arg</arg$argnum>\n";
}

/**
 * @param $arg
 * @param $argnum
 */
function type_gen($arg, $argnum)
{
    global $XMLoutput;
    $XMLoutput = $XMLoutput . "        <arg$argnum type=\"type\">$arg</arg$argnum>\n";
}

/**
 * @param $arg
 * @param $argnum
 */
function symb_gen($arg, $argnum)
{
    global $XMLoutput;
    $pattern = '/@/';
    $result = preg_split($pattern, $arg, NULL, PREG_SPLIT_NO_EMPTY);
    $symbol = $result[0];
    $count = count($result);

    switch ($symbol) {
        case "string":
            if ($count != 2) {
                $XMLoutput = $XMLoutput . "        <arg$argnum type=\"string\"/>\n";
                break;
            }
            $XMLoutput = $XMLoutput . "        <arg$argnum type=\"string\">$result[1]</arg$argnum>\n";
            break;
        case "int":
            $XMLoutput = $XMLoutput . "        <arg$argnum type=\"int\">$result[1]</arg$argnum>\n";
            break;
        case "bool":
            $XMLoutput = $XMLoutput . "        <arg$argnum type=\"bool\">$result[1]</arg$argnum>\n";
            break;
        case "LF":
        case "GF":
        case "TF":
            $XMLoutput = $XMLoutput . "        <arg$argnum type=\"var\">$arg</arg$argnum>\n";
            break;
    }
}

/**
 * @param $line_cnt
 * @param $XMLoutput
 * @param $opcounter
 * @return int
 */
function control_instr($line_cnt, $XMLoutput, $opcounter)
{
    global $XMLoutput;
    $instruction = get_instr($line_cnt);
    //remove comments
    $no_comments = remove_comment($line_cnt);

    $line_array = cut_line($no_comments);
    //count words in line
    $words_count = count($line_array);

    switch ($instruction) {
        case "MOVE":
            count_control(3, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            instruction_end($XMLoutput);
            break;

        case
        "COMMENT":
            break;

        case "CREATEFRAME":
            count_control(1, $words_count);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            instruction_end($XMLoutput);
            break;

        case "PUSHFRAME":
            count_control(1, $words_count);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            instruction_end($XMLoutput);
            break;

        case "POPFRAME":
            count_control(1, $words_count);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            instruction_end($XMLoutput);
            break;

        case "DEFVAR":
            count_control(2, $words_count);
            var_control($line_array[1]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            instruction_end($XMLoutput);
            break;

        case "CALL":
            count_control(2, $words_count);
            label_control($line_array[1]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            label_gen($line_array[1], 1);
            instruction_end($XMLoutput);
            break;

        case "RETURN":
            count_control(1, $words_count);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            instruction_end($XMLoutput);
            break;

        case "PUSHS":
            count_control(2, $words_count);
            symb_control($line_array[1]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            symb_gen($line_array[1], 1);
            instruction_end($XMLoutput);
            break;

        case "POPS":
            count_control(2, $words_count);
            var_control($line_array[1]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            instruction_end($XMLoutput);
            break;

        case "ADD":
            count_control(4, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case "SUB":
            count_control(4, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case "MUL":
            count_control(4, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case"IDIV":
            count_control(4, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case "LT":
        case "GT":
        case "EQ":
            count_control(4, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case "AND":
        case "OR":
            count_control(4, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case "NOT":
            count_control(3, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            instruction_end($XMLoutput);
            break;

        case "INT2CHAR":
            count_control(3, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            instruction_end($XMLoutput);
            break;

        case "STRI2INT":
            count_control(4, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case "READ":
            count_control(3, $words_count);
            var_control($line_array[1]);
            type_control($line_array[2]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            type_gen($line_array[2], 2);
            instruction_end($XMLoutput);
            break;

        case "WRITE":
            count_control(2, $words_count);
            symb_control($line_array[1]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            symb_gen($line_array[1], 1);
            instruction_end($XMLoutput);
            break;

        case "CONCAT":
            count_control(4, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case "STRLEN":
            count_control(3, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            instruction_end($XMLoutput);
            break;

        case "GETCHAR":
            count_control(4, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case "SETCHAR":
            count_control(4, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case "TYPE":
            count_control(3, $words_count);
            var_control($line_array[1]);
            symb_control($line_array[2]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            var_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            instruction_end($XMLoutput);
            break;

        case "LABEL":
            count_control(2, $words_count);
            label_control($line_array[1]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            label_gen($line_array[1], 1);
            instruction_end($XMLoutput);
            break;

        case "JUMP":
            count_control(2, $words_count);
            label_control($line_array[1]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            label_gen($line_array[1], 1);
            instruction_end($XMLoutput);
            break;

        case "JUMPIFEQ":
            count_control(4, $words_count);
            label_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            label_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case "JUMPIFNEQ":
            count_control(4, $words_count);
            label_control($line_array[1]);
            symb_control($line_array[2]);
            symb_control($line_array[3]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            label_gen($line_array[1], 1);
            symb_gen($line_array[2], 2);
            symb_gen($line_array[3], 3);
            instruction_end($XMLoutput);
            break;

        case "EXIT":
            count_control(2, $words_count);
            symb_control($line_array[1]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            symb_gen($line_array[1], 1);
            instruction_end($XMLoutput);
            break;

        case "DPRINT":
            count_control(2, $words_count);
            symb_control($line_array[1]);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            symb_gen($line_array[1], 1);
            instruction_end($XMLoutput);
            break;

        case "BREAK":
            count_control(1, $words_count);
            instruction_begin($XMLoutput, $opcounter, $instruction);
            instruction_end($XMLoutput);
            break;
        case "\n":
        case "":
            //echo "NEWLINE\n";
            break;
        default:
            exit(22);
            break;
    }

    return 0;
}

/*** MAIN BODY ***/

// argument control
if ($argc > 1) {
    if ($argv[1] != "--help") {
        print "\nUnexpected argument!\n";
        exit(10);
    }
    echo "\nSkript typu filter (parse.php v jazyku PHP 7.3) nacita zo standardneho vstupu  zdrojovy kod v IPPcode18, zkontroluje lexikalnu a syntakticku spravnost kodu a vypise na standardny vystup XML reprezentaciu programu.\n\n"; //TODO
    exit(0);
}

$stdin = fopen('php://stdin', 'r');
$input = get_input($stdin);

// first line control
$input[0] = strtoupper($input[0]);
$input[0] = remove_comment($input[0]);
//remove whitespaces from first line
$input[0] = preg_replace('/\s+/', '', $input[0]);
if ($input[0] != ".IPPCODE19" && $input[0] != ".IPPCODE19\n") {
    exit(21);
}

$XMLoutput = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<program language=\"IPPcode19\">\n";

$counter = 0;
foreach ($input as $line) {
    if ($counter != 0) {
        //start of file control
        if ($line == ".IPPCODE19\n") {
            exit(23);
        }
        if (($line != "") && ($line != "\n")) {
            if (control_instr($line, $XMLoutput, $counter) == 22) {
                //echo "error 22!!!\n";
                exit(22);
            }
        }
        if (find_first_character($line) == "#") {
            $counter--;
        }
        if (find_first_character($line) == "\n") {
            $counter--;
        }
    }
    $counter++;
}

$XMLoutput = $XMLoutput . "</program>";

echo $XMLoutput . "\n";

//echo "\nScript has ended - no ERROR\n";
return 0;




