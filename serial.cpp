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


void print_output(vector<string> &winner, double final_points)
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

void get_revs_data(vector<vector<string> > &revs_data, vector<double> &points, 
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

vector<string> extract_winner(vector<vector<string> > &books_vector, int row_number)
{
    vector<string> winner;
    for(int i=0; i<books_vector[row_number].size(); i++)
        winner.push_back(books_vector[row_number][i]);
    
    return winner;
}

void find_top(vector<vector<string> > &books_vector, vector<vector<string> > &revs_data, string genre)
{
    int row_number;
    unordered_map<string, int> id_to_index;
    vector<vector<string> > candidates;
    vector<double> points;
    vector<double> num_of_likes;

    for(int i=0; i<books_vector.size(); i++)
    {
        if ((books_vector[i][2] == genre) || (books_vector[i][3] == genre))
        {
            vector <string> temp;
            temp.push_back(books_vector[i][0]);
            temp.push_back(books_vector[i][1]);
            temp.push_back(books_vector[i][6]);
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
        if (num_of_likes[i] != 0)
            points[i] /= num_of_likes[i];
        else
            points[i] = 0;
        points[i] +=  x;
        points[i] /= TEN;
    }

    int max_index = find_max_index(points);
    stringstream geek(candidates[max_index][3]);
    geek >> row_number;
    vector<string> winner = extract_winner(books_vector, row_number);
    print_output(winner, points[max_index]);

}

ifstream::pos_type filesize(const char* filename)
{
    ifstream in(filename, ifstream::ate | ifstream::binary);
    return in.tellg(); 
}

void* parse_parallel(void* tid)
{
    int thread_id = (intptr_t) tid;
    vector<string> lines;
    if (thread_id % 2 == 0)
        split_string(books_data, '\n', lines);
    else
        split_string(revs_data, '\n', lines);  
    
    for (auto it : lines)
    {
        vector<string> elements_inLine;
        split_string(it, ',', elements_inLine);
        if (thread_id % 2 == 0)
            all_books.push_back(elements_inLine);
        else
            all_revs.push_back(elements_inLine);
    }
    pthread_exit((void*)thread_id);
}

void* read_parallel(void* tid)
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

    file.seekg(begin);
    char* readed = new char[portion];
    file.read(readed, portion);
    file.close();

	pthread_exit((void*)readed);
}


int main(int argc, char **argv)
{
    string genre = argv[1];
    
    pthread_t threads[2 * NUMBER_OF_THREADS];
    pthread_t parse_threads[2];

    int return_code;
	void* status;

    for(int tid = 0; tid < 2 * NUMBER_OF_THREADS; tid++)
	{
		return_code = pthread_create(&threads[tid], NULL,
				read_parallel, (void*)tid); 
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

    for(int tid = 0; tid < TWO; tid++)
	{
		return_code = pthread_create(&parse_threads[tid], NULL,
				parse_parallel, (void*)tid); 
	}

    for(int tid = 0; tid < TWO; tid++)
	{
        return_code = pthread_join(parse_threads[tid], &status); 
	}

    find_top(all_books, all_revs, genre);
}