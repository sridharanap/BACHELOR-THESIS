#include <bits/stdc++.h>

using namespace std;





class TLBO{
public:
    int no_of_tasks;
    int no_of_robots;
    int no_of_stations;
    int T; //no of iterations to be performed
    int p; //no of particles in the class (students)
    vector<vector<int> > population; //stores the population
    vector<int> obj_value; //holds the objective function values of each particle
    vector<vector<int> > input_data;
    vector<vector<vector<int> > > seq_dependent_time;
    vector<vector<int> > pre_matrix;
    vector<vector<int> > setup_time;
    vector<double> energy_consumption;
    unordered_map<int, unordered_set<int> > pre_graph;
    vector<int> counter; //helps tlbo to escape from local minima
    vector<int> teacher;
    vector<int> best;
    int best_value;
    int teacher_value;
    vector<int> mean;
    double r;  //coefficient that controls the influence of teacher and partner in respective phases
    double tf; //teaching factor
    double tp; //learning factor
    
    
    void get_input(){
    	//reading from the file
		ifstream file("input_11_4.txt");
		file.seekg(0);
       file>>no_of_tasks;
       file>>no_of_robots;
       vector<vector<int> > temp_input_data(2*no_of_robots + 1, vector<int>(no_of_tasks));
       vector<vector<vector<int> > > temp_seq_dependent_time(no_of_robots, vector<vector<int>>(no_of_tasks, vector<int>(no_of_tasks)));
       vector<vector<int> > temp_setup_time(no_of_robots, vector<int>(no_of_tasks));
       vector<vector<int> > temp_pre_matrix(no_of_tasks, vector<int>(no_of_tasks));
       vector<double> temp_energy_consumption(no_of_robots+1);
       input_data = temp_input_data;
       seq_dependent_time = temp_seq_dependent_time;
       setup_time = temp_setup_time;
       pre_matrix = temp_pre_matrix;
       energy_consumption = temp_energy_consumption;
       //cout<<endl<<"OPERATION TIMES FOR HRC\n";
        for(int i=0;i<(no_of_robots*2 + 1); i++){
            for(int j=0;j<no_of_tasks;j++){
                file>>input_data[i][j];
            }
        }
       //cout<<endl<<"SETUP TIMES FOR ROBOTS\n";
        for(int i=0;i<no_of_robots; i++){
            for(int j=0;j<no_of_tasks;j++){
                file>>setup_time[i][j];
            }
        }
        for(int i=0;i<no_of_robots;i++){
           //cout<<endl<<"SEQUENCE DEPENDENT TIME FOR ROBOT "<<i+1<<endl;
           for(int j=0;j<no_of_tasks;j++){
               for(int k=0;k<no_of_tasks;k++){
                   file>>seq_dependent_time[i][j][k];
               }
           }
           //cout<<endl;
       }
       //cout<<endl<<"PRECEDENCE MATRIX"<<endl;
       for(int i=0;i<no_of_tasks;i++){
           for(int j=0;j<no_of_tasks;j++){
            	file>>pre_matrix[i][j];
    		}
       }
       //cout<<endl<<"ENERGY CONSUMPTION"<<endl;
       for(int i=0;i<(no_of_robots+1);i++){
           file>>energy_consumption[i];
       }
       file.close();
    	
        no_of_stations = 4;
        T = 100;
        p = 10;
        cout<<endl<<"NUMBER OF TASKS : "<<no_of_tasks;
        cout<<endl<<"NUMBER OF ROBOTS : "<<no_of_robots;
        cout<<endl<<"NUMBER OF STATIONS : "<<no_of_stations;
        cout<<endl<<"NUMBER OF ITERATIONS : "<<T;
        cout<<endl<<"SIZE OF THE POPULATION : "<<p;
        cout<<endl;
        //printing
        cout<<endl<<"OPERATION TIMES FOR HRC\n";
        for(int i=0;i<(2*no_of_robots + 1); i++){
            for(int j=0;j<no_of_tasks;j++){
                cout<<input_data[i][j]<<" ";
            }
            cout<<endl;
        }
        //printing
        cout<<endl<<"SETUP TIMES FOR ROBOTS\n";
        for(int i=0;i<no_of_robots; i++){
            for(int j=0;j<no_of_tasks;j++){
                cout<<setup_time[i][j]<<" ";
            }
            cout<<endl;
        }
        //printing
       for(int i=0;i<no_of_robots;i++){
           cout<<endl<<"SEQUENCE DEPENDENT TIME FOR ROBOT "<<i+1<<endl;
           for(int j=0;j<no_of_tasks;j++){
               for(int k=0;k<no_of_tasks;k++){
                   cout<<seq_dependent_time[i][j][k]<<" ";
               }
               cout<<endl;
           }
           cout<<endl;
       }
        //printing
       cout<<endl<<"PRECEDENCE MATRIX"<<endl;
       for(int i=0;i<no_of_tasks;i++){
           for(int j=0;j<no_of_tasks;j++){
            cout<<pre_matrix[i][j]<<" ";
           }
           cout<<endl;
       }
        //printing
       cout<<endl<<"ENERGY CONSUMPTION"<<endl;
       for(int i=0;i<(no_of_robots+1);i++){
           cout<<energy_consumption[i]<<" ";
       }
       cout<<endl;

        
    /*gets the input from user and generates a precedence graph
        cout<<"Enter number of stations : ";
        cin>>no_of_stations;
        cout<<"Enter number of iterations : ";
        cin>>T;
        cout<<"Enter population size : ";
        cin>>p;*/
        
        r = 0.7; //assuming the co-eff to be 0.7
        tf = 0.7;
        tp = 0.8;
        best_value = INT_MAX;
        for(int i=1;i<=no_of_tasks;i++){
            //cout<<"ENTER PRECEDENCE TASKS FOR TASK "<<i<<": (enter 0 if no precedence):\n";
            for(int j=0;j<no_of_tasks;j++){
                if(pre_matrix[i-1][j]==0) continue;
                pre_graph[i].insert(j+1);
            }
        }

    }
    
