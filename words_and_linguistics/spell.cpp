/** 
 *  @brief      spells out numbers into words and/or words and digits 
 *              form, similar to wolfram alpha's spell function
 *
 *  @note       uses US names for large numbers
 * 
 *  @author     Ashish
 *  @version    1.0
 *  Copyright (C) 2015 Ashish, MPL 2.0 license
 *
 *  This Source Code Form is subject to the
 *  terms of the Mozilla Public License, v.
 *  2.0. If a copy of the MPL was not
 *  distributed with this file, You can
 *  obtain one at
 *  http://mozilla.org/MPL/2.0/
 *
 *  TODO      Performance optimization and code organization
 *            Reduce dependency on external libraries
 *            Make into library files
 *            Write test files
 *  
 */

#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<exception>
#include<stdexcept>

namespace spell_utilities {
    std::string word_for_ones[] =
        { "", "one", "two", "three", "four",
            "five", "six", "seven", "eight",
            "nine" };

    std::string word_for_teens[] =
        { "", "eleven", "twelve",   "thirteen",     "fourteen",
            "fifteen",  "sixteen",  "seventeen",    "eighteen",
            "nineteen" };

    std::string word_for_tens[] =
         { "", "ten",   "twenty",   "thirty",   "forty",
            "fifty",    "sixty",    "seventy",  "eighty",
            "ninety" };

    std::string name_for_big_numbers[] =
        { "", "thousand",           "million",          "billion",
            "trillion",             "quadrillion",      "quintillion",
            "sextillion",           "septillion",       "octillion",
            "nonillion",            "decillion",        "undecillion",
            "duodecillion",         "tredecillion",     "quattuordecillion",
            "quindecillion",        "sexdecillion",     "septendecillion",
            "octodecillion",        "novemdecillion",   "vigintillion",
            "unvigintillion",       "duovigintillion",  "trevigintillion",
            "quattuorvigintillion", "quinvigintillion", "sexvigintillion",
            "septenvigintillion",   "octovigintillion", "novemvigintillion",
            "trigintillion",        "untrigintillion",  "duotrigintillion"};

    const int MAX_DIGITS_ALLOWED = 99 + 2 + 1;  // duotrigintillion = 99 zeros + 100 = 2 zeros + 1 non-zero digit

    int string_to_integer(std::string const& input) {
        if (input.length() == 3) {
            return ( ((input[0] - '0') * 100)
                    + ((input[1] - '0') * 10)
                    + ((input[2] - '0') * 1));
        } else if (input.length() == 2) {
            return ((input[0] - '0') * 10
                    + (input[1] - '0') * 1);
        } else if (input.length() == 1) {
            return ((input[0] - '0') * 1);
        }

        return 0;
    }

    std::string integer_to_string(int const& input) {
        static char buf[4];  // str will have max 3 digit plus one for null

        snprintf(buf, sizeof(buf), "%d", input);

        return buf;
    }

    bool is_valid_number(const std::string& input) {
        int i;

        if (input.empty())
            return false;

        for (i = 0; input[i]; i++) {
            int digit = input[i] - '0';
            if (digit < 0 || digit > 9) return false;
        }

        if (i > MAX_DIGITS_ALLOWED) {
            return false;
        }

        return true;
    }

    int split_number_to_buckets_of_3_digits(std::string const& input, std::vector<std::string>& bucket) {
        const int bkt_length = 3;

        std::string adjusted_input;
        int pad = bkt_length - (input.length() % bkt_length);
        if (pad == 1) {
            adjusted_input += "0";
        } else if (pad == 2) {
            adjusted_input += "00";
        }
        adjusted_input += input;

        int bkt_no = 0;
        for (int i = 0; i < adjusted_input.size() ; i += bkt_length) {
            bucket.push_back(adjusted_input.substr(i, bkt_length));
            bkt_no++;
        }

        return bkt_no;
    }

    char* number_constraints(void) {
        static char rule[128];
        snprintf(rule, sizeof(rule), "Number must be a non-zero positive "
                                     "integer, should not exceed %d digits "
                                     "and may contain commas as digits "
                                     "separator",
                                     spell_utilities::MAX_DIGITS_ALLOWED);
        return rule;
    }
};  // namespace spell_utilities

/*
 * @brief  The spell implementation class
 * 
 * It takes a string of digits during construction and  
 * provides a specific set of member methods to access 
 * its members and to convert upon request into a string
 * of words and/or mix of words and numbers
 * 
 * The default constructur is disabled, to force
 * init of string of digits and reduce validations at
 * mutliple places of its use
 */

class Spell {
 private:
    std::string _number_in_digits;
    std::string _number_in_words;
    std::string _number_in_words_and_digits;

 public:
     explicit Spell(std::string const& given_number_in_digits);

     void set_number_in_digits(std::string const& given_number_in_digits);
     std::string number_in_digits(void);

     std::string number_in_words(void);
     std::string number_in_words_and_digits(void);

     void do_spell(void);

     int number_length(void);
};

