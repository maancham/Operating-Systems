#include "serial.hpp"

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
    ifstream file(BOOKS_FILE);
    int portion = filesize(BOOKS_FILE) / 4;
	int thread_id = (intptr_t) tid;
    int begin;
    if (thread_id == 0)
        begin = 0;
    else
        begin = thread_id * portion + 1;
    
    file.seekg(begin);
    char* readed = new char[portion];
    file.read(readed, portion);
    file.close();

	cout << readed[0] << endl;
	pthread_exit((void*)thread_id);
}

int main(int argc, char **argv)
{
    string genre = argv[1];
    
    pthread_t threads[NUMBER_OF_THREADS];

    
    int return_code;
	void* status;

    for(int tid = 0; tid < NUMBER_OF_THREADS; tid++)
	{
		return_code = pthread_create(&threads[tid], NULL,
				busy_work_books, (void*)tid); 

		if (return_code)
		{
			printf("ERROR; return code from pthread_create() is %d\n",
					return_code);
			exit(-1);
		}
	}
    for(int tid = 0; tid < NUMBER_OF_THREADS; tid++)
	{
		return_code = pthread_join(threads[tid], &status);
	}

	pthread_exit(NULL);

    // vector<vector<string> > books_data = parse_csv(BOOKS_FILE);
    // vector<vector<string> > revs_data = parse_csv(REVS_FILE);

    //find_top(books_data, revs_data, genre);
}