    vector<int> generate_sequence_randomly(unordered_map<int, unordered_set<int> > pre){
    
        int n = 0; //number of tasks in the sequence(assigned)
        vector<int> seq;  //holds the sequence generated
        unordered_set<int> s; //available set that holds tasks that can be assigned
        unordered_set<int> assigned; //holds the already assigned tasks

        //creating the available set
        for(int i=1;i<=no_of_tasks;i++){
            if(pre[i].size()==0){
                s.insert(i);
            }
        }
        while(n < no_of_tasks){
            n = n + 1;
            int x = s.size();
            int random = rand()%x;
            unordered_set<int>::iterator it = s.begin();
            advance(it, random);
            seq.push_back(*it);
            assigned.insert(*it);
            //removing the assigned task from available set and adding the tasks that had this task as its precedence in the available set
            int task = *it;
            s.erase(it);
        
            pre.erase(task);
            for(int i=1;i<=no_of_tasks;i++){
                if(assigned.find(i)==assigned.end()){
                    if(pre[i].find(task)!=pre[i].end()) pre[i].erase(task);
                    if(pre[i].size()==0) s.insert(i);
                }
            }
        
        }

        return seq;
    }
    
    //generates population for iterations
    void generate_population(){
        for(int i=0;i<p;i++){
            population.push_back(generate_sequence_randomly(pre_graph));
        }    
        for(int i=0;i<p;i++){
            obj_value.push_back(objective_function(population[i]));
            counter.push_back(0);
        }
    }
    
    
    //objective function calculation (cycle time)
    int objective_function(vector<int> tasks){
    //(i,j) -> i defines the robot number and j defines the activity number (input_data)
    
    //tasks = given sequence of task allocation by the optimization algorithm
    
        int cycle_time = 0;  
        int no_robots = no_of_robots; //no of rows in the input_data matrix 
        int no_tasks = no_of_tasks;  //no of cols in the input_data matrix
    
    //for getting the min time possible for that allocation (using the formula given in slides)
        for(int j=0;j<no_tasks;j++){
            int min_time = INT_MAX;   //min time taken by the robots to complete the ith task
            for(int i=0;i<no_robots;i++){
                min_time = min(min_time, input_data[i][j]);
            }
            cycle_time+=min_time;
        }
        cycle_time = cycle_time / no_of_stations;
    
        //cout<<endl<<"INITIAL CYCLE TIME : "<<cycle_time<<endl;
        while(true){
        //assigning the start task for the robots
            int start = 0;
            int si;
        //iterating through each station for given cycle time
            for(int s=0;s<no_of_stations;s++){
                si = start;  //si = start index
        
            //if all tasks assigned, the while loop ends
                if(si>=no_tasks) break;
            
                int n = 0; //most no of tasks assigned to a robot in that station
                for(int  r = 0; r<no_robots; r++){
                    int t = 0; //respective time taken by that robot in that station
                    si = start;
                    while(t<cycle_time){
                        if(si>=no_tasks) break;
                        t+=input_data[r][tasks[si]-1];  
                        //adding the sequence dependent times and setup times of the robot
                        t+=setup_time[r][tasks[si]-1];
                        if(si!=start){
                            t+=seq_dependent_time[r][tasks[si]-1][tasks[si-1]-1];
                        }
                        if(t>cycle_time) break;
                        si++;
                    }
            
                    n = max(n, si-start); //(si-start)= no of tasks assigned to that particular robot
                }
            
                start+=n;
            }
        
            if(start>=no_tasks){
                break;
            }
            else cycle_time++;
        }
    
        return cycle_time;
    }
    
    
    //prints the current iteration
    void print_iteration(int t){
        cout<<endl<<"\n\nITERATION "<<t<<" :\n"<<endl;
        cout<<"population = objective function value(cycle_time) :\n";
        for(int i=0;i<p;i++){
            for(int j=0;j<no_of_tasks;j++){
                cout<<population[i][j]<<" ";
            }
            cout<<"  =  "<<obj_value[i];
            cout<<endl;
        }
        cout<<endl;
    }
    
    
    //decoding and printing the solution from obtained best solution
    void print_solution(){
        int ct = teacher_value;
        vector<int> tasks = teacher;
        if(best_value<=ct){
            ct = best_value;
            tasks = best;
        }
        cout<<endl;
        cout<<"BEST SOLUTION FOUND"<<endl;
        for(int i=0;i<no_of_tasks;i++){
            cout<<tasks[i]<<" ";
        }
        cout<<"= "<<ct<<endl;
        //assigning the start task for the robots
        int start = 0;
        int si;
        //iterating through each station for given cycle time
        for(int s=0;s<no_of_stations;s++){
            si = start;  //si = start index
        
            //if all tasks assigned, the while loop ends
            if(si>=no_of_tasks) break;
            int robo_index;
            int n = 0; //most no of tasks assigned to a robot in that station
            for(int  r = 0; r<no_of_robots; r++){
                int t = 0; //respective time taken by that robot in that station
                si = start;
                while(t<ct){
                    if(si>=no_of_tasks) break;
                    t+=input_data[r][tasks[si]-1];  
                    //adding the sequence dependent times and setup times of the robot
                    t+=setup_time[r][tasks[si]-1];
                    if(si!=start){
                        t+=seq_dependent_time[r][tasks[si]-1][tasks[si-1]-1];
                    }
                    if(t>ct) break;
                    si++;
                }
                if((si-start) > n) {
                    robo_index = r;
                    n = si-start;
                }
                
            }
            cout<<"\nstation number "<<s+1<<"'s allocation: "<<endl;
            cout<<"robot assigned : "<<robo_index+1<<endl;
            cout<<"tasks assigned : ";
            int st = 0;
            for(int i=start;i<(start+n);i++){
                cout<<tasks[i]<<" ";
                st+=input_data[robo_index][tasks[i]-1];
                //adding the sequence dependent times and setup times of the robot
                st+=setup_time[robo_index][tasks[i]-1];
                if(i!=start){
                    st+=seq_dependent_time[robo_index][tasks[i]-1][tasks[i-1]-1];
                }
            }
            cout<<endl;
            cout<<"its cycle time : "<<st;
            cout<<endl;
            start+=n;
        }
    }
    
