#include "matplotlibcpp.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <vector>

namespace plt = matplotlibcpp;

using namespace plt;
using namespace std; 

#define workers 4
#define directory "data_test/"

void read_directory(const string& name, vector<string>& v)
{
    DIR* dirp = opendir(name.c_str());
    if(dirp == NULL){
        cout << "Error Filename" << endl;
        exit(1);
}
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        if(strcmp(dp->d_name,"..") && strcmp(dp->d_name,"."))
                v.push_back(dp->d_name);
}
closedir(dirp);
}

void add_data(vector<string> files, vector<vector<int>> &data){

        string type;
        string line;
        string tmp_data;
        vector<int> tmp;

        for(auto f : files){

                ifstream filename(directory + f);

                while(filename.eof() == 0){
                        getline(filename,type,',');
                        if(type.compare("t") == 0 ){
                                // delete line of total
                                getline(filename,line);
                        }
                        if(type.compare("w") == 0){
                                // id worker
                                getline(filename,tmp_data,',');
                                tmp.push_back(stoi(tmp_data));
                                // numbers of clauses
                                string nb_clauses;
                                getline(filename,tmp_data,',');
                                tmp.push_back(stoi(tmp_data));
                                // numbers of doublons
                                string nb_doublons;
                                getline(filename,tmp_data);
                                tmp.push_back(stoi(tmp_data));
                                // Add datas 
                                data.push_back(tmp);
                                // remove vector's data
                                tmp.clear();
                        }
                        if(type.compare("dw") == 0){
                                string nb_workers;
                                getline(filename,nb_workers,',');
                                string nb_clauses_workers;
                                getline(filename,nb_clauses_workers);     
                        }
                        if(type.compare("dd") == 0){
                                string nb_distribued_doublons;
                                getline(filename,nb_distribued_doublons,',');
                                string nb_clauses_distribued_workers;
                                getline(filename, nb_clauses_distribued_workers);
                        }
                        if(type.compare("r") == 0){
                                filename.close();
                        }
                }
        }
}


vector<string> add_labels(){

        vector<string> labels;
        for(int i=0;i<workers;i++){
                labels.push_back(to_string(i));
        }
        return labels;
}


void split_clauses_doublons(vector<vector<int>> data,vector<int> tab_clauses[],vector<int> tab_doublons[]){

        for (int i = 0; i < data.size(); i++)
        {
                tab_clauses[data[i][0]].push_back(data[i][1]);
                tab_doublons[data[i][0]].push_back(data[i][2]);
        }
}

int main() {

        vector<string> files;
        vector<vector<int>> data;

        read_directory(directory,files);
        add_data(files,data);

        vector<int> tab_clauses[workers];
        vector<int> tab_doublons[workers];
        // split between Id workers : Clauses/doublons
        split_clauses_doublons(data,tab_clauses,tab_doublons);

        double sum_clauses = 0;
        double sum_doublons = 0;

        vector<vector<double>> data_boxplot;
        vector<double> tmp_data;

        cout <<"------------------------\n";
        for(int i = 0; i< workers ; i++){
                
                sum_clauses = 0;
                sum_doublons = 0;
                
                auto id = tab_doublons[i].begin();
                for( auto it = tab_clauses[i].begin(); it != tab_clauses[i].end();it++){

                        sum_clauses = sum_clauses + *it;
                        sum_doublons = sum_doublons + *id;

                        double doublons = *id;
                        double clauses = *it;
                        double pourcentage = (doublons/clauses)*100; 

                        tmp_data.push_back(pourcentage);
                        id++;
                }

                data_boxplot.push_back(tmp_data);
                tmp_data.clear();

                cout << "Worker: " << i << "\n" << "number of doublons: " << sum_doublons << "\n" << "number of clauses: " << sum_clauses << "\n------------------------"<<endl;
        } 

        boxplot(data_boxplot,add_labels());
        //ylim(0.0,0.05);
        xlabel("Id of workers");
        ylabel("Percentage of duplicate");
        show();

        return 0;
}