#include "header.hpp"

template <typename Out>
void split(const string &s, char delim, Out result) {
    istringstream iss(s);
    string item;
    while (getline(iss, item, delim)) {
        *result++ = item;
    }
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}

void set_map()
{
    mymap[1] = "1";
    mymap[2] = "2";
    mymap[3] = "3";
    mymap[4] = "4";
    mymap[5] = "5";
    mymap[6] = "6";
    mymap[7] = "7";
    mymap[8] = "8";
    mymap[9] = "9";
    mymap[10] = "10";
    mymap[11] = "11";
    mymap[12] = "12";
    mymap[13] = "13";
    mymap[14] = "14";
    mymap[15] = "15";
    mymap[16] = "16";
    mymap[17] = "17";
}

int find_index(char key)
{
    if (key == 'N')
        return 5;
    else if(key == 'E')
        return 6;
    else if(key == 'J')
        return 7;
    else if(key == 'O')
        return 8;
    else
        return 9; 
}

vector<string> get_input(string user_input)
{
    string temp;
    vector<string> raw_input, detailed_input;
    for(int i=0; i<7; i++)
    {
        detailed_input.push_back("-");
    }
    // getline(cin, user_input);
    // if (user_input == "quit")
    //     return detailed_input;
    stringstream X(user_input); 
    while (getline(X, temp, ' ')) 
    { 
        raw_input.push_back(temp);
    }
    for (int j=0; j<raw_input.size(); j++)
    {
        if(raw_input[j] == "Name")
            detailed_input[0] = raw_input[j+2];
        else if (raw_input[j] == "Platform")
            detailed_input[1] = raw_input[j+2];
        else if(raw_input[j] == "Year")
            detailed_input[2] = raw_input[j+2];
        else if(raw_input[j] == "Genre")
            detailed_input[3] = raw_input[j+2];
        else if(raw_input[j] == "Publisher")
            detailed_input[4] = raw_input[j+2];
        else if(raw_input[j].find("Sales") != std::string::npos)
        {
            detailed_input[5] = raw_input[j];
            detailed_input[6] = raw_input[j+2];
        }
        else if(raw_input[j] == "processes")
            detailed_input.push_back(raw_input[j+2]);
        else if(raw_input[j] == "dir")
            detailed_input.push_back(raw_input[j+2]);            
    }
    return detailed_input;
}

string remove_spaces(string str)  
{ 
    str.erase(remove(str.begin(), str.end(), ' '), str.end()); 
    return str; 
}

bool check_game_ok(vector<string> readed, vector<string>needed)
{
    int size = needed.size();
    int count = 0;
    for(int i=0; i<size; i++)
    {
        if(needed[i] == "-")
        {
            count++;
            continue;
        }
        else if(needed[i] == readed[i])
            count++;
    }
    if(count == (size))
        return true;
    return false;
}

void parse_file(int start, int end, char *key, string directory, vector<string> &ans)
{
    string fixed_header = "/home/houmaan/Desktop/process/" + directory + "/dataset"; 
    string str(key);
    vector<string> fields = split(str, '|');
    fields.resize(WORKER_ARGS);
    
    vector<int> file_exts;
    int space = end - start + 1;
    for(int i=0; i < space; i++)
    {
        file_exts.push_back(start + i);
    }
    
    int match_count = 0;
    for(int i=0; i<space; i++)
    {
        string line;
        string file_number = mymap[file_exts[i]];
        string file_name = fixed_header + file_number;
        ifstream infile;
        infile.open(file_name.c_str());
        while (getline(infile, line))
        {
            string old_line = line;
            line = remove_spaces(line);
            vector<string> readed = split(line, '-');
            bool check = check_game_ok(readed, fields);
            if(check == true)
            {
                match_count++;
                ans.push_back(old_line);

            }
        }
        infile.close();
    }
}



int find_last_wall(char *response)
{
    int index = 0;
    for(int i=0; i<BUFFER_SIZE; i++)
    {
        if (response[i] == '|')
            index = i;
    }
    return (index+1);
}

