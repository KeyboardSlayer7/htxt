#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stack>

struct Tag 
{
    bool multiline;
    std::string open;
    std::string close;
};

struct SpecialReturn
{
    int content_start;
    Tag tag;
};

SpecialReturn getTag(const std::unordered_map<std::string, Tag>& map, std::string& key)
{
    SpecialReturn ret;

    if (map.find(key) != map.end())
        ret = {
            static_cast<int>(key.length()), 
            map.at(key)
        };
    else
        ret = {
            0, 
            {true, "<p>\n", "</p>\n"}
        }; 
    
    return ret;
}

std::string createIndentedString(std::string& content, int indentation)
{
    std::string output = "";

    for (int i = 0; i < indentation; ++i)
        output += "\t";

    return output + content;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "[ERROR] No text file was provided!\n";
        return EXIT_FAILURE;
    }

    std::ifstream file(argv[1]);

    if (!file.is_open())
    {
        std::cout << "[ERROR] Provided text file could not be opened!\n";
        return EXIT_FAILURE;
    }

    std::unordered_map<std::string, Tag> map = 
    {
        {"#", {false, "<h1>", "</h1>\n"}},
        {"!", {false, "<img src=\"", "\" alt=\"Could not find image.\">"}}
    };

    std::string html = "";
    
    int indentation = 0;
    std::string previous = "none";
    std::stack<std::string> stk;
    std::string buffer;

    while (std::getline(file, buffer))
    {
        if (buffer.length() == 0)
            continue;

        std::string key = buffer.substr(0, 1);
        
        SpecialReturn sr = getTag(map, key); 
        Tag& tag = sr.tag;

        if (tag.open != previous)
        {
            if (indentation > 0)
                indentation--;
            
            if (!stk.empty())
            {
                std::string ctag = stk.top();
                html += createIndentedString(ctag, indentation);
                stk.pop();
            }

            html += createIndentedString(tag.open, indentation);
            
            stk.push(tag.close);
            previous = tag.open;

            if (tag.multiline)
                indentation++;
        }

        std::string content = buffer.substr(sr.content_start, buffer.length() - sr.content_start);
        
        if (tag.multiline)
            html += createIndentedString(content, indentation) + "\n";
        else
            html += content;
    }

    if (!stk.empty())
    {
        // NOTE: THIS MAY CAUSE ISSUES IN THE FUTURE, BEWARE!!!!!!!
        indentation--;

        std::string ctag = stk.top();
        html += createIndentedString(ctag, indentation);
        stk.pop();
    }

    std::cout << html << "\n";

    file.close();

    return 0;
}
