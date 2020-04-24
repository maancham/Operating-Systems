#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <bits/stdc++.h> 
#include <typeinfo>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include <unistd.h>
#include <list>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fstream>
#include <iosfwd>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <map>
#include <algorithm>

using namespace std;

#define BOOKS_FILE "./datasets/books.csv"
#define REVS_FILE "./datasets/reviews.csv"
#define TEN 10.00
#define TWO 2
#define ZERO 0.00
#define NUMBER_OF_THREADS 2


pthread_mutex_t mutex_sum;

string books_data, revs_data, genre;
vector<vector<string> > all_books;
vector<vector<string> > all_revs;
vector<vector<string> > candidates;
vector<double> points;
vector<double> num_of_likes;
unordered_map<string, int> id_to_index;


