#include "./../header/parsing.h"

int parse(std::basic_string<char> text)
{
    using namespace boost::locale;
    using namespace boost::locale::boundary;
    generator gen;
    // Create system default locale
    std::locale loc=gen("en_US.UTF-8");
    //????? Make it system global
    std::locale::global(loc);
    std::cout.imbue(loc); //?? (making standard locale for cout)
    // Set as default locale for output
    std::map<std::string,int> vocabulary;

// Create mapping of text for token iterator using global locale.
    ssegment_index myMap(word,text.begin(),text.end());
// Define a rule
    myMap.rule(word_any);
// for loop that puts "words" -- chunks of word boundary
    for(ssegment_index::iterator it=myMap.begin();it!=myMap.end();++it)
    {
        // ???? not sure about type char
        std::basic_string<char> word;
        // normalizing the word
        word = boost::locale::normalize((std::basic_string<char>) * it, boost::locale::norm_default,
                (std::locale) gen("en_US.UTF-8"));
        // folding case of word
        word = boost::locale::fold_case(word, (std::locale) gen("en_US.UTF-8"));

        //finding the word in dict
        std::map<std::string,int>::const_iterator itr = vocabulary.find(word);
        if( itr!=vocabulary.end() )
        {
            // word found. Incrementing
            vocabulary[word] = vocabulary[word] + 1;
        }
        else
            {
            // word not found. adding new pair
            vocabulary[word] = 1;
            }
    }
    //iterating through vocabulary to print all pairs
    for(std::map<std::string, int>::const_iterator iter=vocabulary.begin();iter!=vocabulary.end();++iter)
    {
        std::cout <<iter->first << "\t\t:"<< "\t"<<iter->second <<std::endl;
    }
    return 0;
}