    //finds the best solution obtained from the population
    void find_best_solution(){
        teacher_value = obj_value[0];
        teacher = population[0];
        for(int i=1;i<p;i++){
            if(obj_value[i]<teacher_value){
                teacher_value = obj_value[i];
                teacher = population[i];
            }
        }
    }
    
    
    void update_population(){
        int old_obj_value;
        for(int i=0;i<p;i++){
            old_obj_value = obj_value[i];
            teacher_phase(i);
            learner_phase(i);
            if(old_obj_value==obj_value[i]){
                counter[i] = counter[i] + 1;
            }
        }
    }
    
    //teaching phase
    void teacher_phase(int index){
        find_best_solution(); //finding the teacher(best one from population
        
        vector<int> second_term = subtract_position_position(mean, teacher);
        vector<int> new_student = normalise(add_coeff_positions(r, population[index], second_term), pre_graph);
        int new_student_value = objective_function(new_student);
        //performing greedy selection
        if(new_student_value<=obj_value[index]){
            population[index] = new_student;
            obj_value[index] = new_student_value;
            calculate_mean();
        }

    }
    
    //learning phase
    void learner_phase(int index){
        int random = index;
        while(random==index){
            random = rand()%p;
        }
        vector<int> partner = population[random];
        int partner_value = obj_value[random];
        if(partner_value<=obj_value[index]){
            vector<int> second_term = subtract_position_position(population[index], partner);
            vector<int> new_student = normalise(add_coeff_positions(r, population[index], second_term), pre_graph);
            int new_student_value = objective_function(new_student);
            //performing greedy selection
            if(new_student_value<=obj_value[index]){
                population[index] = new_student;
                obj_value[index] = new_student_value;
                calculate_mean();
            }
        }
        else{
            vector<int> second_term = add_position_position(population[index], partner);
            vector<int> new_student = normalise(add_coeff_positions(r, population[index], second_term), pre_graph);
            int new_student_value = objective_function(new_student);
            //performing greedy selection
            if(new_student_value<=obj_value[index]){
                population[index] = new_student;
                obj_value[index] = new_student_value;
                calculate_mean();
            }
            
        }
        
    }
    
    
    //takes integr average
    void calculate_mean(){
        mean.clear();
        unordered_set<int> s;
        for(int i=0;i<no_of_tasks;i++){
            s.insert(i+1);
        }
        for(int j=0;j<no_of_tasks;j++){
            double avg = 0;
            for(int i=0;i<p;i++){
                avg = avg + double(population[i][j]);
            }
            avg = avg/p;
            int x = ceil(avg);
            bool flag = false;
            if(s.find(x)!=s.end()) {
                mean.push_back(x);
                s.erase(x);
                flag = true;
            }
            if(flag) continue;
            else{
                int diff = INT_MAX;
                int closest = -1;
                for(int i=0;i<p;i++){
                    if(s.find(population[i][j])!=s.end()){
                        if(abs(x-population[i][j])<diff){
                            diff = abs(x-population[i][j]);
                            closest = population[i][j];
                        }
                    }
                }
                if(closest!=-1){
                    mean.push_back(closest);
                    s.erase(closest);
                }
                else {
                    unordered_set<int>::iterator it = s.begin();
                    int size = s.size();
                    int random = rand()%size;
                    advance(it, random);
                    mean.push_back(*it);
                    s.erase(it);
                }
            }
        }
    }
    
    
    //operators used for doing discrete TLBO
    vector<int> subtract_position_position(vector<int> x1, vector<int> x2){
        vector<int> ans;
        
        for(int i=0;i<no_of_tasks;i++){
            if(x1[i]==x2[i]){
                ans.push_back(0);
            }
            else{
                int random = rand()%11;
                if(random<(tf*10)){
                    ans.push_back(x2[i]);
                }
                else{
                    ans.push_back(0);
                }
            }
        }
        return ans;
    }

