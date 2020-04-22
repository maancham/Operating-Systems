#include "serial.hpp"

char return_int(int id)
{
    switch(id) 
    {
        case 1:
            return '1';
            break;
        case 2:
            return '2';
            break;
        case 3:
            return '3';
            break;
        case 4:
            return '4';
            break;
        default:
            return '0';
    }
}

void split_string(const string &s, char delim, vector<string> &elems)
{
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
        elems.push_back(item);
}

// vector<vector<string> > parse_csv(string file_path)
// {
//     string str;
//     vector<vector<string> > line_elements;
//     ifstream file(file_path);
//     file.seekg(0, ios::end);
//     str.reserve(file.tellg());
//     file.seekg(0, ios::beg);
//     str.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
//     if (str.length() > 0)
//     {
//         vector<string> lines;
//         split_string(str, '\n', lines);
//         for (auto it : lines)
//         {
//             vector<string> elements_inLine;
//             split_string(it, ',', elements_inLine);
//             line_elements.push_back(elements_inLine);
//         }
//     }
//     return line_elements;
// }

vector<vector<string> > parse_csv(string input)
{
    vector<vector<string> > line_elements;
    vector<string> lines;
    split_string(input, '\n', lines);
    for (auto it : lines)
    {
        vector<string> elements_inLine;
        split_string(it, ',', elements_inLine);
        line_elements.push_back(elements_inLine);
    }
    return line_elements;
}



void print_output(vector<string> winner, double final_points)
{
    cout << "id: " << winner[0] << endl;
    cout << "Title: " << winner[1] << endl;
    cout << "Genres: " << winner[2] << ", " << winner[3] << endl;
    cout << "Number of Pages: " << winner[4] << endl;
    cout << "Author: " << winner[5] << endl;
    cout << "Average Rating: ";
    cout << fixed;
    cout << setprecision(2);
    cout << final_points << endl;
}

void get_revs_data(vector<vector<string> > revs_data, vector<double> &points, 
                       vector<double> &num_of_likes, unordered_map<string, int> id_to_index)
{
    for(int i=0; i<revs_data.size(); i++)
    {
        if(id_to_index.find(revs_data[i][0]) != id_to_index.end())
        {
            double x,y;
            int index = id_to_index.find(revs_data[i][0])->second;
            stringstream geek(revs_data[i][1]); 
            stringstream seek(revs_data[i][2]);
            geek >> x;
            seek >> y;
            points[index] += x*y;
            num_of_likes[index] += y;
        }
    }
}

int find_max_index(vector<double> &points)
{
    return distance(points.begin(), max_element (points.begin(),points.end()));
}

vector<string> extract_winner(vector<vector<string> > books_data, int row_number)
{
    vector<string> winner;
    for(int i=0; i<books_data[row_number].size(); i++)
        winner.push_back(books_data[row_number][i]);
    
    return winner;
}

void find_top(vector<vector<string> > books_data, vector<vector<string> > revs_data, string genre)
{
    int row_number;
    unordered_map<string, int> id_to_index;
    vector<vector<string> > candidates;
    vector<double> points;
    vector<double> num_of_likes;

    for(int i=0; i<books_data.size(); i++)
    {
        if ((books_data[i][2] == genre) || (books_data[i][3] == genre))
        {
            vector <string> temp;
            temp.push_back(books_data[i][0]);
            temp.push_back(books_data[i][1]);
            temp.push_back(books_data[i][6]);
            temp.push_back(to_string(i));
            candidates.push_back(temp);
            points.push_back(0);
            num_of_likes.push_back(0);
            id_to_index.insert({ temp[0], (candidates.size() - 1)});
        }
    }

    get_revs_data(revs_data, points, num_of_likes, id_to_index);

    for(int i=0; i<points.size(); i++)
    {
        stringstream geek(candidates[i][2]);
        double x;
        geek >> x;
        points[i] /= num_of_likes[i];
        points[i] +=  x;
        points[i] /= TEN;
    }

    int max_index = find_max_index(points);
    stringstream geek(candidates[max_index][3]);
    geek >> row_number;
    vector<string> winner = extract_winner(books_data, row_number);
    print_output(winner, points[max_index]);

}

ifstream::pos_type filesize(const char* filename)
{
    ifstream in(filename, ifstream::ate | ifstream::binary);
    return in.tellg(); 
}

void* busy_work_books(void* tid)
{
    int thread_id = (intptr_t) tid;
    int portion, begin;
    ifstream file;
    if (thread_id % 2 == 0)
    {
        file.open(BOOKS_FILE);
        portion = filesize(BOOKS_FILE) / NUMBER_OF_THREADS;
    }
    else
    {
        file.open(REVS_FILE);
        portion = filesize(REVS_FILE) / NUMBER_OF_THREADS;
    }
    
    if (thread_id == 0 || thread_id == 1)
        begin = 0;
    else
        begin = portion + 1;
    
    //cout << begin << endl;

    file.seekg(begin);
    char* readed = new char[portion];
    file.read(readed, portion);
    file.close();

    //readed[strlen(readed)] = return_int(thread_id);

    //string str(readed);

    // pthread_mutex_lock (&mutex_sum);
    // for (int i=begin; i < begin + portion; i++)
    // {
    //     revs[i] = 'c';
    // }
    // pthread_mutex_unlock (&mutex_sum); 

	pthread_exit((void*)readed);
}

int main(int argc, char **argv)
{
    string genre = argv[1];
    
    pthread_t threads[2 * NUMBER_OF_THREADS];

    char *revs = new char[filesize(REVS_FILE)];
    char *books = new char[filesize(BOOKS_FILE)];

    string revs_data;
    string books_data;
    //cout << filesize(BOOKS_FILE) << endl;

    int return_code;
	void* status;

    //pthread_mutex_init(&mutex_sum, NULL);
    for(int tid = 0; tid < 2 * NUMBER_OF_THREADS; tid++)
	{
		return_code = pthread_create(&threads[tid], NULL,
				busy_work_books, (void*)tid); 
	}

    for(int tid = 0; tid < 2 * NUMBER_OF_THREADS; tid++)
	{
		return_code = pthread_join(threads[tid], &status);
        string str((char*) status);
        if (tid % 2 == 0)
            books_data.append(str);
        else
            revs_data.append(str);
        
	}

	//pthread_exit(NULL);

    //cout << revs_data.size() << " - " << books_data.size() << endl;

    vector<vector<string> > all_books = parse_csv(books_data);
    vector<vector<string> > all_revs = parse_csv(revs_data);

    find_top(all_books, all_revs, genre);
}