vector<int> divide_files(int num_of_procs)
{
    vector<int> each_process;
    int chunk = FILES_NUM / num_of_procs;
    for(int i=0; i< (num_of_procs-1); i++)
    {
        int temp = chunk * (i+1);
        each_process.push_back(temp);
    }
    each_process.push_back(FILES_NUM);
    return each_process;
}

void send_nums_to_presenter(int num_of_procs, char *myfifo)
{
    string s = to_string(num_of_procs);
    char const *pchar = s.c_str();
    int named_fd = open(myfifo, O_WRONLY);
    write(named_fd, pchar, strlen(pchar)+1); 
    close(named_fd);
}

void send_sort_method(char sort_by[], char *myfifo)
{
    int named_fd = open(myfifo, O_WRONLY);
    write(named_fd, sort_by, strlen(sort_by)+1); 
    close(named_fd);
}

char *convert(const string & s)
{
   char *pc = new char[s.size()+1];
   strcpy(pc, s.c_str());
   return pc; 
}

int main(int argc, char **argv)
{
    set_map();
    int round_cound = 0;

    while(true)
    {
        vector<string> detailed_input;
        vector<int> proccess_bonds;
        int num_of_procs;
        string worker_details[WORKER_ARGS];
        char sort_by[2] = {'\0'};
        string user_input;
        getline(cin, user_input);
        if(user_input == "quit")
        {
            return 0;
        }
        remove("myfifo");
        char * myfifo = "myfifo";
        mkfifo(myfifo, 0666);

        detailed_input = get_input(user_input);
        stringstream geek(detailed_input[detailed_input.size() - 2]);  
        geek >> num_of_procs; 

        for (int j=0; j<WORKER_ARGS; j++)
            worker_details[j] = detailed_input[j] + "|";

        proccess_bonds = divide_files(num_of_procs);

        if(detailed_input.size() > 7)
        {    
            sort_by[0] = detailed_input[5][0];
            sort_by[1] = detailed_input[6][0];
        }
        else
        {
            sort_by[0] = '$';
            sort_by[1] = '$';
        }
        
        int pre_fd[2];
        if(pipe(pre_fd) == -1) 
        {
            cerr << "pipe creation failed!" << endl;
            return 1;
        }

        pid_t presenter = fork();
        if (presenter < 0) 
        {
            cerr << "fork failed!" << endl;
            return 1;
        }
        if (presenter == 0)
        {
            //presenter handling
            int worker_pipe = open(myfifo,O_RDONLY, 0);
            char first_message[BUFFER_SIZE];
            int process_count;
            int gathered = 0;
            char sort_key[2];
            vector<string> output;
            //read(worker_pipe, first_message, BUFFER_SIZE);
            close(pre_fd[WRITE_END]);
            read(pre_fd[READ_END], first_message, BUFFER_SIZE);
            sort_key[0] = first_message[0];
            sort_key[1] = first_message[1];
            char temp[sizeof(first_message)];
            for (int i=3; i <sizeof(first_message) - 1; i++)
                temp[i-3] = first_message[i];
            stringstream str(temp);  
            str >> process_count;
            while(gathered < process_count)
            {
                string messy_input;
                char message[NPIPE_SIZE];
                message[NPIPE_SIZE] = {'\0'};   
                if (read(worker_pipe, message, NPIPE_SIZE) <= 0)
                    continue;
                else
                {
                    char worker_id = message[strlen(message) - 1];
                    messy_input = message;
                    string reshaped = messy_input.substr(0, messy_input.size()-1);
                    //cout << worker_id << endl;
                    output.push_back(reshaped);
                    //cout << reshaped << endl;
                    gathered++;
                }   
            }
            close(worker_pipe);
            if (sort_key[0] == '$')
            {
                for(int i=0; i<output.size(); i++)
                    cout << output[i];
            }
            else
            {
                int key_index = find_index(sort_key[0]);
                vector<string> inp;
                for (int i=0; i<output.size(); i++)
                {
                    vector<string> temp = split(output[i], '\n');
                    for (int j=0; j<temp.size(); j++)
                        inp.push_back(temp[j]);
                }
                for(int i=0; i< inp.size(); i++)
                {
                    for(int j=i+1; j<inp.size(); j++)
                    {
                        string fline = inp[i];
                        string sline = inp[j];
                        fline = remove_spaces(fline);
                        sline = remove_spaces(sline);
                        vector<string> ftemp = split(fline, '-');
                        vector<string> stemp = split(sline, '-');
                        double num1 = atof(ftemp[key_index].c_str());
                        double num2 = atof(stemp[key_index].c_str());
                        if (num1 > num2)
                        {
                            string temp = inp[i];
                            inp[i] = inp[j];
                            inp[j] = temp;
                        }  
                    }
                }
                if (sort_key[1] == 'a')
                {
                    for (int i=0; i<inp.size(); i++)
                        cout << inp[i] << endl;
                }
                else
                {
                    for (int i=(inp.size()-1); i>=0; i--)
                        cout << inp[i] << endl;;
                }
            }
        }
        else
        {
            string s = to_string(num_of_procs);
            string k;
            string t1(1, sort_by[0]);
            string t2(1, sort_by[1]);
            k.append(t1);
            k.append(t2);
            k.append("-");
            k.append(s);
            char const *kchar = k.c_str();
            //int named_fd = open(myfifo, O_WRONLY);
            //write(named_fd, kchar, strlen(kchar)+1);
            write(pre_fd[WRITE_END], kchar, strlen(kchar)+1); 
            //close(named_fd);

            // worker handling
            for(int i=0; i<num_of_procs; i++) 
            {
                int fd[2];
                if(pipe(fd) == -1) 
                {
                    cerr << "pipe creation failed!" << endl;
                    return 1;
                }

                pid_t pid = fork();
                if (pid < 0) 
                {
                    cerr << "fork failed!" << endl;
                    return 1;
                }

                if (pid == 0)
                {
                    //cout << "child:" << endl;
                    int id, start_file, end_file, last_index, named_fd;
                    vector<string> ans;
                    string transmit;
                    char response[BUFFER_SIZE];
                    close(fd[WRITE_END]);
                    read(fd[READ_END], &id, sizeof(id));
                    if(id == 0)
                    {
                        start_file = 1;
                        end_file = proccess_bonds[id];
                    }
                    else
                    {
                        start_file = proccess_bonds[id - 1] + 1;
                        end_file = proccess_bonds[id];
                    }
                    response[BUFFER_SIZE] = {'\0'};
                    read(fd[READ_END], response, BUFFER_SIZE);
                    last_index = find_last_wall(response);
                    parse_file(start_file, end_file, response, detailed_input[detailed_input.size() - 1], ans);
                    named_fd = open(myfifo, O_WRONLY);
                    for(int i=0; i<ans.size(); i++)
                    {
                        transmit.append(ans[i]);
                        transmit.append("\n");
                    }
                    transmit.append(to_string(id));
                    int n = transmit.length(); 
                    char char_array[n + 1] = {'\0'}; 
                    strcpy(char_array, transmit.c_str());
                    write(named_fd, char_array, strlen(char_array) + 1); 
                    close(named_fd);
                    break;
                    
                }
                else if (pid > 0)
                {
                    int *stat_loc;
                    write(fd[WRITE_END], &i, sizeof(i));
                    for (int j=0; j<WORKER_ARGS; j++)
                    {
                        int size = worker_details[j].size();
                        char temp[size];
                        for (int k=0; k<size; k++)
                            temp[k] = worker_details[j][k];
                        write(fd[WRITE_END], temp, size*sizeof(char));
                    }
                    //wait(NULL);
                    //pid_t child_status = wait(stat_loc);
                    
                }
            }
            int *stat_loc;
            //pid_t child_status = wait(stat_loc);
            //wait(NULL);
        }
    }

    return 0;
}