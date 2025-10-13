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

struct TagContentInfo 
{
    int content_start;
    Tag tag;
};

TagContentInfo getTag(const std::unordered_map<std::string, Tag>& map, std::string& key)
{
    TagContentInfo ret;

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
        {"##", {false, "<h2>", "</h2>\n"}},
        {"###", {false, "<h3>", "</h3>\n"}},
        {"####", {false, "<h4>", "</h4>\n"}},
        {"!", {false, "<img src=\"", "\" alt=\"Could not find image.\">\n"}}
    };

    std::string html = 
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "\t<meta charset=\"UTF-8\">\n"
        "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "\t<title>Untitled Page</title>\n"
        "\t<link rel=\"stylesheet\" href=\"styles.css\">\n"
        "</head>\n"
        "<body>\n";
    
    int indentation = 1;
    std::string previous = "none";
    std::stack<Tag> stk;
    std::string buffer;

    while (std::getline(file, buffer))
    {
        if (buffer.length() == 0)
        {
            if (!stk.empty())
            {
                if (indentation > 1)
                    indentation--;

                Tag t = stk.top();

                if (t.multiline)
                    html += createIndentedString(t.close, indentation);
                else
                    html += t.close;

                stk.pop();

                previous = "none";
            }
            continue;
        }

        
        int key_length = 1;
        if (buffer[0] == '#')
        {
            while (buffer[key_length] == '#')
            {
                key_length++;
            }
        }

        std::string key = buffer.substr(0, key_length);
        
        TagContentInfo tci = getTag(map, key); 
        Tag& tag = tci.tag;

        if (tag.open != previous)
        {
            if (indentation > 1)
                indentation--;
            
            if (!stk.empty())
            {
                Tag ctag = stk.top();
                if (ctag.multiline)
                    html += createIndentedString(ctag.close, indentation);
                else
                    html += ctag.close;
                stk.pop();
            }

            html += createIndentedString(tag.open, indentation);
            
            stk.push(tag);
            previous = tag.open;

            if (tag.multiline)
                indentation++;
        }

        std::string content = buffer.substr(tci.content_start, buffer.length() - tci.content_start);
        
        if (tag.multiline)
            html += createIndentedString(content, indentation) + "\n";
        else
            html += content;
    }

    if (!stk.empty())
    {
        // NOTE: THIS MAY CAUSE ISSUES IN THE FUTURE, BEWARE!!!!!!!
        indentation--;

        Tag ctag = stk.top();
        html += createIndentedString(ctag.close, indentation);
        stk.pop();
    }
    
    html += 
        "</body>\n"
        "</html>\n";

    std::cout << html << "\n";

    file.close();

    std::string new_file_name = std::string(argv[1]);
    new_file_name = new_file_name.substr(0, new_file_name.length() - 3) + "html";

    std::ofstream output(new_file_name);

    output << html;

    output.close();

    return 0;
}
