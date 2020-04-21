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

string find_top(vector<vector<string> > books_data, vector<vector<string> > revs_data, string genre)
{
    map<string, int> id_to_index;
    vector<vector<string> > candidates;
    vector<int> points;
    vector<int> num_of_likes;
    string top_id = "";
    for(int i=0; i<books_data.size(); i++)
    {
        if ((books_data[i][2] == genre) || (books_data[i][3] == genre))
        {
            vector <string> temp;
            temp.push_back(books_data[i][0]);
            temp.push_back(books_data[i][1]);
            temp.push_back(books_data[i][6]);
            candidates.push_back(temp);
            points.push_back(0);
            num_of_likes.push_back(0);
            id_to_index.insert({ temp[0], (candidates.size() - 1)});
        }
    }

    for(int i=0; i<revs_data.size(); i++)
    {
        if(id_to_index.find(revs_data[i][0]) != id_to_index.end())
        {
            int x,y;
            int index = id_to_index.find(revs_data[i][0])->second;
            stringstream geek(revs_data[i][1]); 
            stringstream seek(revs_data[i][2]);
            geek >> x;
            seek >> y;
            points[index] += x*y;
            num_of_likes[index] += y;
        }
    }

    for(int i=0; i<points.size(); i++)
    {
        stringstream geek(candidates[i][2]);
        int x = 0;
        geek >> x;
        points[i] /= num_of_likes[i];
        points[i] +=  x;
    }

    int max_index = distance(points.begin(), max_element (points.begin(),points.end()));

    for(int i=0; i<candidates.size(); i++)
    {
        for(int j=i+1; j<candidates.size(); j++)
        {
            if (points[j] > points[i])
            {
                string temp = candidates[i][1];
                candidates[i][1] = candidates[j][1];
                candidates[j][1] = temp;
            }
        }
    }
    cout << candidates[0][1] << "-" << candidates[1][1] << candidates[2][1] << endl;
    //return candidates[max_index][1];

    return "kir"; 
}


int main(int argc, char **argv)
{
    string genre = argv[1];
    
    
    vector<vector<string> > books_data = parse_csv(BOOKS_FILE);
    vector<vector<string> > revs_data = parse_csv(REVS_FILE);


    cout << find_top(books_data, revs_data, genre) << endl;
    


    
    

}
