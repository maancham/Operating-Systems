#include "serial.hpp"

void split_string(const string &s, char delim, vector<string> &elems)
{
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
        elems.push_back(item);
}

vector<vector<string> > parse_csv(string file_path)
{
    string str;
    vector<vector<string> > line_elements;
    ifstream file(file_path);
    file.seekg(0, ios::end);
    str.reserve(file.tellg());
    file.seekg(0, ios::beg);
    str.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    if (str.length() > 0)
    {
        vector<string> lines;
        split_string(str, '\n', lines);
        for (auto it : lines)
        {
            vector<string> elements_inLine;
            split_string(it, ',', elements_inLine);
            line_elements.push_back(elements_inLine);
        }
    }
    return line_elements;
}


int main(int argc, char **argv)
{
    //string genre = argv[1];
    
    
    vector<vector<string> > books_data = parse_csv(BOOKS_FILE);
    vector<vector<string> > revs_data = parse_csv(REVS_FILE);

    cout << books_data[1][1] << endl;
    


    
    

}
