#include <string>
#include <vector>
#include <iostream>
#include <omp.h>

const char space = ' ';
const char newline = '\n';
const char tab = '\t';

const std::vector<char> separators{space, newline, tab};

int countWords(std::string line);
bool isSeparator(char symbol);

int main(int argc, char *argv[]) {
    std::vector<std::pair<std::string, int>> samples{
        std::make_pair("hello", 1),
        std::make_pair("hello bro", 2),
        std::make_pair("hello bro   and no bro", 5),
        std::make_pair("      ", 0),
        std::make_pair("", 0),
        std::make_pair(" f d", 2),
        std::make_pair("\n  Ay Arnold!  Hello .", 4),
    };

    for (const auto &sample : samples) {
        int wordNumberInFact = sample.second;
        int wordNumberCalculated = countWords(sample.first);

        if (wordNumberInFact == wordNumberCalculated) {
            std::cout << "PASSED: " << sample.first << std::endl;
        } else {
            std::cout << "FAILED: " << sample.first << ". Fact: " << wordNumberInFact << "; calculated: " << wordNumberCalculated << std::endl;
        }
    }

    return 0;
}

bool isSeparator(char symbol) {
    for (char separator : separators) {
        if (separator == symbol) {
            return true;
        }
    }

    return false;
}

int countWords(std::string line) {
    int wordNumber = 0;
    bool letterRead = false;

    for (char symbol : line) {
        if (isSeparator(symbol)) {
            if (letterRead) {
                wordNumber += 1;
                letterRead = false;
            }
        } else {
            if (!letterRead) {
                letterRead = true;
            }
        }
    }

    if (letterRead) {
        wordNumber += 1;
    }

    return wordNumber;
}