/*
 * @brief        Parameterized constructor
 * @param        string of digits
 */
Spell::Spell(std::string const& given_number_in_digits)
        : _number_in_words(""),
            _number_in_words_and_digits("") {
    set_number_in_digits(given_number_in_digits);
}

/*
 * @brief        Setter for _number_in_digits member
 * @param        String of digits
 * @return       Nothing
 * @throws       runtime_error - if invalid digit string
 */
void Spell::set_number_in_digits(std::string const& given_number_in_digits) {
    std::string sanitized_number(given_number_in_digits.size(), 0);

    // remove comma separators
    sanitized_number.erase(std::remove_copy(
                            given_number_in_digits.begin(),
                            given_number_in_digits.end(),
                            sanitized_number.begin(),
                            ','),
                            sanitized_number.end());

    // remove preceding zeros and as a side-effect also takes care of only zeros input
    sanitized_number.erase(0, sanitized_number.find_first_not_of('0'));

    if (!spell_utilities::is_valid_number(sanitized_number)) {
        throw std::runtime_error(spell_utilities::number_constraints());
    }

    _number_in_digits = sanitized_number;
}

/*
 * @brief        Getter method for _number_in_digits member
 * @param        None
 * @return       The digit string
 */
std::string Spell::number_in_digits(void) {
    return _number_in_digits;
}

/*
 * @brief        Getter method for _number_in_words member
 * @param        None
 * @return       The word string
 */
std::string Spell::number_in_words(void) {
    return _number_in_words;
}

/*
 * @brief        Getter method for _number_in_words_and_digits member
 * @param        None
 * @return       The mixed word and digits string
 */
std::string Spell::number_in_words_and_digits(void) {
    return _number_in_words_and_digits;
}

/*
 * @brief        Get the no. of digits in the member digit string
 * @param        None
 * @return       The length of digit string
 */
int Spell::number_length(void) {
    return _number_in_digits.length();
}

/*
 * @brief        Converts the digit string into the word and 
 *               the mixed word and digit strings and stores
 *               them in respective class members
 * @param        None
 * @return       Nothing
 */
void Spell::do_spell(void) {
    _number_in_words = "";
    _number_in_words_and_digits = "";

    std::vector<std::string> bucket;

    int allocated_buckets = spell_utilities::split_number_to_buckets_of_3_digits(_number_in_digits, bucket);

    bool show_big_number_name = false;
    int big_number = allocated_buckets - 1;

    for (int i = 0; i < allocated_buckets; i++) {
        int number_from_bucket = spell_utilities::string_to_integer(bucket[i]);
        if (number_from_bucket != 0) {
            show_big_number_name = true;
            _number_in_words_and_digits += spell_utilities::integer_to_string(number_from_bucket) + " ";
        }

        // focus on the 100s
        if (number_from_bucket > 99) {
            int number = bucket[i][0] - '0';  // first digit in 123, i.e. 1
            _number_in_words += spell_utilities::word_for_ones[number] + " " + "hundred" + " ";
        }

        number_from_bucket %= 100;  // focus on the tens

        if (number_from_bucket > 10 && number_from_bucket < 20) {  // 11,12...19
            int number = number_from_bucket - 10;  // last digit in 14, i.e. 4
            _number_in_words += spell_utilities::word_for_teens[number] + " ";
            number_from_bucket = 0;  // no need to process ones place
        } else if (number_from_bucket >= 10) {  // 10,20...90
            int number = number_from_bucket / 10;  // first digit in 10, i.e. 1
            _number_in_words += spell_utilities::word_for_tens[number] + " ";
        }

        number_from_bucket %= 10;  // focus on the ones

        if (number_from_bucket > 0 && number_from_bucket < 10) {  // 1,2,...9
            _number_in_words += spell_utilities::word_for_ones[number_from_bucket] + " ";
        }

        if (show_big_number_name) {
            _number_in_words += spell_utilities::name_for_big_numbers[big_number] + " ";
            _number_in_words_and_digits += spell_utilities::name_for_big_numbers[big_number] + " ";
            show_big_number_name = false;
        }

        big_number--;
    }

    return;
}

/*
 * @brief        Shows the usage of the program
 * @param        The argv from main
 * @return       Nothing
 */
void show_usage(char *argv[]) {
    std::cout << "Usage: \n " << argv[0] << " the_number_to_spell \n\n";
    std::cout << spell_utilities::number_constraints() << std::endl;
}

// a basic demonstration of the spell class implementation
int main(int argc, char *argv[]) {
    if (argc > 1 && argv[1][0] == '-') {
        show_usage(argv);
    } else {
        std::string number;
        if (argc == 1) {
            std::cout << ">";
            std::cin >> number;
        } else {
            number = argv[1];
        }

        try {
            Spell spell(number);
            spell.do_spell();
            std::cout << "In words: " << spell.number_in_words() << std::endl;
            std::cout << "In words and digits: " << spell.number_in_words_and_digits() << std::endl;
            std::cout << "Number length: " << spell.number_length() << " digits" << std::endl;
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }

    return 0;
}