    vector<int> add_position_position(vector<int> x1, vector<int> x2){
        vector<int> ans;
        
        for(int i=0;i<no_of_tasks;i++){
            if(x1[i]==x2[i]){
                ans.push_back(x1[i]);
            }
            else{
                int random = rand()%11; 
                if(random<=(tp*10)){
                    ans.push_back(x2[i]);
                }
                else{
                    ans.push_back(0);
                }
            
            }
        }
        return ans;
    }


    vector<int> add_coeff_positions(double c, vector<int> x1, vector<int> x2){
        vector<int> ans;
        for(int i=0;i<no_of_tasks;i++){
            double random = rand()%11;
            c = c*10;
            if(random<c){
                ans.push_back(x1[i]);
            }
            else{
                ans.push_back(x2[i]);
            }
        }
        return ans;
    }
    
    
    //for normalising any given sequence according to the precedence relation
    vector<int> normalise(vector<int> pos, unordered_map<int, unordered_set<int> > pre){
        int n = 0;
        vector<int> seq; 
        unordered_set<int> s; 
        unordered_set<int> assigned; 

   
        for(int i=1;i<=no_of_tasks;i++){
            if(pre[i].size()==0){
                s.insert(i);
            }
        }
        while(n < no_of_tasks && seq.size() < no_of_tasks){
            int task;
            if(s.find(pos[n])!=s.end() && pos[n]!=0){
                seq.push_back(pos[n]);
                assigned.insert(pos[n]);
                task = pos[n];
                s.erase(pos[n]);
                n = n + 1;
            }
            else{
                int x = s.size();
                int random = pos[n]%x;
                unordered_set<int>::iterator it = s.begin();
                advance(it, random);
                seq.push_back(*it);
                assigned.insert(*it);
                task = *it;
                s.erase(it);
            }
        
            pre.erase(task);
            for(int i=1;i<=no_of_tasks;i++){
                if(assigned.find(i)==assigned.end()){
                    if(pre[i].find(task)!=pre[i].end()) pre[i].erase(task);
                    if(pre[i].size()==0) s.insert(i);
                }
            }
        }
        return seq;
    }
    
    
    //to help tlbo from trapping a local minima
    void escape_local_minima(){
        //assuming if the value of a student is not changing even after 30% of iterations, its replaced with a random sequence
        bool changed;
        for(int i=0;i<p;i++){
            changed = false;
            if(counter[i]>=floor(0.3*T)){
                changed = true;
                counter[i] = 0;
                if(obj_value[i] <= best_value){
                    best_value = obj_value[i];
                    best = population[i];
                }
                population[i] = generate_sequence_randomly(pre_graph);
                obj_value[i] = objective_function(population[i]);
            }
        }
        if(changed) calculate_mean();
    }

};

int main()
{
   srand(time(0));	
   int t = 1;
   TLBO object;
   object.get_input();
   object.generate_population();
   object.calculate_mean();
   int T = object.T;
   for(int t=1;t<=T;t++){
       object.print_iteration(t);
       object.update_population();
       object.escape_local_minima();
   }
   object.find_best_solution();
   object.print_solution();
   return 0;
}
